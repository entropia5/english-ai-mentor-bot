
#include "config.h"
#include "logger.h"
#include "database.h"
#include "telegram_client.h"
#include "groq_client.h"
#include "render_utils.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <map>
#include <tuple>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <mutex>
#include <set>
#include <filesystem>
#include <iomanip>
#include <ctime>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;
using InlineKeyboard = std::vector<std::vector<std::pair<std::string, std::string>>>;

std::string trim(const std::string& text);
fs::path bot_state_dir();
void load_bot_state();
void save_bot_state_locked();
void save_bot_state();

// empty string if markdown is invalid (to avoid Telegram errors)
std::string clean_markdown(const std::string& text) {
    std::string result;
    bool in_code_block = false;
    for (size_t i = 0; i < text.length(); i++) {
        char c = text[i];
        if (i + 2 < text.length() && text.substr(i, 3) == "```") {
            in_code_block = !in_code_block;
            result += c;
            continue;
        }
        if (in_code_block) {
            result += c;
            continue;
        }
        if (c == '*' || c == '_' || c == '`' || c == '[' || c == ']' ||
            c == '(' || c == ')' || c == '~' || c == '>' || c == '#' ||
            c == '+' || c == '-' || c == '=' || c == '|' || c == '{' || c == '}') {
            result += ' ';
        } else if (c == '\n' || c == '\r') {
            result += '\n';
        } else {
            result += c;
        }
    }
    return result;
}

std::string format_ai_response_box(const std::string& text) {
    std::string result;
    result.reserve(text.size() + 12);

    for (char c : text) {
        if (c == '`') {
            result += '\'';
        } else if (c == '\r') {
            result += '\n';
        } else {
            result += c;
        }
    }

    result = trim(result);
    if (result.empty()) {
        result = "AI не вернул текст ответа.";
    }

    return "```cpp\n" + result + "\n```";
}

std::string trim(const std::string& text) {
    size_t start = text.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";

    size_t end = text.find_last_not_of(" \t\r\n");
    return text.substr(start, end - start + 1);
}

std::vector<std::string> split_words_input(const std::string& text) {
    std::vector<std::string> words;
    std::stringstream ss(text);
    std::string item;

    while (std::getline(ss, item, ',')) {
        item = trim(item);
        if (!item.empty()) {
            words.push_back(item);
        }
    }

    return words;
}

std::string to_lower_ascii(const std::string& text) {
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return result;
}

std::string strip_list_prefix(std::string line) {
    line = trim(line);
    while (!line.empty() && (line[0] == '-' || line[0] == '*')) {
        line = trim(line.substr(1));
    }

    size_t pos = 0;
    while (pos < line.size() && std::isdigit((unsigned char)line[pos])) {
        pos++;
    }
    if (pos > 0 && pos + 1 < line.size() && (line[pos] == '.' || line[pos] == ')')) {
        line = trim(line.substr(pos + 1));
    }

    return line;
}

struct GeneratedWord {
    std::string english;
    std::string transcription;
    std::string pronunciation;
    std::string translation;
    std::string definition;
};

struct PronunciationUpdate {
    int id = 0;
    std::string transcription;
    std::string pronunciation;
};

struct DefinitionUpdate {
    int id = 0;
    std::string definition;
};

std::vector<GeneratedWord> parse_generated_words(const std::string& response) {
    std::vector<GeneratedWord> words;
    GeneratedWord current;

    std::stringstream ss(response);
    std::string line;
    while (std::getline(ss, line)) {
        line = strip_list_prefix(line);
        if (line.empty()) continue;

        size_t colon = line.find(':');
        if (colon == std::string::npos) continue;

        std::string label = to_lower_ascii(trim(line.substr(0, colon)));
        std::string value = trim(line.substr(colon + 1));

        if (label == "word") {
            if (!current.english.empty() && !current.translation.empty()) {
                words.push_back(current);
                current = GeneratedWord{};
            }
            current.english = value;
        } else if (label == "trans" || label == "transcription") {
            current.transcription = value;
        } else if (label == "pron" || label == "pronunciation") {
            current.pronunciation = value;
        } else if (label == "mean" || label == "meaning" || label == "translation") {
            current.translation = value;
        } else if (label == "def" || label == "definition" || label == "explanation") {
            current.definition = value;
        }
    }

    if (!current.english.empty() && !current.translation.empty()) {
        words.push_back(current);
    }

    return words;
}

std::vector<PronunciationUpdate> parse_pronunciation_updates(const std::string& response) {
    std::vector<PronunciationUpdate> updates;
    PronunciationUpdate current;

    auto flush_current = [&]() {
        if (current.id > 0 && (!current.transcription.empty() || !current.pronunciation.empty())) {
            updates.push_back(current);
        }
        current = PronunciationUpdate{};
    };

    std::stringstream ss(response);
    std::string line;
    while (std::getline(ss, line)) {
        line = strip_list_prefix(line);
        if (line.empty() || line == "---") continue;

        size_t colon = line.find(':');
        if (colon == std::string::npos) continue;

        std::string label = to_lower_ascii(trim(line.substr(0, colon)));
        std::string value = trim(line.substr(colon + 1));

        if (label == "id") {
            flush_current();
            try {
                current.id = std::stoi(value);
            } catch (const std::exception&) {
                current.id = 0;
            }
        } else if (label == "trans" || label == "transcription") {
            current.transcription = value;
        } else if (label == "pron" || label == "pronunciation") {
            current.pronunciation = value;
        }
    }

    flush_current();
    return updates;
}

std::vector<DefinitionUpdate> parse_definition_updates(const std::string& response) {
    std::vector<DefinitionUpdate> updates;
    DefinitionUpdate current;

    auto flush_current = [&]() {
        if (current.id > 0 && !current.definition.empty()) {
            updates.push_back(current);
        }
        current = DefinitionUpdate{};
    };

    std::stringstream ss(response);
    std::string line;
    while (std::getline(ss, line)) {
        line = strip_list_prefix(line);
        if (line.empty() || line == "---") continue;

        size_t colon = line.find(':');
        if (colon == std::string::npos) continue;

        std::string label = to_lower_ascii(trim(line.substr(0, colon)));
        std::string value = trim(line.substr(colon + 1));

        if (label == "id") {
            flush_current();
            try {
                current.id = std::stoi(value);
            } catch (const std::exception&) {
                current.id = 0;
            }
        } else if (label == "def" || label == "definition" || label == "meaning") {
            current.definition = value;
        }
    }

    flush_current();
    return updates;
}

std::string build_existing_words_hint(const std::vector<WordView>& words) {
    if (words.empty()) return "";

    std::string hint = "\nAvoid these existing words for this user: ";
    int count = 0;
    for (const auto& word : words) {
        if (count > 0) hint += ", ";
        hint += std::get<0>(word);
        count++;
        if (count >= 80) break;
    }
    hint += ".";
    return hint;
}

std::string build_seen_generated_words_hint(const std::set<std::string>& words) {
    if (words.empty()) return "";

    std::string hint = "\nAlso avoid these already proposed candidates: ";
    int count = 0;
    for (const std::string& word : words) {
        if (count > 0) hint += ", ";
        hint += word;
        count++;
        if (count >= 80) break;
    }
    hint += ".";
    return hint;
}

std::string english_level_from_learned(int learned) {
    if (learned < 300) return "A1";
    if (learned < 600) return "A2";
    if (learned < 1000) return "B1";
    if (learned < 1500) return "B2";
    return "C1";
}

std::mutex g_screen_mutex;
enum class ScreenMessageType {
    Unknown,
    Text,
    Photo
};

struct ScreenMessageState {
    int message_id = 0;
    ScreenMessageType type = ScreenMessageType::Unknown;
};

std::map<long long, ScreenMessageState> g_active_screen_message;
std::map<long long, bool> g_reply_keyboard_removed;
std::map<long long, int> g_last_ai_user_message;
std::map<long long, int> g_broadcast_hint_message;
std::map<long long, std::string> g_chat_language;
std::map<long long, std::string> g_screen_context;

int get_active_screen_message(long long chat_id) {
    std::lock_guard<std::mutex> lock(g_screen_mutex);
    auto it = g_active_screen_message.find(chat_id);
    return it == g_active_screen_message.end() ? 0 : it->second.message_id;
}

ScreenMessageType get_active_screen_message_type(long long chat_id) {
    std::lock_guard<std::mutex> lock(g_screen_mutex);
    auto it = g_active_screen_message.find(chat_id);
    return it == g_active_screen_message.end() ? ScreenMessageType::Unknown : it->second.type;
}

std::string screen_message_type_to_string(ScreenMessageType type) {
    switch (type) {
        case ScreenMessageType::Text: return "text";
        case ScreenMessageType::Photo: return "photo";
        case ScreenMessageType::Unknown:
        default: return "unknown";
    }
}

ScreenMessageType screen_message_type_from_string(const std::string& type) {
    if (type == "text") return ScreenMessageType::Text;
    if (type == "photo") return ScreenMessageType::Photo;
    return ScreenMessageType::Unknown;
}

ScreenMessageType screen_message_type_from_callback(const json& message) {
    if (message.contains("photo")) {
        return ScreenMessageType::Photo;
    }
    if (message.contains("text")) {
        return ScreenMessageType::Text;
    }
    return ScreenMessageType::Unknown;
}

bool is_persistable_screen_context(const std::string& context) {
    return context == "main" ||
           context == "topics" ||
           context == "ai" ||
           context == "stats" ||
           context == "generation" ||
           context == "dictionary" ||
           context == "learned" ||
           context == "daily" ||
           context == "evening";
}

void remember_screen_context(long long chat_id, const std::string& context) {
    if (!is_persistable_screen_context(context)) return;
    std::lock_guard<std::mutex> lock(g_screen_mutex);
    g_screen_context[chat_id] = context;
    save_bot_state_locked();
}

std::string get_screen_context(long long chat_id) {
    std::lock_guard<std::mutex> lock(g_screen_mutex);
    auto it = g_screen_context.find(chat_id);
    return it == g_screen_context.end() ? "" : it->second;
}

void remember_active_screen_message(long long chat_id, int message_id,
                                    ScreenMessageType type = ScreenMessageType::Unknown) {
    if (message_id <= 0) return;
    std::lock_guard<std::mutex> lock(g_screen_mutex);
    ScreenMessageState& state = g_active_screen_message[chat_id];
    state.message_id = message_id;
    if (type != ScreenMessageType::Unknown) {
        state.type = type;
    }
    save_bot_state_locked();
}

void clear_active_screen_message(long long chat_id) {
    std::lock_guard<std::mutex> lock(g_screen_mutex);
    g_active_screen_message.erase(chat_id);
    save_bot_state_locked();
}

void delete_active_screen_message(long long chat_id, TelegramClient& bot) {
    int message_id = 0;
    {
        std::lock_guard<std::mutex> lock(g_screen_mutex);
        auto it = g_active_screen_message.find(chat_id);
        if (it == g_active_screen_message.end()) {
            return;
        }
        message_id = it->second.message_id;
        g_active_screen_message.erase(it);
        save_bot_state_locked();
    }

    bot.delete_message(chat_id, message_id);
}

void delete_tracked_ai_input(long long chat_id, TelegramClient& bot, int except_message_id = 0) {
    int message_id = 0;
    {
        std::lock_guard<std::mutex> lock(g_screen_mutex);
        auto it = g_last_ai_user_message.find(chat_id);
        if (it == g_last_ai_user_message.end() || it->second == except_message_id) {
            return;
        }
        message_id = it->second;
        g_last_ai_user_message.erase(it);
    }

    bot.delete_message(chat_id, message_id);
}

void delete_tracked_broadcast_hint(long long chat_id, TelegramClient& bot) {
    int message_id = 0;
    {
        std::lock_guard<std::mutex> lock(g_screen_mutex);
        auto it = g_broadcast_hint_message.find(chat_id);
        if (it == g_broadcast_hint_message.end()) {
            return;
        }
        message_id = it->second;
        g_broadcast_hint_message.erase(it);
        save_bot_state_locked();
    }

    bot.delete_message(chat_id, message_id);
}

void remember_ai_input(long long chat_id, int message_id) {
    if (message_id <= 0) return;
    std::lock_guard<std::mutex> lock(g_screen_mutex);
    g_last_ai_user_message[chat_id] = message_id;
}

void remember_broadcast_hint(long long chat_id, int message_id) {
    if (message_id <= 0) return;
    std::lock_guard<std::mutex> lock(g_screen_mutex);
    g_broadcast_hint_message[chat_id] = message_id;
    save_bot_state_locked();
}

void delete_messages_after_delay(TelegramClient& bot, long long chat_id,
                                 std::vector<int> message_ids, int delay_seconds) {
    std::thread([&bot, chat_id, message_ids = std::move(message_ids), delay_seconds]() {
        std::this_thread::sleep_for(std::chrono::seconds(delay_seconds));
        for (int message_id : message_ids) {
            if (message_id > 0) {
                bot.delete_message(chat_id, message_id);
                std::lock_guard<std::mutex> lock(g_screen_mutex);
                auto it = g_broadcast_hint_message.find(chat_id);
                if (it != g_broadcast_hint_message.end() && it->second == message_id) {
                    g_broadcast_hint_message.erase(it);
                    save_bot_state_locked();
                }
            }
        }
    }).detach();
}

void delete_broadcast_hint_after_delay(TelegramClient& bot, long long chat_id,
                                       int message_id, int delay_seconds) {
    std::thread([&bot, chat_id, message_id, delay_seconds]() {
        std::this_thread::sleep_for(std::chrono::seconds(delay_seconds));

        bool should_delete = false;
        {
            std::lock_guard<std::mutex> lock(g_screen_mutex);
            auto it = g_broadcast_hint_message.find(chat_id);
            if (it != g_broadcast_hint_message.end() && it->second == message_id) {
                g_broadcast_hint_message.erase(it);
                save_bot_state_locked();
                should_delete = true;
            }
        }

        if (should_delete) {
            bot.delete_message(chat_id, message_id);
        }
    }).detach();
}

bool send_temporary_broadcast_hint(long long chat_id, TelegramClient& bot,
                                   const std::string& text) {
    delete_tracked_broadcast_hint(chat_id, bot);

    int message_id = 0;
    if (bot.send_message(chat_id, text, "Markdown", &message_id)) {
        remember_broadcast_hint(chat_id, message_id);
        delete_broadcast_hint_after_delay(bot, chat_id, message_id, 15 * 60);
        LOG("Temporary broadcast hint sent to " + std::to_string(chat_id) +
            ", message_id " + std::to_string(message_id));
        return true;
    } else {
        LOG_ERROR("Failed to send temporary broadcast hint to " + std::to_string(chat_id));
        return false;
    }
}

void ensure_reply_keyboard_removed(long long chat_id, TelegramClient& bot) {
    {
        std::lock_guard<std::mutex> lock(g_screen_mutex);
        if (g_reply_keyboard_removed[chat_id]) {
            return;
        }
        g_reply_keyboard_removed[chat_id] = true;
    }

    bot.remove_reply_keyboard(chat_id);
}

InlineKeyboard column_keyboard(std::initializer_list<std::pair<std::string, std::string>> buttons) {
    InlineKeyboard keyboard;
    for (const auto& button : buttons) {
        keyboard.push_back({button});
    }
    return keyboard;
}

InlineKeyboard main_menu_keyboard() {
    return column_keyboard({
        {"Словарь для изучения", "menu_dictionary"},
        {"Словарь для повторения", "menu_learned"},
        {"Добавить слова", "menu_new_words"},
        {"Спросить AI", "menu_ai"},
        {"Статистика", "menu_stats"}
    });
}

InlineKeyboard topic_keyboard() {
    return column_keyboard({
        {"Быт и дом", "topic_daily_life"},
        {"Путешествия", "topic_travel"},
        {"Еда", "topic_food"},
        {"Работа", "topic_business"},
        {"IT и C++", "topic_it_cpp"},
        {"Общение", "topic_communication"},
        {"Главное меню", "menu_main"}
    });
}

bool upsert_screen(long long chat_id, TelegramClient& bot, const std::string& text,
                   const InlineKeyboard& buttons, int preferred_message_id = 0) {
    int message_id = preferred_message_id > 0 ? preferred_message_id : get_active_screen_message(chat_id);
    if (message_id > 0) {
        ScreenMessageType current_type = get_active_screen_message_type(chat_id);
        if (current_type == ScreenMessageType::Text || current_type == ScreenMessageType::Unknown) {
            TelegramRequestResult edit_result;
            if (bot.edit_message(chat_id, message_id, text, buttons, &edit_result)) {
                remember_active_screen_message(chat_id, message_id, ScreenMessageType::Text);
                return true;
            }

            if (edit_result.failure_kind != TelegramFailureKind::Permanent) {
                LOG_WARNING("Keeping live dashboard message " + std::to_string(message_id) +
                            " after temporary editMessageText failure for chat " +
                            std::to_string(chat_id));
                return false;
            }
        } else {
            LOG("Replacing photo live dashboard message " + std::to_string(message_id) +
                " with text screen for chat " + std::to_string(chat_id));
        }

        LOG_WARNING("Resetting stale live dashboard message " + std::to_string(message_id) +
                    " for chat " + std::to_string(chat_id));
        bot.delete_message(chat_id, message_id);
        clear_active_screen_message(chat_id);
    }

    int sent_message_id = 0;
    if (bot.send_inline_keyboard(chat_id, text, buttons, &sent_message_id)) {
        remember_active_screen_message(chat_id, sent_message_id, ScreenMessageType::Text);
        return true;
    }

    return false;
}

bool upsert_photo_screen(long long chat_id, TelegramClient& bot, const std::string& photo_path,
                         const InlineKeyboard& buttons, int preferred_message_id = 0) {
    int message_id = preferred_message_id > 0 ? preferred_message_id : get_active_screen_message(chat_id);
    if (message_id > 0) {
        ScreenMessageType current_type = get_active_screen_message_type(chat_id);
        if (current_type == ScreenMessageType::Photo || current_type == ScreenMessageType::Unknown) {
            TelegramRequestResult edit_result;
            if (bot.edit_message_photo(chat_id, message_id, photo_path, buttons, "", &edit_result)) {
                remember_active_screen_message(chat_id, message_id, ScreenMessageType::Photo);
                return true;
            }

            if (edit_result.failure_kind != TelegramFailureKind::Permanent) {
                LOG_WARNING("Keeping live dashboard message " + std::to_string(message_id) +
                            " after temporary editMessageMedia failure for chat " +
                            std::to_string(chat_id));
                return false;
            }
        } else {
            LOG("Replacing text live dashboard message " + std::to_string(message_id) +
                " with photo screen for chat " + std::to_string(chat_id));
        }

        LOG_WARNING("Resetting stale live dashboard message " + std::to_string(message_id) +
                    " for chat " + std::to_string(chat_id));
        bot.delete_message(chat_id, message_id);
        clear_active_screen_message(chat_id);
    }

    int sent_message_id = 0;
    if (bot.send_photo(chat_id, photo_path, buttons, "", &sent_message_id)) {
        remember_active_screen_message(chat_id, sent_message_id, ScreenMessageType::Photo);
        return true;
    }

    return false;
}

InlineKeyboard page_keyboard(const std::string& callback_prefix, int page, int total,
                             const std::string& menu_callback = "menu_main") {
    InlineKeyboard buttons;
    std::vector<std::pair<std::string, std::string>> row;

    int prev_page = std::max(0, page - 1);
    int next_page = std::min(total - 1, page + 1);
    row.push_back({"<", callback_prefix + "prev_" + std::to_string(prev_page)});
    row.push_back({std::to_string(page + 1) + "/" + std::to_string(total),
                   callback_prefix + "info_" + std::to_string(page)});
    row.push_back({">", callback_prefix + "next_" + std::to_string(next_page)});
    buttons.push_back(row);

    buttons.push_back({{"Главное меню", menu_callback}});
    return buttons;
}

std::string html_escape(const std::string& text) {
    std::string result;
    result.reserve(text.size());
    for (char c : text) {
        switch (c) {
            case '&': result += "&amp;"; break;
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '"': result += "&quot;"; break;
            case '\'': result += "&#39;"; break;
            default: result += c; break;
        }
    }
    return result;
}

uint64_t fnv1a_update(uint64_t hash, const std::string& text) {
    constexpr uint64_t prime = 1099511628211ULL;
    for (unsigned char c : text) {
        hash ^= c;
        hash *= prime;
    }
    return hash;
}

std::string learned_page_hash(
    const std::vector<WordView>& words,
    int page,
    int total,
    int start,
    int end
) {
    uint64_t hash = 1469598103934665603ULL;
    hash = fnv1a_update(hash, std::to_string(page));
    hash = fnv1a_update(hash, std::to_string(total));
    hash = fnv1a_update(hash, std::to_string(words.size()));
    for (int i = start; i < end; i++) {
        const auto& [eng, trans, learned, pron, transcr, definition] = words[i];
        hash = fnv1a_update(hash, eng);
        hash = fnv1a_update(hash, "\n");
        hash = fnv1a_update(hash, trans);
        hash = fnv1a_update(hash, "\n");
        hash = fnv1a_update(hash, pron);
        hash = fnv1a_update(hash, "\n");
        hash = fnv1a_update(hash, transcr);
        hash = fnv1a_update(hash, "\n");
        hash = fnv1a_update(hash, definition);
        hash = fnv1a_update(hash, "\n");
    }

    std::stringstream ss;
    ss << std::hex << hash;
    return ss.str();
}

void remove_render_artifact_set(const fs::path& base_path) {
    for (const std::string& ext : {".png", ".html", ".hash"}) {
        std::error_code ec;
        fs::remove(base_path.string() + ext, ec);
    }
}

void cleanup_stale_page_artifacts(const fs::path& render_dir, int total_pages) {
    if (total_pages < 1 || !fs::exists(render_dir)) {
        return;
    }

    std::set<int> stale_pages;
    std::error_code ec;
    for (const auto& entry : fs::directory_iterator(render_dir, ec)) {
        if (ec || !entry.is_regular_file()) {
            continue;
        }

        std::string stem = entry.path().stem().string();
        const std::string prefix = "page_";
        if (stem.rfind(prefix, 0) != 0) {
            continue;
        }

        try {
            int page_number = std::stoi(stem.substr(prefix.size()));
            if (page_number > total_pages) {
                stale_pages.insert(page_number);
            }
        } catch (const std::exception&) {
        }
    }

    for (int page_number : stale_pages) {
        remove_render_artifact_set(render_dir / ("page_" + std::to_string(page_number)));
    }
}

void cleanup_legacy_stats_artifacts(const fs::path& stats_dir) {
    if (!fs::exists(stats_dir)) {
        return;
    }

    std::error_code ec;
    for (const auto& entry : fs::directory_iterator(stats_dir, ec)) {
        if (ec || !entry.is_regular_file()) {
            continue;
        }

        std::string stem = entry.path().stem().string();
        if (stem.rfind("stats_", 0) == 0) {
            std::error_code remove_ec;
            fs::remove(entry.path(), remove_ec);
        }
    }
}

bool cleanup_render_cache() {
    fs::path rendered_dir = fs::path(project_data_dir()) / "rendered";
    if (!fs::exists(rendered_dir)) {
        LOG("Render cache directory does not exist: " + rendered_dir.string());
        return true;
    }

    int removed = 0;
    std::error_code ec;
    for (const auto& entry : fs::recursive_directory_iterator(rendered_dir, ec)) {
        if (ec || !entry.is_regular_file()) {
            continue;
        }

        const fs::path path = entry.path();
        const std::string filename = path.filename().string();
        const std::string stem = path.stem().string();
        bool should_remove = filename.find(".tmp.") != std::string::npos;

        if (!should_remove && stem.rfind("stats_", 0) == 0) {
            should_remove = true;
        }

        if (!should_remove && path.parent_path().filename() == "menu" && stem == "main_menu") {
            should_remove = true;
        }

        if (should_remove) {
            std::error_code remove_ec;
            fs::remove(path, remove_ec);
            if (!remove_ec) {
                removed++;
            } else {
                LOG_WARNING("Failed to remove render cache artifact " + path.string() +
                            ": " + remove_ec.message());
            }
        }
    }

    LOG("Render cache cleanup complete, removed " + std::to_string(removed) + " files");
    return !ec;
}

fs::path bot_state_dir() {
    fs::path state_dir = fs::path(project_data_dir()) / "state";
    std::error_code ec;
    fs::create_directories(state_dir, ec);
    if (ec) {
        LOG_ERROR("Failed to create state directory: " + state_dir.string() + "; " + ec.message());
    }
    return state_dir;
}

int load_update_offset() {
    fs::path offset_path = bot_state_dir() / "telegram_update_offset.txt";
    std::string value = read_text_file(offset_path.string());
    if (value.empty()) {
        return 0;
    }

    try {
        return std::stoi(value);
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to parse update offset file: " + std::string(e.what()));
        return 0;
    }
}

void save_update_offset(int update_id) {
    fs::path offset_path = bot_state_dir() / "telegram_update_offset.txt";
    if (!write_text_file(offset_path.string(), std::to_string(update_id))) {
        LOG_ERROR("Failed to save update offset: " + offset_path.string());
    }
}

fs::path bot_live_state_path() {
    return bot_state_dir() / "bot_state.json";
}

fs::path broadcast_runs_state_path() {
    return bot_state_dir() / "broadcast_runs.json";
}

void save_bot_state_locked() {
    json state;
    state["chats"] = json::object();

    std::set<long long> chat_ids;
    for (const auto& [chat_id, _] : g_active_screen_message) chat_ids.insert(chat_id);
    for (const auto& [chat_id, _] : g_broadcast_hint_message) chat_ids.insert(chat_id);
    for (const auto& [chat_id, _] : g_chat_language) chat_ids.insert(chat_id);
    for (const auto& [chat_id, _] : g_screen_context) chat_ids.insert(chat_id);

    for (long long chat_id : chat_ids) {
        json chat_state;

        auto live_it = g_active_screen_message.find(chat_id);
        if (live_it != g_active_screen_message.end() && live_it->second.message_id > 0) {
            chat_state["live_dashboard_message_id"] = live_it->second.message_id;
            chat_state["live_dashboard_message_type"] = screen_message_type_to_string(live_it->second.type);
        }

        auto alert_it = g_broadcast_hint_message.find(chat_id);
        if (alert_it != g_broadcast_hint_message.end() && alert_it->second > 0) {
            chat_state["last_alert_text_message_id"] = alert_it->second;
        }

        auto language_it = g_chat_language.find(chat_id);
        if (language_it != g_chat_language.end() && !language_it->second.empty()) {
            chat_state["language"] = language_it->second;
        }

        auto context_it = g_screen_context.find(chat_id);
        if (context_it != g_screen_context.end() && !context_it->second.empty()) {
            chat_state["screen_context"] = context_it->second;
        }

        state["chats"][std::to_string(chat_id)] = chat_state;
    }

    fs::path state_path = bot_live_state_path();
    if (!write_text_file_atomic(state_path, state.dump(2))) {
        LOG_ERROR("Failed to save bot state: " + state_path.string());
    }
}

void save_bot_state() {
    std::lock_guard<std::mutex> lock(g_screen_mutex);
    save_bot_state_locked();
}

void load_bot_state() {
    fs::path state_path = bot_live_state_path();
    std::string state_text = read_text_file(state_path.string());
    if (state_text.empty()) {
        LOG("Bot state file not found or empty: " + state_path.string());
        return;
    }

    try {
        json state = json::parse(state_text);
        std::lock_guard<std::mutex> lock(g_screen_mutex);
        g_active_screen_message.clear();
        g_broadcast_hint_message.clear();
        g_chat_language.clear();
        g_screen_context.clear();

        if (!state.contains("chats") || !state["chats"].is_object()) {
            LOG_WARNING("Bot state has no chats object: " + state_path.string());
            return;
        }

        for (auto it = state["chats"].begin(); it != state["chats"].end(); ++it) {
            long long chat_id = 0;
            try {
                chat_id = std::stoll(it.key());
            } catch (const std::exception& e) {
                LOG_WARNING("Skipping invalid bot state chat id '" + it.key() + "': " + e.what());
                continue;
            }

            const json& chat_state = it.value();
            int live_message_id = chat_state.value("live_dashboard_message_id", 0);
            std::string live_message_type = chat_state.value("live_dashboard_message_type", "photo");
            int alert_message_id = chat_state.value("last_alert_text_message_id", 0);
            std::string language = chat_state.value("language", "");
            std::string screen_context = chat_state.value("screen_context", "");

            if (live_message_id > 0) {
                g_active_screen_message[chat_id] = {
                    live_message_id,
                    screen_message_type_from_string(live_message_type)
                };
            }
            if (alert_message_id > 0) {
                g_broadcast_hint_message[chat_id] = alert_message_id;
            }
            if (!language.empty()) {
                g_chat_language[chat_id] = language;
            }
            if (is_persistable_screen_context(screen_context)) {
                g_screen_context[chat_id] = screen_context;
            }
        }

        LOG("Loaded bot state for " + std::to_string(g_active_screen_message.size()) +
            " live dashboard chats from " + state_path.string());
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to load bot state " + state_path.string() + ": " + e.what());
    }
}

std::string local_date_key(const std::tm& tm) {
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d");
    return ss.str();
}

json load_broadcast_runs_state() {
    fs::path state_path = broadcast_runs_state_path();
    std::string state_text = read_text_file(state_path.string());
    if (state_text.empty()) {
        return json{{"runs", json::object()}};
    }

    try {
        json state = json::parse(state_text);
        if (!state.contains("runs") || !state["runs"].is_object()) {
            state["runs"] = json::object();
        }
        return state;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to parse broadcast runs state " + state_path.string() + ": " + e.what());
        return json{{"runs", json::object()}};
    }
}

bool broadcast_was_sent(const std::string& date_key, const std::string& kind, long long user_id) {
    json state = load_broadcast_runs_state();
    std::string user_key = std::to_string(user_id);
    return state["runs"].contains(date_key) &&
           state["runs"][date_key].contains(kind) &&
           state["runs"][date_key][kind].contains(user_key) &&
           state["runs"][date_key][kind][user_key].value("sent", false);
}

void remember_broadcast_sent(const std::string& date_key, const std::string& kind, long long user_id) {
    json state = load_broadcast_runs_state();
    std::string user_key = std::to_string(user_id);
    state["runs"][date_key][kind][user_key] = {
        {"sent", true},
        {"updated_at", std::time(nullptr)}
    };

    if (!write_text_file_atomic(broadcast_runs_state_path(), state.dump(2))) {
        LOG_ERROR("Failed to save broadcast runs state");
    }
}

std::string render_main_menu_image(const std::string& user_level) {
    fs::path render_dir = fs::path(project_data_dir()) / "rendered" / "menu";
    fs::path base = render_dir / ("main_menu_" + user_level);
    std::string menu_hash = "main-menu-graphite-v7-entropia5-" + user_level;
    std::stringstream html;

    html << R"(<!doctype html>
<html>
<head>
<meta charset="utf-8">
<style>
* { box-sizing: border-box; }
body {
  margin: 0;
  width: 1080px;
  height: 760px;
  background: #0b0d10;
  color: #f5f7f8;
  font-family: Arial, Helvetica, sans-serif;
}
.canvas {
  width: 1080px;
  height: 760px;
  padding: 62px;
  background:
    radial-gradient(circle at 82% 10%, rgba(52, 211, 153, .16), transparent 34%),
    radial-gradient(circle at 8% 90%, rgba(125, 148, 167, .12), transparent 36%),
    linear-gradient(145deg, #171b20 0%, #101317 62%, #090b0e 100%);
}
.panel {
  height: 100%;
  border-radius: 38px;
  border: 1px solid #303841;
  background: #191e24;
  box-shadow: 0 32px 90px rgba(0,0,0,.48);
  padding: 58px;
  position: relative;
}
.brand {
  color: #95a1ad;
  font-size: 30px;
  font-weight: 700;
  letter-spacing: 0;
}
.title-row {
  margin-top: 72px;
  max-width: 640px;
}
.title {
  font-size: 86px;
  line-height: 1.03;
  font-weight: 900;
  letter-spacing: 0;
  text-transform: uppercase;
}
.line {
  width: 220px;
  height: 8px;
  border-radius: 99px;
  margin-top: 18px;
  background: linear-gradient(90deg, #34d399, #d7dee6);
}
.subtitle {
  margin-top: 38px;
  max-width: 760px;
  color: #c9d1d8;
  font-size: 34px;
  line-height: 1.28;
}
.level-card {
  position: absolute;
  right: 58px;
  top: 58px;
  width: 292px;
  height: 292px;
  border-radius: 30px;
  background: #222830;
  border: 1px solid #3a434c;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  box-shadow: inset 0 1px 0 rgba(255,255,255,.04);
}
.level-label {
  color: #91a0ae;
  font-size: 30px;
  font-weight: 800;
}
.level-value {
  margin-top: 18px;
  font-size: 104px;
  line-height: 1;
  font-weight: 900;
  color: transparent;
  background: linear-gradient(135deg, #34d399 0%, #60a5fa 52%, #22d3ee 100%);
  -webkit-background-clip: text;
  background-clip: text;
  text-shadow:
    0 0 18px rgba(52, 211, 153, .34),
    0 0 38px rgba(96, 165, 250, .34);
}
.level-note {
  margin-top: 18px;
  color: #67e8f9;
  font-size: 28px;
  font-weight: 900;
  text-shadow: 0 0 18px rgba(103, 232, 249, .28);
}
</style>
</head>
<body>
<div class="canvas">
  <div class="panel">
    <div>
      <div class="brand">by entropia5</div>
      <div class="title-row">
        <div class="title">Главное<br>меню</div>
        <div class="line"></div>
      </div>
      <div class="subtitle">Выбери действие кнопками ниже.</div>
    </div>
    <div class="level-card">
      <div class="level-label">Текущий уровень</div>
      <div class="level-value">)";
    html << html_escape(user_level);
    html << R"(</div>
      <div class="level-note">English</div>
    </div>
  </div>
</div>
</body>
</html>
)";
    return render_html_image(base, menu_hash, html.str(), "main menu", 1080);
}

std::string render_ai_prompt_image() {
    fs::path base = fs::path(project_data_dir()) / "rendered" / "ai" / "prompt";
    std::string hash = "ai-prompt-graphite-v5-entropia5";
    std::stringstream html;

    html << R"(<!doctype html>
<html>
<head>
<meta charset="utf-8">
<style>
* { box-sizing: border-box; }
body {
  margin: 0;
  width: 1080px;
  height: 760px;
  background: #0b0d10;
  color: #f5f7f8;
  font-family: Arial, Helvetica, sans-serif;
}
.canvas {
  width: 1080px;
  height: 760px;
  padding: 38px;
  background:
    radial-gradient(circle at 82% 10%, rgba(72, 148, 255, .16), transparent 34%),
    radial-gradient(circle at 8% 90%, rgba(52, 211, 153, .13), transparent 35%),
    linear-gradient(145deg, #171b20 0%, #101317 62%, #090b0e 100%);
}
.panel {
  height: 100%;
  border-radius: 32px;
  border: 1px solid #303841;
  background: #191e24;
  box-shadow: 0 32px 90px rgba(0,0,0,.48);
  padding: 54px;
  display: flex;
  flex-direction: column;
  justify-content: space-between;
}
.label {
  color: #91a0ae;
  font-size: 30px;
  font-weight: 800;
}
.title-row {
  margin-top: 54px;
  max-width: 820px;
}
.title {
  font-size: 86px;
  line-height: 1.02;
  font-weight: 900;
  letter-spacing: 0;
  text-transform: uppercase;
}
.line {
  width: 230px;
  height: 8px;
  border-radius: 99px;
  margin-top: 18px;
  background: linear-gradient(90deg, #60a5fa, #34d399);
}
.text {
  margin-top: 38px;
  max-width: 820px;
  color: #d5dde4;
  font-size: 40px;
  line-height: 1.22;
  font-weight: 700;
}
.hint {
  padding: 24px 28px;
  border-radius: 22px;
  background: #242b33;
  border: 1px solid #3b444d;
  color: #c7d0d8;
  font-size: 30px;
  line-height: 1.25;
}
</style>
</head>
<body>
<div class="canvas">
  <div class="panel">
    <div>
      <div class="label">by entropia5</div>
      <div class="title-row">
        <div class="title">Режим<br>AI</div>
        <div class="line"></div>
      </div>
      <div class="text">Задай любой вопрос по английскому языку.</div>
    </div>
    <div class="hint">Я отвечу на активном экране бота. Для возврата используй кнопку ниже.</div>
  </div>
</div>
</body>
</html>
)";

    return render_html_image(base, hash, html.str(), "AI prompt");
}

std::string render_topic_menu_image() {
    fs::path base = fs::path(project_data_dir()) / "rendered" / "topics" / "topic_menu";
    std::string hash = "topic-menu-graphite-v5-entropia5";
    std::stringstream html;

    html << R"(<!doctype html>
<html>
<head>
<meta charset="utf-8">
<style>
* { box-sizing: border-box; }
body {
  margin: 0;
  width: 1080px;
  height: 820px;
  background: #0b0d10;
  color: #f5f7f8;
  font-family: Arial, Helvetica, sans-serif;
}
.canvas {
  width: 1080px;
  height: 820px;
  padding: 38px;
  background:
    radial-gradient(circle at 84% 12%, rgba(52, 211, 153, .16), transparent 34%),
    radial-gradient(circle at 0% 92%, rgba(96, 165, 250, .13), transparent 38%),
    linear-gradient(145deg, #171b20 0%, #101317 62%, #090b0e 100%);
}
.panel {
  height: 100%;
  border-radius: 34px;
  border: 1px solid #303841;
  background: #191e24;
  box-shadow: 0 32px 90px rgba(0,0,0,.48);
  padding: 54px;
  display: flex;
  flex-direction: column;
  justify-content: flex-start;
}
.label {
  color: #91a0ae;
  font-size: 30px;
  font-weight: 800;
}
.title-row {
  margin-top: 50px;
  max-width: 900px;
}
.title {
  font-size: 82px;
  line-height: 1.04;
  font-weight: 900;
  letter-spacing: 0;
  text-transform: uppercase;
}
.line {
  width: 236px;
  height: 8px;
  border-radius: 99px;
  margin-top: 18px;
  background: linear-gradient(90deg, #34d399, #60a5fa);
}
.text {
  margin-top: 40px;
  max-width: 780px;
  color: #d5dde4;
  font-size: 32px;
  line-height: 1.34;
  font-weight: 700;
}
</style>
</head>
<body>
<div class="canvas">
  <div class="panel">
    <div>
      <div class="label">by entropia5</div>
      <div class="title-row">
        <div class="title">Новые<br>слова</div>
        <div class="line"></div>
      </div>
      <div class="text">Выбери тему кнопками ниже. AI добавит 10 практичных слов с переводом, IPA-транскрипцией и русским произношением.</div>
    </div>
  </div>
</div>
</body>
</html>
)";

    return render_html_image(base, hash, html.str(), "topic menu");
}

std::string safe_render_key(std::string text) {
    text = to_lower_ascii(text);
    std::string result;
    for (char c : text) {
        if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) {
            result += c;
        } else if (c == '_' || c == '-' || c == ' ') {
            result += '_';
        }
    }
    if (result.empty()) {
        result = "status";
    }
    return result;
}

std::string render_status_image(const std::string& key, const std::string& title,
                                const std::string& subtitle, const std::string& note) {
    fs::path base = fs::path(project_data_dir()) / "rendered" / "status" / safe_render_key(key);
    std::string hash = "status-graphite-v2-entropia5-" + title + "|" + subtitle + "|" + note;
    std::stringstream html;

    html << R"(<!doctype html>
<html>
<head>
<meta charset="utf-8">
<style>
* { box-sizing: border-box; }
body {
  margin: 0;
  width: 1080px;
  height: 760px;
  background: #0b0d10;
  color: #f5f7f8;
  font-family: Arial, Helvetica, sans-serif;
}
.canvas {
  width: 1080px;
  height: 760px;
  padding: 38px;
  background:
    radial-gradient(circle at 82% 12%, rgba(52, 211, 153, .16), transparent 34%),
    radial-gradient(circle at 4% 92%, rgba(96, 165, 250, .13), transparent 36%),
    linear-gradient(145deg, #171b20 0%, #101317 62%, #090b0e 100%);
}
.panel {
  height: 100%;
  border-radius: 34px;
  border: 1px solid #303841;
  background: #191e24;
  box-shadow: 0 32px 90px rgba(0,0,0,.48);
  padding: 58px;
  display: flex;
  flex-direction: column;
  justify-content: space-between;
}
.label {
  color: #91a0ae;
  font-size: 30px;
  font-weight: 800;
}
.title {
  margin-top: 66px;
  font-size: 82px;
  line-height: 1.04;
  font-weight: 900;
  letter-spacing: 0;
  text-transform: uppercase;
}
.line {
  width: 230px;
  height: 8px;
  border-radius: 99px;
  margin-top: 20px;
  background: linear-gradient(90deg, #34d399, #60a5fa);
}
.subtitle {
  margin-top: 38px;
  color: #d5dde4;
  font-size: 38px;
  line-height: 1.24;
  font-weight: 800;
}
.note {
  padding: 26px 30px;
  border-radius: 24px;
  background: #242b33;
  border: 1px solid #3b444d;
  color: #c7d0d8;
  font-size: 30px;
  line-height: 1.28;
  font-weight: 700;
}
</style>
</head>
<body>
<div class="canvas">
  <div class="panel">
    <div>
      <div class="label">by entropia5</div>
      <div class="title">)";
    html << html_escape(title);
    html << R"(</div>
      <div class="line"></div>
      <div class="subtitle">)";
    html << html_escape(subtitle);
    html << R"(</div>
    </div>
    <div class="note">)";
    html << html_escape(note);
    html << R"(</div>
  </div>
</div>
</body>
</html>
)";

    return render_html_image(base, hash, html.str(), "status " + key, 1080);
}

bool show_status_screen(long long chat_id, TelegramClient& bot, const std::string& key,
                        const std::string& title, const std::string& subtitle,
                        const std::string& note, const InlineKeyboard& buttons,
                        int message_id = 0) {
    std::string image_path = render_status_image(key, title, subtitle, note);
    if (!image_path.empty()) {
        return upsert_photo_screen(chat_id, bot, image_path, buttons, message_id);
    }

    return upsert_screen(chat_id, bot, "*" + title + "*\n\n" + subtitle + "\n\n" + note, buttons, message_id);
}

std::string render_stats_image(long long chat_id, int total, int learned, const std::string& level,
                               const std::string& next_name, int next_level,
                               int percent) {
    fs::path stats_dir = fs::path(project_data_dir()) / "rendered" / "stats";
    cleanup_legacy_stats_artifacts(stats_dir);

    fs::path base = stats_dir / std::to_string(chat_id) / "stats";
    std::string hash = "stats-graphite-v4-entropia5-" + std::to_string(total) + "-" +
                       std::to_string(learned) + "-" + level + "-" +
                       std::to_string(next_level) + "-" + std::to_string(percent);
    int clamped_percent = std::max(0, std::min(100, percent));
    std::string next_text = next_level > 0
        ? "До " + next_name + " осталось " + std::to_string(next_level) + " слов"
        : "Уровень C1 достигнут";

    std::stringstream html;
    html << R"(<!doctype html>
<html>
<head>
<meta charset="utf-8">
<style>
* { box-sizing: border-box; }
body {
  margin: 0;
  width: 1080px;
  height: 860px;
  background: #0b0d10;
  color: #f5f7f8;
  font-family: Arial, Helvetica, sans-serif;
}
.canvas {
  width: 1080px;
  height: 860px;
  padding: 34px;
  background:
    radial-gradient(circle at 76% 8%, rgba(52, 211, 153, .16), transparent 34%),
    radial-gradient(circle at 0% 92%, rgba(96, 165, 250, .14), transparent 38%),
    linear-gradient(145deg, #171b20 0%, #101317 62%, #090b0e 100%);
}
.panel {
  height: 100%;
  border-radius: 34px;
  border: 1px solid #303841;
  background: #191e24;
  box-shadow: 0 32px 90px rgba(0,0,0,.48);
  padding: 48px;
}
.top { display: flex; justify-content: space-between; align-items: flex-start; gap: 26px; }
.label { color: #91a0ae; font-size: 30px; font-weight: 800; }
.title-row { margin-top: 18px; }
.title { font-size: 76px; line-height: 1.04; font-weight: 900; text-transform: uppercase; }
.line {
  width: 220px;
  height: 8px;
  border-radius: 99px;
  margin-top: 16px;
  background: linear-gradient(90deg, #34d399, #60a5fa);
}
.level {
  min-width: 150px;
  text-align: center;
  padding: 22px 26px;
  border-radius: 24px;
  background: #242b33;
  border: 1px solid #3b444d;
  font-size: 42px;
  font-weight: 900;
}
.grid { margin-top: 44px; display: grid; grid-template-columns: 1fr 1fr; gap: 18px; }
.metric {
  border-radius: 22px;
  background: #222830;
  border: 1px solid #303842;
  padding: 28px;
}
.metric .name { color: #9ba7b2; font-size: 28px; font-weight: 800; }
.metric .value { margin-top: 12px; font-size: 64px; line-height: 1; font-weight: 900; color: #fff; }
.progress-wrap { margin-top: 42px; }
.progress-head { display: flex; justify-content: space-between; align-items: center; font-size: 32px; font-weight: 900; }
.bar {
  margin-top: 18px;
  height: 58px;
  border-radius: 999px;
  background: #101419;
  border: 1px solid #303842;
  overflow: hidden;
}
.fill {
  height: 100%;
  border-radius: 999px;
  background: linear-gradient(90deg, #34d399, #60a5fa);
  box-shadow: 0 0 34px rgba(52, 211, 153, .28);
}
.next {
  margin-top: 34px;
  padding: 28px;
  border-radius: 24px;
  background: #242b33;
  border: 1px solid #3b444d;
  color: #d7e0e7;
  font-size: 36px;
  line-height: 1.22;
  font-weight: 900;
}
</style>
</head>
<body>
<div class="canvas">
<div class="panel">
  <div class="top">
    <div>
      <div class="label">by entropia5</div>
      <div class="title-row">
        <div class="title">Статистика</div>
        <div class="line"></div>
      </div>
    </div>
    <div class="level">)";
    html << html_escape(level);
    html << R"(</div>
  </div>
  <div class="grid">
    <div class="metric"><div class="name">Всего слов</div><div class="value">)";
    html << total;
    html << R"(</div></div>
    <div class="metric"><div class="name">Выучено</div><div class="value">)";
    html << learned;
    html << R"(</div></div>
  </div>
  <div class="progress-wrap">
    <div class="progress-head"><div>Прогресс словаря</div><div>)";
    html << clamped_percent;
    html << R"(%</div></div>
    <div class="bar"><div class="fill" style="width: )";
    html << clamped_percent;
    html << R"(%"></div></div>
  </div>
  <div class="next">)";
    html << html_escape(next_text);
    html << R"(</div>
</div>
</div>
</body>
</html>
)";

    return render_html_image(base, hash, html.str(), "stats");
}

std::string render_words_card_image(
    long long chat_id,
    const std::vector<WordView>& words,
    int page,
    int total,
    int start,
    int end,
    const std::string& folder,
    const std::string& title,
    const std::string& subtitle,
    const std::string& footer_left,
    bool checked_marker = true
) {
    fs::path render_dir = fs::path(project_data_dir()) / "rendered" / folder / std::to_string(chat_id);
    std::error_code ec;
    fs::create_directories(render_dir, ec);
    if (ec) {
        LOG_ERROR("Failed to create render directory: " + render_dir.string() + "; " + ec.message());
        return "";
    }
    cleanup_stale_page_artifacts(render_dir, total);

    fs::path page_base = render_dir / ("page_" + std::to_string(page + 1));
    std::string page_hash = folder + "-graphite-mobile-v10-entropia5-marker-" +
                            (checked_marker ? "checked-" : "study-") +
                            learned_page_hash(words, page, total, start, end);
    std::stringstream html;

    html << R"(<!doctype html>
<html>
<head>
<meta charset="utf-8">
<style>
* { box-sizing: border-box; }
body {
  margin: 0;
  width: 1440px;
  min-height: 1260px;
  background: #0d0f12;
  color: #f4f7f8;
  font-family: Arial, Helvetica, sans-serif;
}
.canvas {
  width: 1440px;
  min-height: 1260px;
  padding: 20px;
  background:
    radial-gradient(circle at 18% 0%, rgba(58, 171, 131, .18), transparent 38%),
    linear-gradient(145deg, #15191e 0%, #0f1216 58%, #0a0c0f 100%);
}
.panel {
  border: 1px solid #2f363d;
  border-radius: 26px;
  padding: 30px;
  background: #191e24;
  box-shadow: 0 28px 80px rgba(0,0,0,.42);
}
.head { display: flex; align-items: flex-start; justify-content: space-between; margin-bottom: 24px; gap: 24px; }
.title-row { display: block; }
.title { font-size: 76px; line-height: 1.02; font-weight: 900; letter-spacing: 0; }
.title-line {
  width: 220px;
  height: 8px;
  border-radius: 99px;
  margin-top: 14px;
  background: linear-gradient(90deg, #34d399, #60a5fa);
}
.subtitle { margin-top: 12px; color: #9ca8b3; font-size: 40px; line-height: 1.18; }
.badge {
  min-width: 190px;
  text-align: center;
  padding: 20px 24px;
  border-radius: 22px;
  background: #242b33;
  border: 1px solid #3b444d;
  color: #d8e1e8;
  font-size: 42px;
  font-weight: 800;
}
.list { display: grid; gap: 12px; }
.item {
  padding: 22px 26px;
  border-radius: 18px;
  background: #222830;
  border: 1px solid #303842;
}
.mark {
  flex: 0 0 auto;
  width: 46px;
  height: 46px;
  border-radius: 12px;
  background: linear-gradient(145deg, #34d399, #1f9e65);
  box-shadow: inset 0 -3px 0 rgba(0,0,0,.22);
  position: relative;
}
.mark:after {
  content: "";
  position: absolute;
  left: 13px;
  top: 8px;
  width: 14px;
  height: 26px;
  border: solid #062014;
  border-width: 0 5px 5px 0;
  transform: rotate(45deg);
}
.mark.study {
  background: #1a222b;
  border: 2px solid #60a5fa;
  box-shadow: inset 0 0 0 4px rgba(96, 165, 250, .12);
}
.mark.study:after { display: none; }
.word-line {
  display: flex;
  align-items: center;
  gap: 14px;
  flex-wrap: wrap;
}
.word { font-size: 58px; line-height: 1.08; font-weight: 900; color: #ffffff; }
.phonetics {
  display: inline-flex;
  align-items: center;
  gap: 12px;
  flex-wrap: wrap;
  color: #8ee6c8;
  font-size: 34px;
  line-height: 1.12;
  font-weight: 800;
}
.ipa {
  color: #77d8ff;
  text-shadow: 0 0 16px rgba(96, 165, 250, .22);
}
.pron {
  color: #8ee6c8;
  text-shadow: 0 0 16px rgba(52, 211, 153, .18);
}
.translation { margin-top: 8px; padding-left: 60px; font-size: 38px; line-height: 1.18; color: #cfd7de; }
.definition { margin-top: 8px; padding-left: 60px; font-size: 32px; line-height: 1.22; color: #9ca8b3; }
.footer {
  margin-top: 24px;
  display: flex;
  justify-content: space-between;
  align-items: center;
  color: #88939f;
  font-size: 32px;
}
</style>
</head>
<body>
<div class="canvas">
<div class="panel">
<div class="head">
<div>
<div class="title-row"><div class="title">)";
    html << html_escape(title);
    html << R"(</div><div class="title-line"></div></div>
<div class="subtitle">)";
    html << html_escape(subtitle);
    html << R"(</div>
</div>
<div class="badge">)";
    html << (page + 1) << " / " << total;
    html << R"(</div>
</div>
<div class="list">
)";

    for (int i = start; i < end; i++) {
        const auto& [eng, trans, learned, pron, transcr, definition] = words[i];
        std::string ipa = trim(transcr);
        if (!ipa.empty() && ipa.front() != '/' && ipa.front() != '[') {
            ipa = "/" + ipa + "/";
        }
        std::string pronunciation_ru = trim(pron);

        html << "<div class=\"item\">"
             << "<div class=\"word-line\"><span class=\"mark"
             << (checked_marker ? "" : " study")
             << "\"></span><span class=\"word\">" << html_escape(eng) << "</span>";
        if (!ipa.empty() || !pronunciation_ru.empty()) {
            html << "<span class=\"phonetics\">";
            if (!ipa.empty()) {
                html << "<span class=\"ipa\">" << html_escape(ipa) << "</span>";
            }
            if (!pronunciation_ru.empty()) {
                html << "<span class=\"pron\">" << html_escape(pronunciation_ru) << "</span>";
            }
            html << "</span>";
        }
        html << "</div>"
             << "<div class=\"translation\">" << html_escape(trans) << "</div>";
        if (!trim(definition).empty()) {
            html << "<div class=\"definition\">" << html_escape(trim(definition)) << "</div>";
        }
        html << "</div>\n";
    }

    html << R"(</div>
<div class="footer">
<div>)";
    html << html_escape(footer_left);
    html << R"(</div>
<div>)";
    html << "Всего: " << words.size();
    html << R"(</div>
</div>
</div>
</div>
</body>
</html>
)";
    return render_html_image(page_base, page_hash, html.str(), folder + " words", 1440);
}

std::string render_dictionary_words_image(
    long long chat_id,
    const std::vector<WordView>& words,
    int page,
    int total,
    int start,
    int end
) {
    return render_words_card_image(chat_id, words, page, total, start, end,
                                   "dictionary", "Словарь для изучения",
                                   "Напиши слово в чат, чтобы отметить его выученным",
                                   "by entropia5",
                                   false);
}

std::string render_learned_words_image(
    long long chat_id,
    const std::vector<WordView>& words,
    int page,
    int total,
    int start,
    int end
) {
    return render_words_card_image(chat_id, words, page, total, start, end,
                                   "learned", "Выученные слова",
                                   "Словарь для повторения и закрепления выученных слов",
                                   "by entropia5");
}

std::string render_daily_review_image(
    long long chat_id,
    const std::vector<WordView>& words,
    int page,
    int total,
    int start,
    int end
) {
    return render_words_card_image(chat_id, words, page, total, start, end,
                                   "daily", "Доброе утро",
                                   "Повторение выученных слов",
                                   "by entropia5");
}

std::string render_evening_words_image(
    long long chat_id,
    const std::vector<WordView>& words,
    int page,
    int total,
    int start,
    int end
) {
    return render_words_card_image(chat_id, words, page, total, start, end,
                                   "evening", "Новые слова",
                                   "Вечерняя подборка для изучения",
                                   "by entropia5");
}

// better formatting for words in the dictionary
std::string format_word(const std::string& english, const std::string& translation,
                        const std::string& transcription, const std::string& pronunciation_ru,
                        const std::string& definition) {
    std::string result;
    result += "🇺🇸 *" + english + "*\n";
    if (!transcription.empty()) result += "🏳️ " + transcription + "\n";
    if (!pronunciation_ru.empty()) result += "🏴 " + pronunciation_ru + "\n";
    result += "🇷🇺 " + translation;
    if (!trim(definition).empty()) {
        result += "\nСмысл: " + trim(definition);
    }
    return result;
}

// send or edit main menu with inline buttons
void send_main_menu(long long chat_id, TelegramClient& bot, Database& db, int message_id = 0) {
    remember_screen_context(chat_id, "main");
    int learned = db.get_words_count(chat_id, true);
    std::string user_level = english_level_from_learned(learned);
    std::string image_path = render_main_menu_image(user_level);
    if (!image_path.empty()) {
        upsert_photo_screen(chat_id, bot, image_path, main_menu_keyboard(), message_id);
        return;
    }

    upsert_screen(chat_id, bot, "*Главное меню*\n\nВыбери действие:", main_menu_keyboard(), message_id);
}

// send or edit topic selection menu with inline buttons
void send_topic_menu(long long chat_id, TelegramClient& bot, int message_id = 0) {
    remember_screen_context(chat_id, "topics");
    InlineKeyboard buttons = topic_keyboard();
    std::string image_path = render_topic_menu_image();
    if (!image_path.empty()) {
        upsert_photo_screen(chat_id, bot, image_path, buttons, message_id);
        return;
    }

    upsert_screen(chat_id, bot,
                  "*Выбери тему для новых слов:*\n\nAI подберет 10 новых слов.",
                  buttons, message_id);
}

void show_ai_prompt(long long chat_id, TelegramClient& bot, int message_id = 0) {
    remember_screen_context(chat_id, "ai");
    InlineKeyboard buttons = column_keyboard({
        {"Главное меню", "menu_main"}
    });
    std::string image_path = render_ai_prompt_image();
    if (!image_path.empty()) {
        upsert_photo_screen(chat_id, bot, image_path, buttons, message_id);
        return;
    }

    upsert_screen(chat_id, bot,
                  "*Режим AI*\n\nЗадай любой вопрос по английскому.\n\nДля возврата нажми кнопку ниже.",
                  buttons, message_id);
}

// show dictionary page with INLINE buttons for pagination
void show_dictionary_page(long long chat_id, TelegramClient& bot,
                          const std::vector<WordView>& words,
                          int page, int& current_page, std::string& last_action,
                          int& message_id, bool is_new = true) {
    int per_page = 5;
    int total = (words.size() + per_page - 1) / per_page;
    if (total == 0) total = 1;
    if (page < 0) page = 0;
    if (page >= total) page = total - 1;
    current_page = page;
    last_action = "dictionary";
    remember_screen_context(chat_id, "dictionary");

    int start = page * per_page;
    int end = std::min(start + per_page, (int)words.size());

    if (words.empty()) {
        std::string msg = "📚 *Словарь пуст*\n\nНажми «Добавить слова», чтобы пополнить словарь.";
        upsert_screen(chat_id, bot, msg, column_keyboard({
            {"Добавить слова", "menu_new_words"},
            {"Главное меню", "menu_main"}
        }), is_new ? 0 : message_id);
        return;
    }

    InlineKeyboard buttons = page_keyboard("dict_", page, total);
    std::string image_path = render_dictionary_words_image(chat_id, words, page, total, start, end);

    if (image_path.empty()) {
        std::string msg = "📚 *Словарь для изучения*\n";
        msg += "▫️ Страница " + std::to_string(page + 1) + " из " + std::to_string(total) + "\n";
        msg += "▫️ Чтобы отметить слово - просто напиши его\n\n";

        for (int i = start; i < end; i++) {
            const auto& [eng, trans, learned, pron, transcr, definition] = words[i];
            msg += format_word(eng, trans, transcr, pron, definition) + "\n\n";
        }
        upsert_screen(chat_id, bot, msg, buttons, is_new ? 0 : message_id);
    } else if (is_new) {
        upsert_photo_screen(chat_id, bot, image_path, buttons);
    } else {
        upsert_photo_screen(chat_id, bot, image_path, buttons, message_id);
    }
}

// show learned words page with INLINE buttons for pagination
void show_learned_page(long long chat_id, TelegramClient& bot,
                       const std::vector<WordView>& words,
                       int page, int& current_page, std::string& last_action,
                       int& message_id, bool is_new = true) {
    int per_page = 5;
    int total = (words.size() + per_page - 1) / per_page;
    if (total == 0) total = 1;
    if (page < 0) page = 0;
    if (page >= total) page = total - 1;
    current_page = page;
    last_action = "learned";
    remember_screen_context(chat_id, "learned");

    if (words.empty()) {
        std::string msg = "✅ *Выученных слов пока нет*\n\n📚 Учи слова из словаря!";
        upsert_screen(chat_id, bot, msg, column_keyboard({
            {"Словарь для изучения", "menu_dictionary"},
            {"Главное меню", "menu_main"}
        }), is_new ? 0 : message_id);
        return;
    }

    int start = page * per_page;
    int end = std::min(start + per_page, (int)words.size());
    std::string image_path = render_learned_words_image(chat_id, words, page, total, start, end);
    InlineKeyboard buttons = page_keyboard("learn_", page, total);

    if (image_path.empty()) {
        std::string msg = "*Выученные слова*\n\nНе удалось собрать картинку. Попробуй открыть раздел еще раз.";
        upsert_screen(chat_id, bot, msg, buttons, is_new ? 0 : message_id);
    } else if (is_new) {
        upsert_photo_screen(chat_id, bot, image_path, buttons);
    } else {
        upsert_photo_screen(chat_id, bot, image_path, buttons, message_id);
    }
}

// show daily review page with INLINE buttons for pagination
bool show_daily_review_page(long long chat_id, TelegramClient& bot,
                            const std::vector<WordView>& words,
                            int page, int message_id = 0, bool is_new = true, std::string* last_action = nullptr) {
    int per_page = 5;
    int total = (words.size() + per_page - 1) / per_page;
    if (total == 0) total = 1;
    if (page < 0) page = 0;
    if (page >= total) page = total - 1;
    if (last_action != nullptr) {
        *last_action = "daily";
    }
    remember_screen_context(chat_id, "daily");

    if (words.empty()) {
        return show_status_screen(
            chat_id,
            bot,
            "daily_empty",
            "Доброе утро",
            "Пока нет выученных слов для повторения.",
            "Отметь несколько слов как выученные, и завтра утренний экран покажет их для практики.",
            column_keyboard({
                {"Словарь для повторения", "menu_learned"},
                {"Главное меню", "menu_main"}
            }),
            is_new ? 0 : message_id
        );
    }

    int start = page * per_page;
    int end = std::min(start + per_page, (int)words.size());
    std::string image_path = render_daily_review_image(chat_id, words, page, total, start, end);
    InlineKeyboard buttons = page_keyboard("daily_", page, total);

    if (image_path.empty()) {
        std::string msg = "*Доброе утро*\n\nНе удалось собрать картинку повторения. Попробуй открыть раздел еще раз.";
        return upsert_screen(chat_id, bot, msg, buttons, is_new ? 0 : message_id);
    } else if (is_new) {
        return upsert_photo_screen(chat_id, bot, image_path, buttons);
    } else {
        return upsert_photo_screen(chat_id, bot, image_path, buttons, message_id);
    }
}

bool show_evening_words_page(long long chat_id, TelegramClient& bot,
                             const std::vector<WordView>& words,
                             int page, int message_id = 0, bool is_new = true, std::string* last_action = nullptr) {
    int per_page = 5;
    int total = (words.size() + per_page - 1) / per_page;
    if (total == 0) total = 1;
    if (page < 0) page = 0;
    if (page >= total) page = total - 1;
    if (last_action != nullptr) {
        *last_action = "evening";
    }
    remember_screen_context(chat_id, "evening");

    if (words.empty()) {
        return show_status_screen(
            chat_id,
            bot,
            "evening_empty",
            "Новые слова",
            "Сейчас нет слов для изучения.",
            "Выбери тему вручную, чтобы добавить новую подборку.",
            column_keyboard({
                {"Добавить слова", "menu_new_words"},
                {"Главное меню", "menu_main"}
            }),
            is_new ? 0 : message_id
        );
    }

    int start = page * per_page;
    int end = std::min(start + per_page, (int)words.size());
    std::string image_path = render_evening_words_image(chat_id, words, page, total, start, end);
    InlineKeyboard buttons = page_keyboard("evening_", page, total);

    if (image_path.empty()) {
        std::string msg = "*Новые слова*\n\nНе удалось собрать картинку. Попробуй открыть раздел еще раз.";
        return upsert_screen(chat_id, bot, msg, buttons, is_new ? 0 : message_id);
    } else if (is_new) {
        return upsert_photo_screen(chat_id, bot, image_path, buttons);
    } else {
        return upsert_photo_screen(chat_id, bot, image_path, buttons, message_id);
    }
}

// statistics page with progress bar and levels
void show_stats(long long chat_id, TelegramClient& bot, Database& db, int message_id = 0) {
    remember_screen_context(chat_id, "stats");
    int total = db.get_words_count(chat_id, false) + db.get_words_count(chat_id, true);
    int learned = db.get_words_count(chat_id, true);

    std::string level;
    int next_level;
    std::string next_name;

    if (learned < 300) { level = "A1"; next_level = 300 - learned; next_name = "A2"; }
    else if (learned < 600) { level = "A2"; next_level = 600 - learned; next_name = "B1"; }
    else if (learned < 1000) { level = "B1"; next_level = 1000 - learned; next_name = "B2"; }
    else if (learned < 1500) { level = "B2"; next_level = 1500 - learned; next_name = "C1"; }
    else { level = "C1"; next_level = 0; next_name = ""; }

    int percent = (total > 0) ? (learned * 100 / total) : 0;

    InlineKeyboard buttons = column_keyboard({
        {"Главное меню", "menu_main"}
    });
    std::string image_path = render_stats_image(chat_id, total, learned, level, next_name, next_level, percent);
    if (!image_path.empty()) {
        upsert_photo_screen(chat_id, bot, image_path, buttons, message_id);
        return;
    }

    std::string msg = "*Статистика*\n\n";
    msg += "Уровень: " + level + "\n";
    msg += "Всего слов: " + std::to_string(total) + "\n";
    msg += "Выучено: " + std::to_string(learned) + "\n";
    msg += "Прогресс: " + std::to_string(percent) + "%\n\n";
    msg += next_level > 0
        ? "До " + next_name + " осталось " + std::to_string(next_level) + " слов"
        : "Ты достиг уровня C1!";
    upsert_screen(chat_id, bot, msg, buttons, message_id);
}

bool try_mark_words_from_input(long long chat_id, const std::string& text, Database& db) {
    auto requested_words = split_words_input(text);
    if (requested_words.empty()) return false;

    std::vector<std::string> marked_words;
    std::vector<std::string> not_found_words;

    for (const auto& word : requested_words) {
        if (db.word_exists(chat_id, word)) {
            if (db.mark_word_learned(chat_id, word)) {
                marked_words.push_back(word);
            }
        } else if (requested_words.size() > 1) {
            not_found_words.push_back(word);
        }
    }

    if (marked_words.empty()) return false;

    return true;
}

std::vector<WordView>
get_learned_words_for_review(long long chat_id, Database& db);

void refresh_after_marking_words(long long chat_id, TelegramClient& bot, Database& db,
                                 int& dict_current_page, int& dict_message_id,
                                 int& learn_current_page, int& learn_message_id,
                                 std::string& current_action) {
    std::string persisted_context = get_screen_context(chat_id);
    std::string context = is_persistable_screen_context(persisted_context)
        ? persisted_context
        : current_action;
    current_action = context;

    if (context == "daily") {
        auto words = get_learned_words_for_review(chat_id, db);
        show_daily_review_page(chat_id, bot, words, 0, get_active_screen_message(chat_id), false, &current_action);
        return;
    }

    if (context == "evening") {
        auto words = db.get_user_words_full(chat_id, true);
        show_evening_words_page(chat_id, bot, words, 0, get_active_screen_message(chat_id), false, &current_action);
        return;
    }

    if (context == "learned") {
        auto all_words = db.get_user_words_full(chat_id, false);
        std::vector<WordView> learned;
        for (const auto& word : all_words) {
            if (std::get<2>(word)) {
                learned.push_back(word);
            }
        }
        learn_message_id = get_active_screen_message(chat_id);
        show_learned_page(chat_id, bot, learned, learn_current_page, learn_current_page,
                          current_action, learn_message_id, false);
        return;
    }

    auto words = db.get_user_words_full(chat_id, true);
    int page = context == "dictionary" ? dict_current_page : 0;
    dict_message_id = get_active_screen_message(chat_id);
    show_dictionary_page(chat_id, bot, words, page, dict_current_page, current_action, dict_message_id, false);
}

bool backfill_missing_pronunciations(Database& db, GroqClient& ai) {
    if (!ai.is_available()) {
        LOG_ERROR("Cannot backfill transcriptions: GROQ_API_KEY is not configured");
        return false;
    }

    int total_updated = 0;
    constexpr int batch_size = 15;
    constexpr int max_batches = 100;

    for (int batch = 1; batch <= max_batches; batch++) {
        auto missing_words = db.get_words_missing_pronunciation(batch_size);
        if (missing_words.empty()) {
            LOG("Transcription backfill complete, updated " + std::to_string(total_updated) + " words");
            return true;
        }

        std::stringstream prompt;
        prompt << "Fill missing pronunciation data for these English words. "
               << "Return ONLY blocks in this exact format, keep IDs exactly, no numbering and no extra text:\n"
               << "ID: 123\n"
               << "TRANS: /IPA transcription/\n"
               << "PRON: русское произношение кириллицей\n"
               << "---\n\n"
               << "Words:\n";

        for (const auto& [id, english, translation] : missing_words) {
            prompt << "ID: " << id << "\n"
                   << "WORD: " << english << "\n"
                   << "MEAN: " << translation << "\n"
                   << "---\n";
        }

        std::string response = ai.ask(prompt.str(),
            "You return machine-readable English pronunciation data only. "
            "TRANS is IPA with slashes. PRON is Russian Cyrillic approximate pronunciation.");
        auto updates = parse_pronunciation_updates(response);
        LOG("Backfill batch " + std::to_string(batch) + ": parsed " +
            std::to_string(updates.size()) + " pronunciation updates");

        int batch_updated = 0;
        for (const auto& update : updates) {
            if (db.update_word_pronunciation(update.id, update.transcription, update.pronunciation)) {
                batch_updated++;
            }
        }

        total_updated += batch_updated;
        if (batch_updated == 0) {
            LOG_ERROR("Transcription backfill stopped: AI returned no usable updates");
            return false;
        }
    }

    LOG_WARNING("Transcription backfill reached batch limit, updated " + std::to_string(total_updated) + " words");
    return true;
}

bool backfill_missing_definitions(Database& db, GroqClient& ai) {
    if (!ai.is_available()) {
        LOG_ERROR("Cannot backfill definitions: GROQ_API_KEY is not configured");
        return false;
    }

    int total_updated = 0;
    constexpr int batch_size = 20;
    constexpr int max_batches = 100;
    int empty_update_retries = 0;

    for (int batch = 1; batch <= max_batches; batch++) {
        auto missing_words = db.get_words_missing_definition(batch_size);
        if (missing_words.empty()) {
            LOG("Definition backfill complete, updated " + std::to_string(total_updated) + " words");
            return true;
        }

        std::stringstream prompt;
        prompt << "Write short Russian meaning explanations for these English words. "
               << "Each DEF must explain the sense in simple Russian, not just repeat the translation. "
               << "Keep DEF concise: 1 short sentence, max 90 characters. "
               << "Return ONLY blocks in this exact format, keep IDs exactly, no numbering and no extra text:\n"
               << "ID: 123\n"
               << "DEF: краткое объяснение смысла слова\n"
               << "---\n\n"
               << "Words:\n";

        for (const auto& [id, english, translation] : missing_words) {
            prompt << "ID: " << id << "\n"
                   << "WORD: " << english << "\n"
                   << "TRANSLATION: " << translation << "\n"
                   << "---\n";
        }

        std::string response = ai.ask(prompt.str(),
            "You return machine-readable Russian word definitions only. "
            "Each DEF is a concise Russian explanation of meaning, not an example sentence.");
        auto updates = parse_definition_updates(response);
        LOG("Definition backfill batch " + std::to_string(batch) + ": parsed " +
            std::to_string(updates.size()) + " updates");

        int batch_updated = 0;
        for (const auto& update : updates) {
            if (db.update_word_definition(update.id, trim(update.definition))) {
                batch_updated++;
            }
        }

        total_updated += batch_updated;
        if (batch_updated == 0) {
            if (empty_update_retries < 3) {
                empty_update_retries++;
                LOG_WARNING("Definition backfill batch returned no usable updates; retry " +
                            std::to_string(empty_update_retries) + "/3 after delay");
                std::this_thread::sleep_for(std::chrono::seconds(5));
                batch--;
                continue;
            }
            LOG_ERROR("Definition backfill stopped: AI returned no usable updates");
            return false;
        }
        empty_update_retries = 0;
    }

    LOG_WARNING("Definition backfill reached batch limit, updated " + std::to_string(total_updated) + " words");
    return true;
}

struct WordGenerationResult {
    int added = 0;
    int duplicates = 0;
    int duplicate_in_response = 0;
    int rejected_quality = 0;
    int fallback_added = 0;
    int parsed_total = 0;
    int failed = 0;
};

std::string generation_attempt_hint(int attempt) {
    if (attempt == 1) {
        return "Use practical everyday verbs and concrete nouns that fit A2/B1 learners.";
    }
    if (attempt == 2) {
        return "Use a different lexical angle: errands, services, planning, tools, documents, places, and real-life actions.";
    }
    if (attempt == 3) {
        return "Use home, work, travel, food, communication, and city-life vocabulary that is useful but not too basic.";
    }
    if (attempt == 4) {
        return "Use common collocation-friendly nouns and verbs from everyday adult life.";
    }
    if (attempt == 5) {
        return "Use B1/B2 practical vocabulary from services, paperwork, repairs, schedules, and decisions.";
    }
    return "Use another fresh semantic field. Avoid generic words, rare jargon, brands, apps, and any previous candidate.";
}

bool is_ascii_english_word(const std::string& word) {
    if (word.size() < 3 || word.size() > 18) {
        return false;
    }
    return std::all_of(word.begin(), word.end(), [](unsigned char c) {
        return c >= 'a' && c <= 'z';
    });
}

const std::set<std::string>& blocked_generated_words() {
    static const std::set<std::string> blocked_words = {
        "swype", "microwaver", "googling", "instagram", "whatsapp", "facebook",
        "youtube", "tiktok", "uber", "airbnb", "outpatient",
        "html", "css", "api", "sql", "url", "usb",
        "hacker", "virus", "malware", "robot", "mouse"
    };

    return blocked_words;
}

bool is_suspicious_generated_word(const std::string& normalized_word) {
    static const std::vector<std::string> blocked_suffixes = {
        "warez"
    };

    if (!is_ascii_english_word(normalized_word)) {
        return true;
    }
    if (blocked_generated_words().count(normalized_word) > 0) {
        return true;
    }
    for (const std::string& suffix : blocked_suffixes) {
        if (normalized_word.size() > suffix.size() &&
            normalized_word.compare(normalized_word.size() - suffix.size(), suffix.size(), suffix) == 0) {
            return true;
        }
    }

    return false;
}

std::vector<GeneratedWord> built_in_fallback_practical_words() {
    return {
        {"receipt", "/rɪˈsiːt/", "рисит", "чек"},
        {"refund", "/ˈriːfʌnd/", "рифанд", "возврат денег"},
        {"warranty", "/ˈwɒrənti/", "уорэнти", "гарантия"},
        {"invoice", "/ˈɪnvɔɪs/", "инвойс", "счет"},
        {"estimate", "/ˈestɪmət/", "эстимэт", "оценка стоимости"},
        {"deadline", "/ˈdedlaɪn/", "дедлайн", "крайний срок"},
        {"appointment", "/əˈpɔɪntmənt/", "эпойнтмэнт", "встреча, запись"},
        {"schedule", "/ˈʃedjuːl/", "шеджул", "расписание"},
        {"reminder", "/rɪˈmaɪndər/", "римайндер", "напоминание"},
        {"checklist", "/ˈtʃeklɪst/", "чеклист", "контрольный список"},
        {"paperwork", "/ˈpeɪpərwɜːrk/", "пэйпэруорк", "документы"},
        {"timesheet", "/ˈtaɪmʃiːt/", "таймшит", "табель учета времени"},
        {"courier", "/ˈkʊriər/", "куриэр", "курьер"},
        {"parcel", "/ˈpɑːrsl/", "парсл", "посылка"},
        {"delivery", "/dɪˈlɪvəri/", "диливэри", "доставка"},
        {"package", "/ˈpækɪdʒ/", "пэкидж", "упаковка, посылка"},
        {"storage", "/ˈstɔːrɪdʒ/", "сторидж", "хранение"},
        {"shelf", "/ʃelf/", "шелф", "полка"},
        {"drawer", "/drɔːr/", "дроэр", "ящик"},
        {"outlet", "/ˈaʊtlet/", "аутлет", "розетка"},
        {"adapter", "/əˈdæptər/", "эдэптэр", "адаптер"},
        {"blanket", "/ˈblæŋkɪt/", "блэнкит", "одеяло"},
        {"kettle", "/ˈketl/", "кетл", "чайник"},
        {"faucet", "/ˈfɔːsɪt/", "фосит", "кран"},
        {"laundry", "/ˈlɔːndri/", "лондри", "стирка"},
        {"detergent", "/dɪˈtɜːrdʒənt/", "дитёрджэнт", "моющее средство"},
        {"commute", "/kəˈmjuːt/", "комьют", "поездка на работу"},
        {"shortcut", "/ˈʃɔːrtkʌt/", "шорткат", "короткий путь"},
        {"suburb", "/ˈsʌbɜːrb/", "сабёрб", "пригород"},
        {"traffic", "/ˈtræfɪk/", "трэфик", "дорожное движение"},
        {"luggage", "/ˈlʌɡɪdʒ/", "лагидж", "багаж"},
        {"boarding", "/ˈbɔːrdɪŋ/", "бординг", "посадка"},
        {"arrival", "/əˈraɪvəl/", "эрайвэл", "прибытие"},
        {"departure", "/dɪˈpɑːrtʃər/", "дипарчер", "отправление"},
        {"platform", "/ˈplætfɔːrm/", "плэтформ", "платформа"},
        {"aisle", "/aɪl/", "айл", "проход"},
        {"receipt", "/rɪˈsiːt/", "рисит", "чек"},
        {"ingredient", "/ɪnˈɡriːdiənt/", "ингридиэнт", "ингредиент"},
        {"leftover", "/ˈleftoʊvər/", "лэфтоувэр", "остаток еды"},
        {"portion", "/ˈpɔːrʃn/", "поршн", "порция"},
        {"utensil", "/juːˈtensl/", "ютенсл", "столовый прибор"},
        {"grocery", "/ˈɡroʊsəri/", "гроусэри", "продукты"},
        {"pantry", "/ˈpæntri/", "пэнтри", "кладовая"},
        {"receipt", "/rɪˈsiːt/", "рисит", "чек"},
        {"summary", "/ˈsʌməri/", "самэри", "краткое описание"},
        {"request", "/rɪˈkwest/", "риквест", "запрос"},
        {"approval", "/əˈpruːvəl/", "эпрувэл", "одобрение"},
        {"feedback", "/ˈfiːdbæk/", "фидбэк", "обратная связь"},
        {"meeting", "/ˈmiːtɪŋ/", "митинг", "встреча"},
        {"agenda", "/əˈdʒendə/", "эдженда", "повестка"},
        {"priority", "/praɪˈɔːrəti/", "прайорити", "приоритет"},
        {"progress", "/ˈprɑːɡres/", "прогрэс", "прогресс"},
        {"issue", "/ˈɪʃuː/", "ишью", "проблема"},
        {"solution", "/səˈluːʃn/", "солюшн", "решение"},
        {"backup", "/ˈbækʌp/", "бэкап", "резервная копия"},
        {"folder", "/ˈfoʊldər/", "фоулдэр", "папка"},
        {"attachment", "/əˈtætʃmənt/", "этэчмэнт", "вложение"},
        {"browser", "/ˈbraʊzər/", "браузэр", "браузер"},
        {"password", "/ˈpæswɜːrd/", "пэсуорд", "пароль"},
        {"privacy", "/ˈpraɪvəsi/", "прайвэси", "конфиденциальность"}
    };
}

std::string json_string_value(const json& item,
                              const std::string& primary_key,
                              const std::string& fallback_key = "") {
    if (item.contains(primary_key) && item[primary_key].is_string()) {
        return trim(item[primary_key].get<std::string>());
    }
    if (!fallback_key.empty() && item.contains(fallback_key) && item[fallback_key].is_string()) {
        return trim(item[fallback_key].get<std::string>());
    }
    return "";
}

std::vector<GeneratedWord> load_fallback_words_from_file(const fs::path& path) {
    std::string text = read_text_file(path.string());
    if (text.empty()) {
        return {};
    }

    try {
        json document = json::parse(text);
        const json* words_json = &document;
        if (document.is_object() && document.contains("words")) {
            words_json = &document["words"];
        }
        if (!words_json->is_array()) {
            LOG_WARNING("Fallback dictionary is not a JSON array: " + path.string());
            return {};
        }

        std::vector<GeneratedWord> words;
        std::set<std::string> seen;
        for (size_t i = 0; i < words_json->size(); i++) {
            const json& item = (*words_json)[i];
            if (!item.is_object()) {
                LOG_WARNING("Skipping fallback dictionary item " + std::to_string(i) +
                            ": expected object");
                continue;
            }

            GeneratedWord word;
            word.english = to_lower_ascii(json_string_value(item, "english", "word"));
            word.transcription = json_string_value(item, "transcription", "trans");
            word.pronunciation = json_string_value(item, "pronunciation_ru", "pronunciation");
            word.translation = json_string_value(item, "translation_ru", "translation");
            word.definition = json_string_value(item, "definition_ru", "definition");

            if (word.english.empty() || word.transcription.empty() || word.pronunciation.empty() ||
                word.translation.empty() || word.definition.empty() ||
                is_suspicious_generated_word(word.english)) {
                LOG_WARNING("Skipping invalid fallback dictionary word at index " + std::to_string(i) +
                            " in " + path.string());
                continue;
            }
            if (seen.count(word.english) > 0) {
                LOG_WARNING("Skipping duplicate fallback dictionary word: " + word.english);
                continue;
            }

            seen.insert(word.english);
            words.push_back(word);
        }

        LOG("Loaded fallback dictionary from " + path.string() +
            ": " + std::to_string(words.size()) + " words");
        return words;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to parse fallback dictionary " + path.string() + ": " + e.what());
        return {};
    }
}

std::vector<GeneratedWord> fallback_practical_words() {
    std::vector<fs::path> paths = {
        fs::path(project_data_dir()) / "fallback_words.json",
        fs::path("resources") / "fallback_words.json",
        fs::path("..") / "resources" / "fallback_words.json"
    };

    for (const fs::path& path : paths) {
        if (!fs::exists(path)) {
            continue;
        }
        auto words = load_fallback_words_from_file(path);
        if (!words.empty()) {
            return words;
        }
    }

    LOG_WARNING("Fallback dictionary file is unavailable, using built-in fallback words");
    return built_in_fallback_practical_words();
}

WordGenerationResult add_generated_words_to_db(long long chat_id, Database& db, GroqClient& ai,
                                               const std::string& topic_keyword, int target_count = 10) {
    WordGenerationResult result;
    std::set<std::string> seen_generated_words;
    constexpr int max_attempts = 8;

    for (int attempt = 1; attempt <= max_attempts && result.added < target_count; attempt++) {
        auto existing_words = db.get_user_words_full(chat_id, false);
        int need = target_count - result.added;
        int requested_candidates = std::min(30, std::max(need * 4, 16));

        std::string prompt = "Generate " + std::to_string(requested_candidates) +
            " candidate English words. I will accept only " + std::to_string(need) +
            " of them. Topic: " + topic_keyword +
            ". Use practical dictionary headwords for language learners. Do not repeat words from the avoid list. "
            "Do not repeat words inside your own answer. "
            "Return only real common English dictionary words, lowercase, 3-18 letters, one word only. "
            "Do not invent words. Do not use acronyms, brand names, app names, typos, slang spellings, medical terms, "
            "security scare words, hardware objects, or obscure jargon. " +
            generation_attempt_hint(attempt) + " "
            "For every word, TRANS must be the original English IPA transcription with slashes, "
            "PRON must be a Russian Cyrillic pronunciation hint, "
            "and DEF must be a short Russian explanation of the word meaning, not just a synonym." +
            build_existing_words_hint(existing_words) +
            build_seen_generated_words_hint(seen_generated_words) +
            "\nReturn ONLY blocks in this exact format, no numbering and no extra text:\n"
            "WORD: word\n"
            "TRANS: /IPA transcription/\n"
            "PRON: русское произношение кириллицей\n"
            "MEAN: russian translation\n"
            "DEF: краткое объяснение смысла на русском\n"
            "---\n";

        std::string response = ai.ask(prompt);
        LOG("AI Response length: " + std::to_string(response.length()) +
            ", attempt: " + std::to_string(attempt));

        auto generated_words = parse_generated_words(response);
        result.parsed_total += generated_words.size();
        LOG("Parsed generated words: " + std::to_string(generated_words.size()));

        if (generated_words.empty()) {
            result.failed++;
            continue;
        }

        for (const auto& word : generated_words) {
            if (result.added >= target_count) break;

            std::string normalized_word = to_lower_ascii(trim(word.english));
            if (normalized_word.empty()) {
                result.failed++;
                continue;
            }
            std::string translation = trim(word.translation);
            std::string pronunciation = trim(word.pronunciation);
            std::string transcription = trim(word.transcription);
            std::string definition = trim(word.definition);
            if (translation.empty() || pronunciation.empty() || transcription.empty() || definition.empty()) {
                result.rejected_quality++;
                LOG("Generated incomplete word rejected: " + word.english);
                continue;
            }
            if (is_suspicious_generated_word(normalized_word)) {
                result.rejected_quality++;
                LOG("Generated low-quality word rejected: " + word.english);
                continue;
            }
            if (seen_generated_words.count(normalized_word) > 0) {
                result.duplicate_in_response++;
                LOG("Generated duplicate inside response skipped: " + word.english);
                continue;
            }
            seen_generated_words.insert(normalized_word);

            if (db.word_exists(chat_id, normalized_word)) {
                result.duplicates++;
                LOG("Generated duplicate skipped before insert: " + word.english);
                continue;
            }

            LOG("Adding word: " + normalized_word + " -> " + translation);
            if (db.add_word(chat_id, normalized_word, translation, pronunciation,
                            transcription, topic_keyword, definition)) {
                result.added++;
            } else {
                result.failed++;
            }
        }
    }

    if (result.added < target_count) {
        for (const GeneratedWord& word : fallback_practical_words()) {
            if (result.added >= target_count) break;

            std::string normalized_word = to_lower_ascii(trim(word.english));
            std::string translation = trim(word.translation);
            std::string pronunciation = trim(word.pronunciation);
            std::string transcription = trim(word.transcription);
            std::string definition = trim(word.definition);
            if (definition.empty() && !translation.empty()) {
                definition = "Означает: " + translation + ".";
            }
            if (normalized_word.empty() || seen_generated_words.count(normalized_word) > 0 ||
                translation.empty() || pronunciation.empty() || transcription.empty() || definition.empty() ||
                is_suspicious_generated_word(normalized_word)) {
                continue;
            }
            seen_generated_words.insert(normalized_word);

            if (db.word_exists(chat_id, normalized_word)) {
                result.duplicates++;
                continue;
            }

            LOG("Adding fallback word: " + normalized_word + " -> " + translation);
            if (db.add_word(chat_id, normalized_word, translation, pronunciation,
                            transcription, topic_keyword, definition)) {
                result.added++;
                result.fallback_added++;
            } else {
                result.failed++;
            }
        }
    }

    return result;
}

// generate words using AI and add to database
void generate_words(long long chat_id, TelegramClient& bot, Database& db, GroqClient& ai,
                    const std::string& topic_keyword, const std::string& topic_name,
                    int message_id = 0) {
    remember_screen_context(chat_id, "generation");
    show_status_screen(
        chat_id,
        bot,
        "generate_" + topic_keyword,
        "Генерирую слова",
        "Тема: " + topic_name,
        "AI подбирает реальные словарные слова, проверяет дубли и готовит произношение.",
        column_keyboard({{"Главное меню", "menu_main"}}),
        message_id
    );

    WordGenerationResult generation = add_generated_words_to_db(chat_id, db, ai, topic_keyword, 10);

    if (generation.added > 0) {
        std::string note = "Добавлено: " + std::to_string(generation.added) + ".";
        if (generation.fallback_added > 0) {
            note += " Добрано из резервного словаря: " + std::to_string(generation.fallback_added) + ".";
        }
        if (generation.added < 10) {
            note += " Не хватило уникальных слов даже после дополнительных попыток и резервного словаря.";
            if (generation.duplicates > 0) {
                note += " Дубли: " + std::to_string(generation.duplicates) + ".";
            }
            if (generation.rejected_quality > 0) {
                note += " Отсеяно: " + std::to_string(generation.rejected_quality) + ".";
            }
        }
        show_status_screen(chat_id, bot, "generate_done_" + topic_keyword,
                           "Слова добавлены", "Новая подборка готова.", note,
                           column_keyboard({
            {"Словарь для изучения", "menu_dictionary"},
            {"Добавить слова", "menu_new_words"},
            {"Главное меню", "menu_main"}
        }));
    } else {
        std::string note = "Попробуй другую тему.";
        if (generation.duplicates > 0) {
            note = "AI предложил только слова, которые уже есть в словаре. " + note;
        } else if (generation.parsed_total == 0) {
            note = "AI вернул ответ в неожиданном формате. " + note;
        } else if (generation.rejected_quality > 0) {
            note = "Все новые варианты не прошли фильтр качества. " + note;
        }
        show_status_screen(chat_id, bot, "generate_failed_" + topic_keyword,
                           "Не удалось добавить", "Новых слов сейчас нет.", note,
                           column_keyboard({
            {"Добавить слова", "menu_new_words"},
            {"Главное меню", "menu_main"}
        }));
    }
}

long long configured_user_id(const std::string& key);

std::vector<WordView>
get_learned_words_for_review(long long chat_id, Database& db) {
    auto all_words = db.get_user_words_full(chat_id, false);
    std::vector<WordView> learned;
    for (const auto& word : all_words) {
        if (std::get<2>(word)) {
            learned.push_back(word);
        }
    }
    return learned;
}

bool refresh_doc_screenshots(Database& db, long long chat_id) {
    if (chat_id <= 0) {
        LOG_ERROR("--refresh-doc-screenshots requires a chat id or USER_1_ID in .env");
        return false;
    }

    fs::path docs_dir = "docs/ui";
    std::error_code ec;
    fs::create_directories(docs_dir, ec);
    if (ec) {
        LOG_ERROR("Failed to create docs/ui directory: " + ec.message());
        return false;
    }

    auto copy_png = [&](const std::string& source, const std::string& name) {
        if (source.empty() || !fs::exists(source)) {
            LOG_ERROR("Cannot refresh docs screenshot " + name + ": source image is missing");
            return false;
        }
        fs::copy_file(source, docs_dir / name, fs::copy_options::overwrite_existing, ec);
        if (ec) {
            LOG_ERROR("Failed to copy docs screenshot " + name + ": " + ec.message());
            ec.clear();
            return false;
        }
        return true;
    };

    bool ok = true;
    int learned_count = db.get_words_count(chat_id, true);
    ok = copy_png(render_main_menu_image(english_level_from_learned(learned_count)), "menu.png") && ok;
    ok = copy_png(render_topic_menu_image(), "new_words.png") && ok;

    auto all_words = db.get_user_words_full(chat_id, false);
    std::vector<WordView> learned;
    for (const auto& word : all_words) {
        if (std::get<2>(word)) {
            learned.push_back(word);
        }
    }
    auto not_learned = db.get_user_words_full(chat_id, true);
    auto review_words = get_learned_words_for_review(chat_id, db);

    auto render_first_page = [](const std::vector<WordView>& words, int per_page) {
        int total = (static_cast<int>(words.size()) + per_page - 1) / per_page;
        if (total == 0) total = 1;
        int start = 0;
        int end = std::min(per_page, static_cast<int>(words.size()));
        return std::make_tuple(total, start, end);
    };

    if (!learned.empty()) {
        auto [total, start, end] = render_first_page(learned, 5);
        ok = copy_png(render_learned_words_image(chat_id, learned, 0, total, start, end), "words2.png") && ok;
    }
    if (!not_learned.empty()) {
        auto [total, start, end] = render_first_page(not_learned, 5);
        ok = copy_png(render_dictionary_words_image(chat_id, not_learned, 0, total, start, end), "words.png") && ok;
    }
    if (!review_words.empty()) {
        auto [total, start, end] = render_first_page(review_words, 5);
        ok = copy_png(render_daily_review_image(chat_id, review_words, 0, total, start, end), "morning.png") && ok;
    }
    if (!not_learned.empty()) {
        auto [total, start, end] = render_first_page(not_learned, 5);
        ok = copy_png(render_evening_words_image(chat_id, not_learned, 0, total, start, end), "evening.png") && ok;
    }

    LOG(ok ? "Docs screenshots refreshed" : "Docs screenshot refresh finished with errors");
    return ok;
}

std::vector<long long> configured_broadcast_users() {
    std::vector<long long> users;

    std::string allowed_users = trim(g_config.get("ALLOWED_USERS", ""));
    if (!allowed_users.empty()) {
        for (const auto& id_text : split_words_input(allowed_users)) {
            try {
                users.push_back(std::stoll(id_text));
            } catch (const std::exception& e) {
                LOG_ERROR("Invalid ALLOWED_USERS id: " + id_text + "; " + e.what());
            }
        }
    }

    if (users.empty()) {
        for (int i = 1; i <= 10; i++) {
            long long user_id = configured_user_id("USER_" + std::to_string(i) + "_ID");
            if (user_id > 0) {
                users.push_back(user_id);
            }
        }
    }

    return users;
}

enum class BroadcastResult {
    Delivered,
    NoContent,
    Failed
};

// send daily review with learned words to repeat
BroadcastResult send_daily_review(long long chat_id, TelegramClient& bot, Database& db) {
    auto words = get_learned_words_for_review(chat_id, db);
    bool screen_ok = show_daily_review_page(chat_id, bot, words, 0);
    if (!screen_ok) {
        LOG_ERROR("Morning review screen delivery failed for " + std::to_string(chat_id));
        return BroadcastResult::Failed;
    }

    if (words.empty()) {
        LOG("Morning review skipped for " + std::to_string(chat_id) + ": no learned words");
        return BroadcastResult::NoContent;
    }

    bool hint_ok = send_temporary_broadcast_hint(
        chat_id,
        bot,
        "Утреннее повторение выученных слов.\n\n"
        "Прочитай слова на экране и проговори их вслух. Навигация и возврат в меню доступны кнопками ниже."
    );
    return hint_ok ? BroadcastResult::Delivered : BroadcastResult::Failed;
}

BroadcastResult send_evening_new_words(long long chat_id, TelegramClient& bot, Database& db) {
    remember_screen_context(chat_id, "generation");
    bool status_ok = show_status_screen(
        chat_id,
        bot,
        "evening_generation",
        "Вечерняя подборка",
        "Генерирую новые слова для изучения.",
        "AI проверяет дубли, отсеивает слабые варианты и готовит произношение.",
        column_keyboard({{"Главное меню", "menu_main"}})
    );
    if (!status_ok) {
        LOG_WARNING("Evening generation status screen delivery failed for " + std::to_string(chat_id));
    }

    GroqClient ai;
    WordGenerationResult generation = add_generated_words_to_db(
        chat_id,
        db,
        ai,
        "mixed practical English for daily life, work, travel, food, IT and communication",
        10
    );

    LOG("Evening words generated for " + std::to_string(chat_id) +
        ": added " + std::to_string(generation.added) +
        ", duplicates " + std::to_string(generation.duplicates) +
        ", response duplicates " + std::to_string(generation.duplicate_in_response) +
        ", rejected quality " + std::to_string(generation.rejected_quality) +
        ", fallback added " + std::to_string(generation.fallback_added));

    auto words = db.get_user_words_full(chat_id, true);
    bool screen_ok = show_evening_words_page(chat_id, bot, words, 0);
    if (!screen_ok) {
        LOG_ERROR("Evening words screen delivery failed for " + std::to_string(chat_id));
        return BroadcastResult::Failed;
    }

    if (words.empty()) {
        LOG("Evening words skipped for " + std::to_string(chat_id) + ": no words to show");
        return BroadcastResult::NoContent;
    }

    if (generation.added > 0) {
        bool hint_ok = send_temporary_broadcast_hint(
            chat_id,
            bot,
            "Вечерняя подборка новых слов готова.\n\n"
            "Когда выучил слово, отправь его текстом сюда. Бот обновит прогресс и оставит навигацию на экране."
        );
        return hint_ok ? BroadcastResult::Delivered : BroadcastResult::Failed;
    } else {
        bool hint_ok = send_temporary_broadcast_hint(
            chat_id,
            bot,
            "Сегодня AI не добавил новых слов: все варианты оказались дублями или пришли в неверном формате.\n\n"
            "Показываю текущий список слов для изучения. Можно выбрать тему вручную кнопкой «Добавить слова»."
        );
        return hint_ok ? BroadcastResult::NoContent : BroadcastResult::Failed;
    }
}

std::vector<std::string> suspicious_words_for_cleanup() {
    return std::vector<std::string>(
        blocked_generated_words().begin(),
        blocked_generated_words().end()
    );
}

bool audit_bad_words(Database& db, bool remove_words) {
    auto rows = db.find_words_by_normalized_english(suspicious_words_for_cleanup());
    if (rows.empty()) {
        std::cout << "Suspicious words: 0" << std::endl;
        LOG("Suspicious word audit complete: 0 rows");
        return true;
    }

    std::cout << "Suspicious words: " << rows.size() << std::endl;
    std::vector<int> ids;
    ids.reserve(rows.size());
    for (const auto& row : rows) {
        ids.push_back(row.id);
        std::cout << "id=" << row.id
                  << " user=" << row.user_id
                  << " word=" << row.english
                  << " learned=" << (row.is_learned ? "true" : "false")
                  << " translation=" << row.translation
                  << std::endl;
    }

    if (!remove_words) {
        LOG("Suspicious word audit complete: " + std::to_string(rows.size()) + " rows");
        return true;
    }

    int removed = db.delete_words_by_ids(ids);
    std::cout << "Removed suspicious words: " << removed << std::endl;
    return removed >= 0;
}

bool run_self_tests() {
    int failed = 0;
    auto expect = [&](bool condition, const std::string& name) {
        if (condition) {
            std::cout << "[OK] " << name << std::endl;
        } else {
            std::cout << "[FAIL] " << name << std::endl;
            failed++;
        }
    };

    std::string generated =
        "WORD: receipt\n"
        "TRANS: /rɪˈsiːt/\n"
        "PRON: рисит\n"
        "MEAN: чек\n"
        "DEF: бумажное или электронное подтверждение оплаты\n"
        "---\n"
        "WORD: refund\n"
        "TRANS: /ˈriːfʌnd/\n"
        "PRON: рифанд\n"
        "MEAN: возврат денег\n"
        "DEF: деньги, которые возвращают после отмены покупки\n";
    auto words = parse_generated_words(generated);
    expect(words.size() == 2 && words[0].english == "receipt" &&
           words[1].translation == "возврат денег" &&
           words[1].definition == "деньги, которые возвращают после отмены покупки",
           "parse_generated_words parses machine-readable AI output");

    std::string definitions =
        "ID: 10\n"
        "DEF: короткое объяснение\n"
        "---\n";
    auto definition_updates = parse_definition_updates(definitions);
    expect(definition_updates.size() == 1 && definition_updates[0].id == 10 &&
           definition_updates[0].definition == "короткое объяснение",
           "parse_definition_updates parses definition backfill output");

    expect(is_suspicious_generated_word("html"), "filter blocks acronym-like weak word");
    expect(is_suspicious_generated_word("hacker"), "filter blocks weak security word");
    expect(!is_suspicious_generated_word("receipt"), "filter accepts practical word");

    auto split = split_words_input(" house, meeting , refund ");
    expect(split.size() == 3 && split[0] == "house" && split[2] == "refund",
           "split_words_input trims comma-separated words");

    auto fallback_words = fallback_practical_words();
    bool fallback_has_definitions = !fallback_words.empty() &&
        std::all_of(fallback_words.begin(), fallback_words.end(), [](const GeneratedWord& word) {
            return !trim(word.english).empty() &&
                   !trim(word.transcription).empty() &&
                   !trim(word.pronunciation).empty() &&
                   !trim(word.translation).empty() &&
                   !trim(word.definition).empty();
        });
    expect(fallback_words.size() >= 50 && fallback_has_definitions,
           "fallback_practical_words loads external dictionary with definitions");

    expect(format_ai_response_box("`int x = 1;`").find('`') != std::string::npos &&
           format_ai_response_box("`int x = 1;`").find("'''") == std::string::npos,
           "format_ai_response_box wraps answer in code block");

    fs::path config_path = fs::temp_directory_path() /
                           ("english_mentor_self_test_" + std::to_string(std::time(nullptr)) + ".env");
    bool config_file_written = write_text_file(config_path.string(),
        "EMPTY=\n"
        "QUOTED=\"hello world\"\n"
        "NUMBER=42\n"
    );
    Config cfg;
    bool config_loaded = config_file_written && cfg.load(config_path.string());
    expect(config_loaded && cfg.get("EMPTY", "fallback").empty() &&
           cfg.get("QUOTED") == "hello world" &&
           cfg.get_int("NUMBER", 0) == 42,
           "Config::load handles empty and quoted values");
    std::error_code ec;
    fs::remove(config_path, ec);

    if (failed == 0) {
        std::cout << "Self-tests passed" << std::endl;
        return true;
    }

    std::cout << "Self-tests failed: " << failed << std::endl;
    return false;
}

long long configured_user_id(const std::string& key) {
    std::string value = trim(g_config.get(key, ""));
    if (value.empty()) {
        return 0;
    }

    try {
        return std::stoll(value);
    } catch (const std::exception& e) {
        LOG_ERROR("Invalid " + key + " value: " + value + "; " + e.what());
        return 0;
    }
}

bool is_user_1(long long chat_id, long long sender_user_id) {
    long long user_1_id = configured_user_id("USER_1_ID");
    return user_1_id > 0 && (chat_id == user_1_id || sender_user_id == user_1_id);
}

// scheduler thread for morning review and evening new-word delivery
void scheduler_thread(TelegramClient& bot) {
    LOG("Scheduler thread started");
    Database scheduler_db;

    auto ensure_scheduler_db = [&]() {
        if (scheduler_db.is_connected()) {
            return true;
        }
        if (!scheduler_db.connect()) {
            LOG_ERROR("Scheduler database connection failed");
            return false;
        }
        if (!scheduler_db.init_tables()) {
            LOG_ERROR("Scheduler database initialization failed");
            scheduler_db.disconnect();
            return false;
        }
        return true;
    };

    while (true) {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm* now_tm = std::localtime(&now_time);
        if (now_tm == nullptr) {
            LOG_ERROR("Scheduler failed to read local time");
            std::this_thread::sleep_for(std::chrono::seconds(60));
            continue;
        }
        std::string date_key = local_date_key(*now_tm);

        if (now_tm->tm_hour == 9 && now_tm->tm_min < 5) {
            if (ensure_scheduler_db()) {
                LOG("Checking learned words review at 09:" + std::to_string(now_tm->tm_min));
                auto users = configured_broadcast_users();
                for (long long user_id : users) {
                    if (broadcast_was_sent(date_key, "morning", user_id)) {
                        continue;
                    }
                    BroadcastResult result = send_daily_review(user_id, bot, scheduler_db);
                    if (result != BroadcastResult::Failed) {
                        remember_broadcast_sent(date_key, "morning", user_id);
                    }
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                }
            }
        }

        if (now_tm->tm_hour == 21 && now_tm->tm_min < 5) {
            if (ensure_scheduler_db()) {
                LOG("Checking evening new words at 21:" + std::to_string(now_tm->tm_min));
                auto users = configured_broadcast_users();
                for (long long user_id : users) {
                    if (broadcast_was_sent(date_key, "evening", user_id)) {
                        continue;
                    }
                    BroadcastResult result = send_evening_new_words(user_id, bot, scheduler_db);
                    if (result != BroadcastResult::Failed) {
                        remember_broadcast_sent(date_key, "evening", user_id);
                    }
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                }
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(60));
    }
}

int main(int argc, char* argv[]) {
    std::cout << "=== English Mentor Bot v2 ===" << std::endl;

    bool cleanup_only = false;
    bool cleanup_render_cache_only = false;
    bool backfill_transcriptions_only = false;
    bool backfill_definitions_only = false;
    bool audit_words_only = false;
    bool cleanup_bad_words_only = false;
    bool self_test_only = false;
    bool send_evening_once = false;
    long long send_evening_chat_id = 0;
    bool refresh_doc_screenshots_only = false;
    long long refresh_doc_chat_id = 0;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--cleanup-db") {
            cleanup_only = true;
        } else if (arg == "--cleanup-render-cache") {
            cleanup_render_cache_only = true;
        } else if (arg == "--backfill-transcriptions") {
            backfill_transcriptions_only = true;
        } else if (arg == "--backfill-definitions") {
            backfill_definitions_only = true;
        } else if (arg == "--audit-words") {
            audit_words_only = true;
        } else if (arg == "--cleanup-bad-words") {
            cleanup_bad_words_only = true;
        } else if (arg == "--self-test") {
            self_test_only = true;
        } else if (arg == "--send-evening-once") {
            send_evening_once = true;
            if (i + 1 < argc) {
                std::string maybe_id = argv[i + 1];
                if (!maybe_id.empty() && maybe_id[0] != '-') {
                    try {
                        send_evening_chat_id = std::stoll(maybe_id);
                        i++;
                    } catch (const std::exception& e) {
                        LOG_ERROR("Invalid --send-evening-once chat id: " + maybe_id + "; " + e.what());
                        return 1;
                    }
                }
            }
        } else if (arg == "--refresh-doc-screenshots") {
            refresh_doc_screenshots_only = true;
            if (i + 1 < argc) {
                std::string maybe_id = argv[i + 1];
                if (!maybe_id.empty() && maybe_id[0] != '-') {
                    try {
                        refresh_doc_chat_id = std::stoll(maybe_id);
                        i++;
                    } catch (const std::exception& e) {
                        LOG_ERROR("Invalid --refresh-doc-screenshots chat id: " + maybe_id + "; " + e.what());
                        return 1;
                    }
                }
            }
        }
    }

    if (self_test_only) {
        return run_self_tests() ? 0 : 1;
    }

    if (cleanup_render_cache_only) {
        return cleanup_render_cache() ? 0 : 1;
    }

    if (!g_config.load(".env") && !g_config.load("../.env")) {
        LOG_ERROR("Failed to load .env");
        return 1;
    }

    if (cleanup_only || backfill_transcriptions_only || backfill_definitions_only ||
        audit_words_only || cleanup_bad_words_only || refresh_doc_screenshots_only) {
        Database db;
        if (!db.connect()) {
            return 1;
        }
        if (!db.init_tables()) {
            return 1;
        }
        if (backfill_transcriptions_only) {
            GroqClient ai;
            return backfill_missing_pronunciations(db, ai) ? 0 : 1;
        }
        if (backfill_definitions_only) {
            GroqClient ai;
            return backfill_missing_definitions(db, ai) ? 0 : 1;
        }
        if (audit_words_only) {
            return audit_bad_words(db, false) ? 0 : 1;
        }
        if (cleanup_bad_words_only) {
            return audit_bad_words(db, true) ? 0 : 1;
        }
        if (refresh_doc_screenshots_only) {
            long long target_chat_id = refresh_doc_chat_id > 0
                ? refresh_doc_chat_id
                : configured_user_id("USER_1_ID");
            return refresh_doc_screenshots(db, target_chat_id) ? 0 : 1;
        }
        LOG("Database cleanup finished");
        return 0;
    }

    std::string token = g_config.get("TELEGRAM_TOKEN");
    if (token.empty()) {
        LOG_ERROR("TELEGRAM_TOKEN not found");
        return 1;
    }

    TelegramClient bot(token);
    if (!bot.test_token()) {
        LOG_ERROR("Invalid token");
        return 1;
    }
    bot.delete_webhook(false);

    Database db;
    db.connect();
    db.init_tables();

    load_bot_state();

    if (send_evening_once) {
        long long target_chat_id = send_evening_chat_id > 0
            ? send_evening_chat_id
            : configured_user_id("USER_1_ID");
        if (target_chat_id <= 0) {
            LOG_ERROR("--send-evening-once requires a chat id or USER_1_ID in .env");
            return 1;
        }

        BroadcastResult result = send_evening_new_words(target_chat_id, bot, db);
        return result == BroadcastResult::Failed ? 1 : 0;
    }

    GroqClient ai;
    LOG("Bot started!");

    // run scheduler thread
    std::thread scheduler(scheduler_thread, std::ref(bot));
    scheduler.detach();
    LOG("Scheduler thread launched");

    int last_id = load_update_offset();
    LOG("Starting Telegram polling from update offset " + std::to_string(last_id + 1));
    std::map<long long, int> dict_page, learn_page;
    std::map<long long, int> dict_msg_id, learn_msg_id;
    std::map<long long, std::string> last_action;

    while (true) {
        try {
            std::string updates_str = bot.get_updates(last_id + 1, 10);
            if (updates_str.empty() || updates_str == "{}") continue;

            auto updates = json::parse(updates_str);
            if (!updates.value("ok", false)) {
                LOG_ERROR("getUpdates API failed: " + updates.value("description", "unknown error"));
                std::this_thread::sleep_for(std::chrono::seconds(2));
                continue;
            }

            for (auto& update : updates["result"]) {
                last_id = update["update_id"];
                save_update_offset(last_id);

                // pagination for dictionary and learned words
                if (update.contains("callback_query")) {
                    auto callback = update["callback_query"];
                    std::string callback_id = callback["id"];
                    long long chat_id = callback["message"]["chat"]["id"];
                    int message_id = callback["message"]["message_id"];
                    std::string data = callback["data"];
                    bot.answer_callback_query(callback_id);
                    remember_active_screen_message(
                        chat_id,
                        message_id,
                        screen_message_type_from_callback(callback["message"])
                    );
                    delete_tracked_broadcast_hint(chat_id, bot);
                    delete_tracked_ai_input(chat_id, bot);

                    if (data == "menu_main") {
                        send_main_menu(chat_id, bot, db, message_id);
                    }
                    else if (data == "menu_dictionary") {
                        auto words = db.get_user_words_full(chat_id, true);
                        dict_page[chat_id] = 0;
                        dict_msg_id[chat_id] = message_id;
                        show_dictionary_page(chat_id, bot, words, 0, dict_page[chat_id], last_action[chat_id], dict_msg_id[chat_id], false);
                    }
                    else if (data == "menu_learned") {
                        auto words = db.get_user_words_full(chat_id, false);
                        std::vector<WordView> learned;
                        for (const auto& w : words) if (std::get<2>(w)) learned.push_back(w);
                        learn_page[chat_id] = 0;
                        learn_msg_id[chat_id] = message_id;
                        show_learned_page(chat_id, bot, learned, 0, learn_page[chat_id], last_action[chat_id], learn_msg_id[chat_id], false);
                    }
                    else if (data == "menu_new_words") {
                        send_topic_menu(chat_id, bot, message_id);
                    }
                    else if (data == "menu_ai") {
                        show_ai_prompt(chat_id, bot, message_id);
                    }
                    else if (data == "menu_stats") {
                        show_stats(chat_id, bot, db, message_id);
                    }
                    else if (data == "topic_daily_life") {
                        generate_words(chat_id, bot, db, ai, "daily life", "Быт и дом", message_id);
                    }
                    else if (data == "topic_travel") {
                        generate_words(chat_id, bot, db, ai, "travel", "Путешествия", message_id);
                    }
                    else if (data == "topic_food") {
                        generate_words(chat_id, bot, db, ai, "food", "Еда", message_id);
                    }
                    else if (data == "topic_business") {
                        generate_words(chat_id, bot, db, ai, "business", "Работа", message_id);
                    }
                    else if (data == "topic_it_cpp") {
                        generate_words(chat_id, bot, db, ai, "IT programming", "IT и C++", message_id);
                    }
                    else if (data == "topic_communication") {
                        generate_words(chat_id, bot, db, ai, "communication", "Общение", message_id);
                    }
                    else if (data.rfind("dict_prev_", 0) == 0 || data.rfind("dict_next_", 0) == 0 ||
                             data.rfind("dict_info_", 0) == 0) {
                        auto words = db.get_user_words_full(chat_id, true);
                        size_t last_underscore = data.find_last_of('_');
                        int page = std::stoi(data.substr(last_underscore + 1));
                        dict_msg_id[chat_id] = message_id;
                        show_dictionary_page(chat_id, bot, words, page, dict_page[chat_id], last_action[chat_id], dict_msg_id[chat_id], false);
                    }
                    else if (data.rfind("learn_prev_", 0) == 0 || data.rfind("learn_next_", 0) == 0 ||
                             data.rfind("learn_info_", 0) == 0) {
                        auto all_words = db.get_user_words_full(chat_id, false);
                        std::vector<WordView> learned;
                        for (const auto& w : all_words) if (std::get<2>(w)) learned.push_back(w);
                        size_t last_underscore = data.find_last_of('_');
                        int page = std::stoi(data.substr(last_underscore + 1));
                        learn_msg_id[chat_id] = message_id;
                        show_learned_page(chat_id, bot, learned, page, learn_page[chat_id], last_action[chat_id], learn_msg_id[chat_id], false);
                    }
                    else if (data.rfind("daily_prev_", 0) == 0 || data.rfind("daily_next_", 0) == 0 ||
                             data.rfind("daily_info_", 0) == 0) {
                        size_t last_underscore = data.find_last_of('_');
                        int page = std::stoi(data.substr(last_underscore + 1));
                        auto words = get_learned_words_for_review(chat_id, db);
                        show_daily_review_page(chat_id, bot, words, page, message_id, false, &last_action[chat_id]);
                    }
                    else if (data.rfind("evening_prev_", 0) == 0 || data.rfind("evening_next_", 0) == 0 ||
                             data.rfind("evening_info_", 0) == 0) {
                        size_t last_underscore = data.find_last_of('_');
                        int page = std::stoi(data.substr(last_underscore + 1));
                        auto words = db.get_user_words_full(chat_id, true);
                        show_evening_words_page(chat_id, bot, words, page, message_id, false, &last_action[chat_id]);
                    }
                    continue;
                }

                //  only process text messages


                if (!update.contains("message") || !update["message"].contains("text")) continue;

                long long chat_id = update["message"]["chat"]["id"];
                long long sender_user_id = update["message"]["from"].value("id", 0LL);
                int incoming_message_id = update["message"]["message_id"];
                std::string text = update["message"]["text"];
                std::string name = update["message"]["from"]["first_name"];

                LOG("[" + name + "]: " + text);
                db.save_conversation(chat_id, "user", text);
                ensure_reply_keyboard_removed(chat_id, bot);
                delete_tracked_ai_input(chat_id, bot, incoming_message_id);
                delete_tracked_broadcast_hint(chat_id, bot);

                // command handling

                bool delete_incoming_after_handled = false;
                std::string normalized_text = to_lower_ascii(trim(text));

                if (text == "/start" || text == "start") {
                    delete_active_screen_message(chat_id, bot);
                    send_main_menu(chat_id, bot, db);
                    delete_incoming_after_handled = true;
                }
                else if (normalized_text == "testing" || normalized_text == "/testing") {
                    if (is_user_1(chat_id, sender_user_id)) {
                        send_daily_review(chat_id, bot, db);
                    } else {
                        LOG("testing command ignored for unauthorized chat " + std::to_string(chat_id) +
                            ", sender " + std::to_string(sender_user_id));
                    }
                    delete_incoming_after_handled = true;
                }
                else if (normalized_text == "testing_evening" || normalized_text == "/testing_evening" ||
                         normalized_text == "testing evening") {
                    if (is_user_1(chat_id, sender_user_id)) {
                        send_evening_new_words(chat_id, bot, db);
                    } else {
                        LOG("testing_evening command ignored for unauthorized chat " + std::to_string(chat_id) +
                            ", sender " + std::to_string(sender_user_id));
                    }
                    delete_incoming_after_handled = true;
                }
                else if (text == "Словарь для изучения" || text == "словарь для изучения" ||
                         text == "📚 Словарь" || text == "Словарь" || text == "словарь") {
                    auto words = db.get_user_words_full(chat_id, true);
                    dict_page[chat_id] = 0;
                    show_dictionary_page(chat_id, bot, words, 0, dict_page[chat_id], last_action[chat_id], dict_msg_id[chat_id], true);
                    delete_incoming_after_handled = true;
                }
                else if (text == "Словарь для повторения" || text == "словарь для повторения" ||
                         text == "✅ Выученные" || text == "Выученные" || text == "выученные") {
                    auto words = db.get_user_words_full(chat_id, false);
                    std::vector<WordView> learned;
                    for (const auto& w : words) if (std::get<2>(w)) learned.push_back(w);
                    learn_page[chat_id] = 0;
                    show_learned_page(chat_id, bot, learned, 0, learn_page[chat_id], last_action[chat_id], learn_msg_id[chat_id], true);
                    delete_incoming_after_handled = true;
                }
                else if (text == "Добавить слова" || text == "добавить слова" ||
                         text == "➕ Новые слова" || text == "Новые слова" || text == "новые слова") {
                    send_topic_menu(chat_id, bot);
                    delete_incoming_after_handled = true;
                }
                else if (text == "🤖 Спросить AI" || text == "Спросить AI" || text == "спросить ai") {
                    show_ai_prompt(chat_id, bot);
                    delete_incoming_after_handled = true;
                }
                else if (text == "📊 Статистика" || text == "Статистика" || text == "статистика") {
                    show_stats(chat_id, bot, db);
                    delete_incoming_after_handled = true;
                }
                else if (text == "/menu" || text == "🔙 Главное меню" || text == "Главное меню") {
                    send_main_menu(chat_id, bot, db);
                    delete_incoming_after_handled = true;
                }
                else if (text == "🏠 Быт и дом" || text == "Быт и дом") {
                    generate_words(chat_id, bot, db, ai, "daily life", "Быт и дом");
                    delete_incoming_after_handled = true;
                }
                else if (text == "✈️ Путешествия" || text == "Путешествия") {
                    generate_words(chat_id, bot, db, ai, "travel", "Путешествия");
                    delete_incoming_after_handled = true;
                }
                else if (text == "🍕 Еда" || text == "Еда") {
                    generate_words(chat_id, bot, db, ai, "food", "Еда");
                    delete_incoming_after_handled = true;
                }
                else if (text == "💼 Работа" || text == "Работа") {
                    generate_words(chat_id, bot, db, ai, "business", "Работа");
                    delete_incoming_after_handled = true;
                }
                else if (text == "💻 IT и C++" || text == "IT и C++") {
                    generate_words(chat_id, bot, db, ai, "IT programming", "IT и C++");
                    delete_incoming_after_handled = true;
                }
                else if (text == "🗣️ Общение" || text == "Общение") {
                    generate_words(chat_id, bot, db, ai, "communication", "Общение");
                    delete_incoming_after_handled = true;
                }
                else {
                    if (try_mark_words_from_input(chat_id, text, db)) {
                        refresh_after_marking_words(chat_id, bot, db,
                                                    dict_page[chat_id], dict_msg_id[chat_id],
                                                    learn_page[chat_id], learn_msg_id[chat_id],
                                                    last_action[chat_id]);
                        int confirmation_message_id = 0;
                        bot.send_message(chat_id,
                                         "Готово.\n\n Новое слово внесено в Словарь для повторения и было отмечено, как выученное.",
                                         "",
                                         &confirmation_message_id);
                        remember_broadcast_hint(chat_id, confirmation_message_id);
                        delete_messages_after_delay(bot, chat_id,
                                                    {incoming_message_id, confirmation_message_id},
                                                    10);
                    } else {
                        remember_ai_input(chat_id, incoming_message_id);
                        upsert_screen(chat_id, bot, "*Думаю...*", column_keyboard({
                            {"Главное меню", "menu_main"}
                        }));
                        std::string response = ai.ask(text);
                        upsert_screen(chat_id, bot, format_ai_response_box(response), column_keyboard({
                            {"Главное меню", "menu_main"}
                        }));
                        db.save_conversation(chat_id, "assistant", response);
                        delete_tracked_ai_input(chat_id, bot);
                    }
                }

                if (delete_incoming_after_handled) {
                    bot.delete_message(chat_id, incoming_message_id);
                }
            }
        } catch (const std::exception& e) {
            LOG_ERROR(std::string(e.what()));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}

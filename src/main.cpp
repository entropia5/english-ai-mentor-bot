
#include "config.h"
#include "logger.h"
#include "database.h"
#include "telegram_client.h"
#include "groq_client.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <map>
#include <tuple>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <mutex>
#include <fstream>
#include <cstdlib>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <unistd.h>

using json = nlohmann::json;
namespace fs = std::filesystem;
using InlineKeyboard = std::vector<std::vector<std::pair<std::string, std::string>>>;

std::string trim(const std::string& text);

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
};

struct PronunciationUpdate {
    int id = 0;
    std::string transcription;
    std::string pronunciation;
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
            if (!current.english.empty()) {
                words.push_back(current);
                current = GeneratedWord{};
            }
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

std::string build_existing_words_hint(const std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>>& words) {
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

std::string english_level_from_learned(int learned) {
    if (learned < 300) return "A1";
    if (learned < 600) return "A2";
    if (learned < 1000) return "B1";
    if (learned < 1500) return "B2";
    return "C1";
}

std::mutex g_screen_mutex;
std::map<long long, int> g_active_screen_message;
std::map<long long, bool> g_reply_keyboard_removed;
std::map<long long, int> g_last_ai_user_message;

int get_active_screen_message(long long chat_id) {
    std::lock_guard<std::mutex> lock(g_screen_mutex);
    auto it = g_active_screen_message.find(chat_id);
    return it == g_active_screen_message.end() ? 0 : it->second;
}

void remember_active_screen_message(long long chat_id, int message_id) {
    if (message_id <= 0) return;
    std::lock_guard<std::mutex> lock(g_screen_mutex);
    g_active_screen_message[chat_id] = message_id;
}

void clear_active_screen_message(long long chat_id) {
    std::lock_guard<std::mutex> lock(g_screen_mutex);
    g_active_screen_message.erase(chat_id);
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

void remember_ai_input(long long chat_id, int message_id) {
    if (message_id <= 0) return;
    std::lock_guard<std::mutex> lock(g_screen_mutex);
    g_last_ai_user_message[chat_id] = message_id;
}

void delete_messages_after_delay(TelegramClient& bot, long long chat_id,
                                 std::vector<int> message_ids, int delay_seconds) {
    std::thread([&bot, chat_id, message_ids = std::move(message_ids), delay_seconds]() {
        std::this_thread::sleep_for(std::chrono::seconds(delay_seconds));
        for (int message_id : message_ids) {
            if (message_id > 0) {
                bot.delete_message(chat_id, message_id);
            }
        }
    }).detach();
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
        {"Словарь", "menu_dictionary"},
        {"Выученные", "menu_learned"},
        {"Новые слова", "menu_new_words"},
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
        if (bot.edit_message(chat_id, message_id, text, buttons)) {
            remember_active_screen_message(chat_id, message_id);
            return true;
        }
        bot.delete_message(chat_id, message_id);
        clear_active_screen_message(chat_id);
    }

    int sent_message_id = 0;
    if (bot.send_inline_keyboard(chat_id, text, buttons, &sent_message_id)) {
        remember_active_screen_message(chat_id, sent_message_id);
        return true;
    }

    return false;
}

bool upsert_photo_screen(long long chat_id, TelegramClient& bot, const std::string& photo_path,
                         const InlineKeyboard& buttons, int preferred_message_id = 0) {
    int message_id = preferred_message_id > 0 ? preferred_message_id : get_active_screen_message(chat_id);
    if (message_id > 0) {
        if (bot.edit_message_photo(chat_id, message_id, photo_path, buttons)) {
            remember_active_screen_message(chat_id, message_id);
            return true;
        }
        bot.delete_message(chat_id, message_id);
        clear_active_screen_message(chat_id);
    }

    int sent_message_id = 0;
    if (bot.send_photo(chat_id, photo_path, buttons, "", &sent_message_id)) {
        remember_active_screen_message(chat_id, sent_message_id);
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

std::string shell_quote(const std::string& text) {
    std::string result = "'";
    for (char c : text) {
        if (c == '\'') {
            result += "'\\''";
        } else {
            result += c;
        }
    }
    result += "'";
    return result;
}

std::string project_data_dir() {
    if (fs::exists("CMakeLists.txt")) {
        return "data";
    }
    if (fs::exists("../CMakeLists.txt")) {
        return "../data";
    }
    return "data";
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
    const std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>>& words,
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
        const auto& [eng, trans, learned, pron, transcr] = words[i];
        hash = fnv1a_update(hash, eng);
        hash = fnv1a_update(hash, "\n");
        hash = fnv1a_update(hash, trans);
        hash = fnv1a_update(hash, "\n");
        hash = fnv1a_update(hash, pron);
        hash = fnv1a_update(hash, "\n");
        hash = fnv1a_update(hash, transcr);
        hash = fnv1a_update(hash, "\n");
    }

    std::stringstream ss;
    ss << std::hex << hash;
    return ss.str();
}

std::string read_text_file(const std::string& path) {
    std::ifstream file(path);
    if (!file) return "";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return trim(buffer.str());
}

bool write_text_file(const std::string& path, const std::string& text) {
    std::ofstream file(path);
    if (!file) return false;
    file << text;
    return true;
}

std::string render_html_image(const fs::path& base, const std::string& hash_value,
                              const std::string& html_content, const std::string& log_name) {
    std::error_code ec;
    fs::create_directories(base.parent_path(), ec);
    if (ec) {
        LOG_ERROR("Failed to create render directory for " + log_name + ": " +
                  base.parent_path().string() + "; " + ec.message());
        return "";
    }

    std::string html_path = base.string() + ".html";
    std::string png_path = base.string() + ".png";
    std::string hash_path = base.string() + ".hash";

    if (fs::exists(png_path) && read_text_file(hash_path) == hash_value) {
        LOG("Reusing " + log_name + " image: " + png_path);
        return png_path;
    }

    if (!write_text_file(html_path, html_content)) {
        LOG_ERROR("Failed to write " + log_name + " HTML: " + html_path);
        return "";
    }

    std::string command = "wkhtmltoimage --quiet --width 1080 --disable-smart-width " +
                          shell_quote(html_path) + " " + shell_quote(png_path);
    int rc = std::system(command.c_str());
    if (rc != 0) {
        LOG_ERROR("wkhtmltoimage " + log_name + " failed with code " + std::to_string(rc));
        return "";
    }

    if (!write_text_file(hash_path, hash_value)) {
        LOG_ERROR("Failed to write " + log_name + " image hash: " + hash_path);
    }

    return png_path;
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

std::string render_main_menu_image(const std::string& user_level) {
    fs::path render_dir = fs::path(project_data_dir()) / "rendered" / "menu";
    std::error_code ec;
    fs::create_directories(render_dir, ec);
    if (ec) {
        LOG_ERROR("Failed to create main menu render directory: " + render_dir.string() + "; " + ec.message());
        return "";
    }

    fs::path base = render_dir / ("main_menu_" + user_level);
    std::string html_path = base.string() + ".html";
    std::string png_path = base.string() + ".png";
    std::string hash_path = base.string() + ".hash";
    std::string menu_hash = "main-menu-graphite-v5-title-underline-" + user_level;

    if (fs::exists(png_path) && read_text_file(hash_path) == menu_hash) {
        LOG("Reusing main menu image: " + png_path);
        return png_path;
    }

    std::ofstream html(html_path);
    if (!html) {
        LOG_ERROR("Failed to create main menu HTML: " + html_path);
        return "";
    }

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
      <div class="brand">English AI Mentor</div>
      <div class="title-row">
        <div class="title">Главное<br>меню</div>
        <div class="line"></div>
      </div>
      <div class="subtitle">Выбери действие кнопками под этой карточкой.</div>
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
    html.close();

    std::string command = "wkhtmltoimage --quiet --width 1080 --disable-smart-width " +
                          shell_quote(html_path) + " " + shell_quote(png_path);
    int rc = std::system(command.c_str());
    if (rc != 0) {
        LOG_ERROR("wkhtmltoimage main menu failed with code " + std::to_string(rc));
        return "";
    }

    if (!write_text_file(hash_path, menu_hash)) {
        LOG_ERROR("Failed to write main menu image hash: " + hash_path);
    }

    return png_path;
}

std::string render_ai_prompt_image() {
    fs::path base = fs::path(project_data_dir()) / "rendered" / "ai" / "prompt";
    std::string hash = "ai-prompt-graphite-v3-title-underline";
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
      <div class="label">English AI Mentor</div>
      <div class="title-row">
        <div class="title">Режим<br>AI</div>
        <div class="line"></div>
      </div>
      <div class="text">Задай любой вопрос по английскому языку.</div>
    </div>
    <div class="hint">Я отвечу на активном экране бота. Для возврата используй кнопку под карточкой.</div>
  </div>
</div>
</body>
</html>
)";

    return render_html_image(base, hash, html.str(), "AI prompt");
}

std::string render_topic_menu_image() {
    fs::path base = fs::path(project_data_dir()) / "rendered" / "topics" / "topic_menu";
    std::string hash = "topic-menu-graphite-v3-clean-title-underline";
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
      <div class="label">English AI Mentor</div>
      <div class="title-row">
        <div class="title">Новые<br>слова</div>
        <div class="line"></div>
      </div>
      <div class="text">Выбери тему кнопками под карточкой. AI добавит 10 практичных слов с переводом, IPA-транскрипцией и русским произношением.</div>
    </div>
  </div>
</div>
</body>
</html>
)";

    return render_html_image(base, hash, html.str(), "topic menu");
}

std::string render_stats_image(int total, int learned, const std::string& level,
                               const std::string& next_name, int next_level,
                               int percent) {
    fs::path base = fs::path(project_data_dir()) / "rendered" / "stats" /
                    ("stats_" + level + "_" + std::to_string(total) + "_" +
                     std::to_string(learned) + "_" + std::to_string(percent));
    std::string hash = "stats-graphite-v3-title-underline-" + std::to_string(total) + "-" +
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
      <div class="label">English AI Mentor</div>
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
    const std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>>& words,
    int page,
    int total,
    int start,
    int end,
    const std::string& folder,
    const std::string& title,
    const std::string& subtitle,
    const std::string& footer_left
) {
    fs::path render_dir = fs::path(project_data_dir()) / "rendered" / folder / std::to_string(chat_id);
    std::error_code ec;
    fs::create_directories(render_dir, ec);
    if (ec) {
        LOG_ERROR("Failed to create render directory: " + render_dir.string() + "; " + ec.message());
        return "";
    }

    fs::path page_base = render_dir / ("page_" + std::to_string(page + 1));
    std::string html_path = (page_base.string() + ".html");
    std::string png_path = (page_base.string() + ".png");
    std::string hash_path = (page_base.string() + ".hash");
    std::string page_hash = folder + "-graphite-mobile-v7-wide-checkword-" + learned_page_hash(words, page, total, start, end);

    if (fs::exists(png_path) && read_text_file(hash_path) == page_hash) {
        LOG("Reusing words card image: " + png_path);
        return png_path;
    }

    std::ofstream html(html_path);
    if (!html) {
        LOG_ERROR("Failed to create words card HTML: " + html_path);
        return "";
    }

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
.translation { margin-top: 8px; padding-left: 60px; font-size: 40px; line-height: 1.2; color: #cfd7de; }
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
        const auto& [eng, trans, learned, pron, transcr] = words[i];
        std::string ipa = trim(transcr);
        if (!ipa.empty() && ipa.front() != '/' && ipa.front() != '[') {
            ipa = "/" + ipa + "/";
        }
        std::string pronunciation_ru = trim(pron);

        html << "<div class=\"item\">"
             << "<div class=\"word-line\"><span class=\"mark\"></span><span class=\"word\">" << html_escape(eng) << "</span>";
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
             << "<div class=\"translation\">" << html_escape(trans) << "</div>"
             << "</div>\n";
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
    html.close();

    std::string command = "wkhtmltoimage --quiet --width 1440 --disable-smart-width " +
                          shell_quote(html_path) + " " + shell_quote(png_path);
    int rc = std::system(command.c_str());
    if (rc != 0) {
        LOG_ERROR("wkhtmltoimage failed with code " + std::to_string(rc));
        return "";
    }

    if (!write_text_file(hash_path, page_hash)) {
        LOG_ERROR("Failed to write learned words image hash: " + hash_path);
    }

    return png_path;
}

std::string render_learned_words_image(
    long long chat_id,
    const std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>>& words,
    int page,
    int total,
    int start,
    int end
) {
    return render_words_card_image(chat_id, words, page, total, start, end,
                                   "learned", "Выученные слова",
                                   "Закрепленный словарь",
                                   "English AI Mentor");
}

std::string render_daily_review_image(
    long long chat_id,
    const std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>>& words,
    int page,
    int total,
    int start,
    int end
) {
    return render_words_card_image(chat_id, words, page, total, start, end,
                                   "daily", "Доброе утро",
                                   "Повторение выученных слов",
                                   "Закрепи утренней практикой");
}

std::string render_evening_words_image(
    long long chat_id,
    const std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>>& words,
    int page,
    int total,
    int start,
    int end
) {
    return render_words_card_image(chat_id, words, page, total, start, end,
                                   "evening", "Новые слова",
                                   "Вечерняя подборка для изучения",
                                   "21:00 English AI Mentor");
}

// better formatting for words in the dictionary
std::string format_word(const std::string& english, const std::string& translation,
                        const std::string& transcription, const std::string& pronunciation_ru) {
    std::string result;
    result += "🇺🇸 *" + english + "*\n";
    if (!transcription.empty()) result += "🏳️ " + transcription + "\n";
    if (!pronunciation_ru.empty()) result += "🏴 " + pronunciation_ru + "\n";
    result += "🇷🇺 " + translation;
    return result;
}

// send or edit main menu with inline buttons
void send_main_menu(long long chat_id, TelegramClient& bot, Database& db, int message_id = 0) {
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
                          const std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>>& words,
                          int page, int& current_page, std::string& last_action,
                          int& message_id, bool is_new = true) {
    int per_page = 5;
    int total = (words.size() + per_page - 1) / per_page;
    if (total == 0) total = 1;
    if (page < 0) page = 0;
    if (page >= total) page = total - 1;
    current_page = page;
    last_action = "dictionary";

    int start = page * per_page;
    int end = std::min(start + per_page, (int)words.size());

    std::string msg = "📚 *Словарь для изучения*\n";
    msg += "▫️ Страница " + std::to_string(page + 1) + " из " + std::to_string(total) + "\n";
    msg += "▫️ Чтобы отметить слово - просто напиши его\n\n";

    for (int i = start; i < end; i++) {
        const auto& [eng, trans, learned, pron, transcr] = words[i];
        msg += format_word(eng, trans, transcr, pron) + "\n\n";
    }

    if (words.empty()) {
        msg = "📚 *Словарь пуст*\n\n➕ Нажми «➕ Новые слова» чтобы добавить слова!";
        upsert_screen(chat_id, bot, msg, column_keyboard({
            {"Новые слова", "menu_new_words"},
            {"Главное меню", "menu_main"}
        }), is_new ? 0 : message_id);
        return;
    }

    InlineKeyboard buttons = page_keyboard("dict_", page, total);

    if (is_new) {
        upsert_screen(chat_id, bot, msg, buttons);
    } else {
        if (bot.edit_message(chat_id, message_id, msg, buttons)) {
            remember_active_screen_message(chat_id, message_id);
        } else {
            upsert_screen(chat_id, bot, msg, buttons);
        }
    }
}

// show learned words page with INLINE buttons for pagination
void show_learned_page(long long chat_id, TelegramClient& bot,
                       const std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>>& words,
                       int page, int& current_page, std::string& last_action,
                       int& message_id, bool is_new = true) {
    int per_page = 7;
    int total = (words.size() + per_page - 1) / per_page;
    if (total == 0) total = 1;
    if (page < 0) page = 0;
    if (page >= total) page = total - 1;
    current_page = page;
    last_action = "learned";

    if (words.empty()) {
        std::string msg = "✅ *Выученных слов пока нет*\n\n📚 Учи слова из словаря!";
        upsert_screen(chat_id, bot, msg, column_keyboard({
            {"Словарь", "menu_dictionary"},
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
        if (bot.edit_message_photo(chat_id, message_id, image_path, buttons)) {
            remember_active_screen_message(chat_id, message_id);
        } else {
            upsert_photo_screen(chat_id, bot, image_path, buttons);
        }
    }
}

// show daily review page with INLINE buttons for pagination
void show_daily_review_page(long long chat_id, TelegramClient& bot,
                            const std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>>& words,
                            int page, int message_id = 0, bool is_new = true, std::string* last_action = nullptr) {
    int per_page = 7;
    int total = (words.size() + per_page - 1) / per_page;
    if (total == 0) total = 1;
    if (page < 0) page = 0;
    if (page >= total) page = total - 1;
    if (last_action != nullptr) {
        *last_action = "daily";
    }

    if (words.empty()) {
        std::string msg = "*Доброе утро!*\n\nПока нет выученных слов для повторения.";
        upsert_screen(chat_id, bot, msg, column_keyboard({
            {"Выученные", "menu_learned"},
            {"Главное меню", "menu_main"}
        }), is_new ? 0 : message_id);
        return;
    }

    int start = page * per_page;
    int end = std::min(start + per_page, (int)words.size());
    std::string image_path = render_daily_review_image(chat_id, words, page, total, start, end);
    InlineKeyboard buttons = page_keyboard("daily_", page, total);

    if (image_path.empty()) {
        std::string msg = "*Доброе утро*\n\nНе удалось собрать картинку повторения. Попробуй открыть раздел еще раз.";
        upsert_screen(chat_id, bot, msg, buttons, is_new ? 0 : message_id);
    } else if (is_new) {
        upsert_photo_screen(chat_id, bot, image_path, buttons);
    } else {
        if (bot.edit_message_photo(chat_id, message_id, image_path, buttons)) {
            remember_active_screen_message(chat_id, message_id);
        } else {
            upsert_photo_screen(chat_id, bot, image_path, buttons);
        }
    }
}

void show_evening_words_page(long long chat_id, TelegramClient& bot,
                             const std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>>& words,
                             int page, int message_id = 0, bool is_new = true, std::string* last_action = nullptr) {
    int per_page = 7;
    int total = (words.size() + per_page - 1) / per_page;
    if (total == 0) total = 1;
    if (page < 0) page = 0;
    if (page >= total) page = total - 1;
    if (last_action != nullptr) {
        *last_action = "evening";
    }

    if (words.empty()) {
        std::string msg = "*Новые слова*\n\nПока нет слов для изучения.";
        upsert_screen(chat_id, bot, msg, column_keyboard({
            {"Новые слова", "menu_new_words"},
            {"Главное меню", "menu_main"}
        }), is_new ? 0 : message_id);
        return;
    }

    int start = page * per_page;
    int end = std::min(start + per_page, (int)words.size());
    std::string image_path = render_evening_words_image(chat_id, words, page, total, start, end);
    InlineKeyboard buttons = page_keyboard("evening_", page, total);

    if (image_path.empty()) {
        std::string msg = "*Новые слова*\n\nНе удалось собрать картинку. Попробуй открыть раздел еще раз.";
        upsert_screen(chat_id, bot, msg, buttons, is_new ? 0 : message_id);
    } else if (is_new) {
        upsert_photo_screen(chat_id, bot, image_path, buttons);
    } else {
        if (bot.edit_message_photo(chat_id, message_id, image_path, buttons)) {
            remember_active_screen_message(chat_id, message_id);
        } else {
            upsert_photo_screen(chat_id, bot, image_path, buttons);
        }
    }
}

// statistics page with progress bar and levels
void show_stats(long long chat_id, TelegramClient& bot, Database& db, int message_id = 0) {
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
    std::string image_path = render_stats_image(total, learned, level, next_name, next_level, percent);
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
            db.mark_word_learned(chat_id, word);
            marked_words.push_back(word);
        } else if (requested_words.size() > 1) {
            not_found_words.push_back(word);
        }
    }

    if (marked_words.empty()) return false;

    return true;
}

std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>>
get_learned_words_for_review(long long chat_id, Database& db);

void refresh_after_marking_words(long long chat_id, TelegramClient& bot, Database& db,
                                 int& dict_current_page, int& dict_message_id,
                                 int& learn_current_page, int& learn_message_id,
                                 std::string& current_action) {
    if (current_action == "daily") {
        auto words = get_learned_words_for_review(chat_id, db);
        show_daily_review_page(chat_id, bot, words, 0, get_active_screen_message(chat_id), false, &current_action);
        return;
    }

    if (current_action == "evening") {
        auto words = db.get_user_words_full(chat_id, true);
        show_evening_words_page(chat_id, bot, words, 0, get_active_screen_message(chat_id), false, &current_action);
        return;
    }

    auto words = db.get_user_words_full(chat_id, true);
    int page = current_action == "dictionary" ? dict_current_page : 0;
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

struct WordGenerationResult {
    int added = 0;
    int duplicates = 0;
    int parsed_total = 0;
    int failed = 0;
};

WordGenerationResult add_generated_words_to_db(long long chat_id, Database& db, GroqClient& ai,
                                               const std::string& topic_keyword, int target_count = 10) {
    WordGenerationResult result;

    for (int attempt = 1; attempt <= 3 && result.added < target_count; attempt++) {
        auto existing_words = db.get_user_words_full(chat_id, false);
        int need = target_count - result.added;

        std::string prompt = "Generate exactly " + std::to_string(need) +
            " NEW English words on topic: " + topic_keyword +
            ". Use practical words for language learners. Do not repeat words from the avoid list. "
            "For every word, TRANS must be the original English IPA transcription with slashes, "
            "and PRON must be a Russian Cyrillic pronunciation hint." +
            build_existing_words_hint(existing_words) +
            "\nReturn ONLY blocks in this exact format, no numbering and no extra text:\n"
            "WORD: word\n"
            "TRANS: /IPA transcription/\n"
            "PRON: русское произношение кириллицей\n"
            "MEAN: russian translation\n"
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

            if (db.word_exists(chat_id, word.english)) {
                result.duplicates++;
                LOG("Generated duplicate skipped before insert: " + word.english);
                continue;
            }

            LOG("Adding word: " + word.english + " -> " + word.translation);
            if (db.add_word(chat_id, word.english, word.translation, word.pronunciation,
                            word.transcription, topic_keyword)) {
                result.added++;
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
    upsert_screen(chat_id, bot, "*Генерирую слова на тему: " + topic_name + "*\n\nПодожди...",
                  column_keyboard({{"Главное меню", "menu_main"}}), message_id);

    WordGenerationResult generation = add_generated_words_to_db(chat_id, db, ai, topic_keyword, 10);

    if (generation.added > 0) {
        std::string msg = "✅ *Добавлено " + std::to_string(generation.added) + " новых слов!*\n📚 Смотри в разделе «Словарь»";
        if (generation.added < 10) {
            msg += "\n\n▫️ Меньше 10, потому что часть слов уже была в словаре";
            if (generation.duplicates > 0) {
                msg += " (" + std::to_string(generation.duplicates) + " дубл.)";
            }
        }
        upsert_screen(chat_id, bot, msg, column_keyboard({
            {"Словарь", "menu_dictionary"},
            {"Новые слова", "menu_new_words"},
            {"Главное меню", "menu_main"}
        }));
    } else {
        std::string msg = "❌ *Не удалось добавить новые слова*";
        if (generation.duplicates > 0) {
            msg += "\n\nAI предложил только слова, которые уже есть в словаре.";
        } else if (generation.parsed_total == 0) {
            msg += "\n\nAI вернул ответ в неожиданном формате.";
        }
        msg += "\nПопробуй другую тему";
        upsert_screen(chat_id, bot, msg, column_keyboard({
            {"Новые слова", "menu_new_words"},
            {"Главное меню", "menu_main"}
        }));
    }
}

long long configured_user_id(const std::string& key);

std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>>
get_learned_words_for_review(long long chat_id, Database& db) {
    auto all_words = db.get_user_words_full(chat_id, false);
    std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>> learned;
    for (const auto& word : all_words) {
        if (std::get<2>(word)) {
            learned.push_back(word);
        }
    }
    return learned;
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

// send daily review with learned words to repeat
void send_daily_review(long long chat_id, TelegramClient& bot, Database& db) {
    auto words = get_learned_words_for_review(chat_id, db);
    show_daily_review_page(chat_id, bot, words, 0);
}

void send_evening_new_words(long long chat_id, TelegramClient& bot, Database& db) {
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
        ", duplicates " + std::to_string(generation.duplicates));

    auto words = db.get_user_words_full(chat_id, true);
    show_evening_words_page(chat_id, bot, words, 0);
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
void scheduler_thread(TelegramClient& bot, Database& db) {
    LOG("Scheduler thread started");
    bool morning_sent_today = false;
    bool evening_sent_today = false;
    int morning_day = -1;
    int evening_day = -1;

    while (true) {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm* now_tm = std::localtime(&now_time);

        if (now_tm->tm_hour == 9 && now_tm->tm_min < 5) {
            if (morning_day != now_tm->tm_yday) {
                morning_sent_today = false;
                morning_day = now_tm->tm_yday;
            }

            if (!morning_sent_today) {
                LOG("Sending learned words review at 09:" + std::to_string(now_tm->tm_min));
                auto users = configured_broadcast_users();
                for (long long user_id : users) {
                    send_daily_review(user_id, bot, db);
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                }

                morning_sent_today = true;
            }
        }

        if (now_tm->tm_hour == 21 && now_tm->tm_min < 5) {
            if (evening_day != now_tm->tm_yday) {
                evening_sent_today = false;
                evening_day = now_tm->tm_yday;
            }

            if (!evening_sent_today) {
                LOG("Sending evening new words at 21:" + std::to_string(now_tm->tm_min));
                auto users = configured_broadcast_users();
                for (long long user_id : users) {
                    send_evening_new_words(user_id, bot, db);
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                }

                evening_sent_today = true;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(60));
    }
}

int main(int argc, char* argv[]) {
    std::cout << "=== English Mentor Bot v2 ===" << std::endl;

    if (!g_config.load(".env") && !g_config.load("../.env")) {
        LOG_ERROR("Failed to load .env");
        return 1;
    }

    bool cleanup_only = false;
    bool backfill_transcriptions_only = false;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--cleanup-db") {
            cleanup_only = true;
        } else if (arg == "--backfill-transcriptions") {
            backfill_transcriptions_only = true;
        }
    }

    if (cleanup_only || backfill_transcriptions_only) {
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

    GroqClient ai;
    LOG("Bot started!");

    // run scheduler thread
    std::thread scheduler(scheduler_thread, std::ref(bot), std::ref(db));
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
                    remember_active_screen_message(chat_id, message_id);
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
                        std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>> learned;
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
                        std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>> learned;
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

                // command handling

                bool delete_incoming_after_handled = false;
                std::string normalized_text = to_lower_ascii(trim(text));

                if (text == "/start" || text == "start") {
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
                else if (text == "📚 Словарь" || text == "Словарь" || text == "словарь") {
                    auto words = db.get_user_words_full(chat_id, true);
                    dict_page[chat_id] = 0;
                    show_dictionary_page(chat_id, bot, words, 0, dict_page[chat_id], last_action[chat_id], dict_msg_id[chat_id], true);
                    delete_incoming_after_handled = true;
                }
                else if (text == "✅ Выученные" || text == "Выученные" || text == "выученные") {
                    auto words = db.get_user_words_full(chat_id, false);
                    std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>> learned;
                    for (const auto& w : words) if (std::get<2>(w)) learned.push_back(w);
                    learn_page[chat_id] = 0;
                    show_learned_page(chat_id, bot, learned, 0, learn_page[chat_id], last_action[chat_id], learn_msg_id[chat_id], true);
                    delete_incoming_after_handled = true;
                }
                else if (text == "➕ Новые слова" || text == "Новые слова" || text == "новые слова") {
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
                                         "Отлично, слово теперь в Вашем словаре выученных слов.",
                                         "",
                                         &confirmation_message_id);
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

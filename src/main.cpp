
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
#include <nlohmann/json.hpp>
#include <unistd.h>

using json = nlohmann::json;

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

// send main menu with KEYBOARD buttons
void send_main_menu(long long chat_id, TelegramClient& bot) {
    std::vector<std::vector<std::string>> keyboard = {
        {"📚 Словарь", "✅ Выученные", "➕ Новые слова"},
        {"🤖 Спросить AI", "📊 Статистика"}
    };
    std::string menu_text = "📚 *Главное меню*\n\n Нажми на кнопку из меню:";
    bot.send_message_with_keyboard(chat_id, menu_text, keyboard, true, false);
}

// send topic selection menu with KEYBOARD buttons
void send_topic_menu(long long chat_id, TelegramClient& bot) {
    std::vector<std::vector<std::string>> keyboard = {
        {"🏠 Быт и дом", "✈️ Путешествия", "🍕 Еда"},
        {"💼 Работа", "💻 IT и C++", "🗣️ Общение"},
        {"🔙 Главное меню"}
    };
    std::string menu_text = "📚 *Выбери тему для новых слов:*\n\n AI сейчас подберет 10 новых слов.";
    bot.send_message_with_keyboard(chat_id, menu_text, keyboard, true, false);
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
        if (is_new) {
            bot.send_message(chat_id, msg);
        } else {
            bot.edit_message(chat_id, message_id, msg);
        }
        return;
    }

    // INLINE buttons for pagination
    std::vector<std::vector<std::pair<std::string, std::string>>> buttons;
    std::vector<std::pair<std::string, std::string>> row;

    if (page > 0) {
        row.push_back({"◀️ Назад", "dict_prev"});
    }
    if (page < total - 1) {
        row.push_back({"Вперед ▶️", "dict_next"});
    }
    if (!row.empty()) {
        buttons.push_back(row);
    }

    if (is_new) {
        bot.send_inline_keyboard(chat_id, msg, buttons);
    } else {
        bot.edit_message(chat_id, message_id, msg, buttons);
    }
}

// show learned words page with INLINE buttons for pagination
void show_learned_page(long long chat_id, TelegramClient& bot,
                       const std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>>& words,
                       int page, int& current_page, std::string& last_action,
                       int& message_id, bool is_new = true) {
    int per_page = 10;
    int total = (words.size() + per_page - 1) / per_page;
    if (total == 0) total = 1;
    if (page < 0) page = 0;
    if (page >= total) page = total - 1;
    current_page = page;
    last_action = "learned";

    int start = page * per_page;
    int end = std::min(start + per_page, (int)words.size());

    std::string msg = "✅ *Выученные слова*\n";
    msg += "▫️ Страница " + std::to_string(page + 1) + " из " + std::to_string(total) + "\n\n";

    for (int i = start; i < end; i++) {
        const auto& [eng, trans, learned, pron, transcr] = words[i];
        msg += "✅ *" + eng + "* — " + trans + "\n";
    }

    if (words.empty()) {
        msg = "✅ *Выученных слов пока нет*\n\n📚 Учи слова из словаря!";
        if (is_new) {
            bot.send_message(chat_id, msg);
        } else {
            bot.edit_message(chat_id, message_id, msg);
        }
        return;
    }

    // INLINE buttons for pagination
    std::vector<std::vector<std::pair<std::string, std::string>>> buttons;
    std::vector<std::pair<std::string, std::string>> row;

    if (page > 0) {
        row.push_back({"◀️ Назад", "learn_prev"});
    }
    if (page < total - 1) {
        row.push_back({"Вперед ▶️", "learn_next"});
    }
    if (!row.empty()) {
        buttons.push_back(row);
    }

    if (is_new) {
        bot.send_inline_keyboard(chat_id, msg, buttons);
    } else {
        bot.edit_message(chat_id, message_id, msg, buttons);
    }
}

// show daily review page with INLINE buttons for pagination
void show_daily_review_page(long long chat_id, TelegramClient& bot,
                            const std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>>& words,
                            int page, int message_id = 0, bool is_new = true) {
    int per_page = 10;
    int total = (words.size() + per_page - 1) / per_page;
    if (total == 0) total = 1;
    if (page < 0) page = 0;
    if (page >= total) page = total - 1;

    if (words.empty()) {
        std::string msg = "🌅 *Доброе утро!*\n\n📚 У тебя нет новых слов для повторения.\n\n➕ Нажми «➕ Новые слова» чтобы добавить!";
        if (is_new) {
            bot.send_message(chat_id, msg);
        } else {
            bot.edit_message(chat_id, message_id, msg);
        }
        return;
    }

    int start = page * per_page;
    int end = std::min(start + per_page, (int)words.size());

    std::string msg = "🌅 *Доброе утро!*\n\n";
    msg += "📚 *Слова для повторения сегодня:*\n";
    msg += "▫️ Страница " + std::to_string(page + 1) + " из " + std::to_string(total) + "\n\n";

    for (int i = start; i < end; i++) {
        const auto& [eng, trans, learned, pron, transcr] = words[i];
        msg += format_word(eng, trans, transcr, pron) + "\n\n";
    }

    msg += "💡 *Совет:* Напиши слово, чтобы отметить его как выученное!";

    std::vector<std::vector<std::pair<std::string, std::string>>> buttons;
    std::vector<std::pair<std::string, std::string>> row;

    if (page > 0) {
        row.push_back({"◀️ Назад", "daily_" + std::to_string(page - 1)});
    }
    if (page < total - 1) {
        row.push_back({"Вперед ▶️", "daily_" + std::to_string(page + 1)});
    }
    if (!row.empty()) {
        buttons.push_back(row);
    }

    if (is_new) {
        bot.send_inline_keyboard(chat_id, msg, buttons);
    } else {
        bot.edit_message(chat_id, message_id, msg, buttons);
    }
}

// statistics page with progress bar and levels
void show_stats(long long chat_id, TelegramClient& bot, Database& db) {
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
    int bars = percent / 10;

    std::string msg = "📊 *Статистика*\n\n";
    msg += "▫️ *Уровень:* " + level + "\n";
    msg += "▫️ *Всего слов:* " + std::to_string(total) + "\n";
    msg += "▫️ *Выучено:* " + std::to_string(learned) + "\n\n";
    msg += "▫️";
    for (int i = 0; i < 10; i++) msg += (i < bars) ? "▪️" : "▫️";
    msg += " " + std::to_string(percent) + "%\n\n";

    if (next_level > 0) {
        msg += "🎯 До " + next_name + " осталось " + std::to_string(next_level) + " слов";
    } else {
        msg += "🏆 Ты достиг уровня C1!";
    }
    bot.send_message(chat_id, msg);
}

bool try_mark_words_from_input(long long chat_id, const std::string& text, TelegramClient& bot, Database& db) {
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

    std::string msg = "🎉 *Отлично!*\n\n";
    msg += "✅ Отмечено как выученное: ";
    for (size_t i = 0; i < marked_words.size(); i++) {
        if (i > 0) msg += ", ";
        msg += "*" + marked_words[i] + "*";
    }

    if (!not_found_words.empty()) {
        msg += "\n\nНе нашел в словаре: ";
        for (size_t i = 0; i < not_found_words.size(); i++) {
            if (i > 0) msg += ", ";
            msg += not_found_words[i];
        }
    }

    bot.send_message(chat_id, msg);
    return true;
}

// generate words using AI and add to database
void generate_words(long long chat_id, TelegramClient& bot, Database& db, GroqClient& ai,
                    const std::string& topic_keyword, const std::string& topic_name) {
    bot.send_message(chat_id, "🤖 *Генерирую слова на тему: " + topic_name + "*\n⏳ Подожди...");

    int added = 0;
    int duplicates = 0;
    int parsed_total = 0;
    int failed = 0;

    for (int attempt = 1; attempt <= 3 && added < 10; attempt++) {
        auto existing_words = db.get_user_words_full(chat_id, false);
        int need = 10 - added;

        std::string prompt = "Generate exactly " + std::to_string(need) +
            " NEW English words on topic: " + topic_keyword +
            ". Use practical words for language learners. Do not repeat words from the avoid list." +
            build_existing_words_hint(existing_words) +
            "\nReturn ONLY blocks in this exact format, no numbering and no extra text:\n"
            "WORD: word\n"
            "TRANS: /transcription/\n"
            "PRON: russian pronunciation\n"
            "MEAN: russian translation\n"
            "---\n";

        std::string response = ai.ask(prompt);
        LOG("AI Response length: " + std::to_string(response.length()) +
            ", attempt: " + std::to_string(attempt));

        auto generated_words = parse_generated_words(response);
        parsed_total += generated_words.size();
        LOG("Parsed generated words: " + std::to_string(generated_words.size()));

        if (generated_words.empty()) {
            failed++;
            continue;
        }

        for (const auto& word : generated_words) {
            if (added >= 10) break;

            if (db.word_exists(chat_id, word.english)) {
                duplicates++;
                LOG("Generated duplicate skipped before insert: " + word.english);
                continue;
            }

            LOG("Adding word: " + word.english + " -> " + word.translation);
            if (db.add_word(chat_id, word.english, word.translation, word.pronunciation,
                            word.transcription, topic_keyword)) {
                added++;
            } else {
                failed++;
            }
        }
    }

    if (added > 0) {
        std::string msg = "✅ *Добавлено " + std::to_string(added) + " новых слов!*\n📚 Смотри в разделе «Словарь»";
        if (added < 10) {
            msg += "\n\n▫️ Меньше 10, потому что часть слов уже была в словаре";
            if (duplicates > 0) {
                msg += " (" + std::to_string(duplicates) + " дубл.)";
            }
        }
        bot.send_message(chat_id, msg);
    } else {
        std::string msg = "❌ *Не удалось добавить новые слова*";
        if (duplicates > 0) {
            msg += "\n\nAI предложил только слова, которые уже есть в словаре.";
        } else if (parsed_total == 0) {
            msg += "\n\nAI вернул ответ в неожиданном формате.";
        }
        msg += "\nПопробуй другую тему";
        bot.send_message(chat_id, msg);
    }
}

// send daily review with words to repeat
void send_daily_review(long long chat_id, TelegramClient& bot, Database& db) {
    auto words = db.get_user_words_full(chat_id, true);
    show_daily_review_page(chat_id, bot, words, 0);
}

// sheduler thread to send daily reviews at 9 AM
void scheduler_thread(TelegramClient& bot, Database& db) {
    LOG("Scheduler thread started");

    while (true) {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm* now_tm = std::localtime(&now_time);

        if (now_tm->tm_hour == 9 && now_tm->tm_min < 5) {
            static bool sent_today = false;
            static int last_day = -1;

            if (last_day != now_tm->tm_yday) {
                sent_today = false;
                last_day = now_tm->tm_yday;
            }

            if (!sent_today) {
                LOG("Sending daily review at 09:" + std::to_string(now_tm->tm_min));

                std::vector<long long> users = {8729072841LL, 7772561460LL};
                for (long long user_id : users) {
                    send_daily_review(user_id, bot, db);
                    std::this_thread::sleep_for(std::chrono::seconds(2));
                }

                sent_today = true;
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(60));
    }
}

int main() {
    sleep(15);
    std::cout << "=== English Mentor Bot v2 ===" << std::endl;

    if (!g_config.load(".env") && !g_config.load("../.env")) {
        LOG_ERROR("Failed to load .env");
        return 1;
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

    Database db;
    db.connect();
    db.init_tables();

    GroqClient ai;
    LOG("Bot started!");

    // run scheduler thread
    std::thread scheduler(scheduler_thread, std::ref(bot), std::ref(db));
    scheduler.detach();
    LOG("Scheduler thread launched");

    int last_id = 0;
    std::map<long long, int> dict_page, learn_page;
    std::map<long long, int> dict_msg_id, learn_msg_id;
    std::map<long long, std::string> last_action;

    while (true) {
        try {
            std::string updates_str = bot.get_updates(last_id + 1, 30);
            if (updates_str.empty() || updates_str == "{}") continue;

            auto updates = json::parse(updates_str);

            for (auto& update : updates["result"]) {
                last_id = update["update_id"];

                // pagination for dictionary and learned words
                if (update.contains("callback_query")) {
                    auto callback = update["callback_query"];
                    long long chat_id = callback["message"]["chat"]["id"];
                    int message_id = callback["message"]["message_id"];
                    std::string data = callback["data"];

                    if (data == "dict_prev") {
                        auto words = db.get_user_words_full(chat_id, true);
                        int page = dict_page[chat_id] - 1;
                        dict_msg_id[chat_id] = message_id;
                        show_dictionary_page(chat_id, bot, words, page, dict_page[chat_id], last_action[chat_id], dict_msg_id[chat_id], false);
                    }
                    else if (data == "dict_next") {
                        auto words = db.get_user_words_full(chat_id, true);
                        int page = dict_page[chat_id] + 1;
                        dict_msg_id[chat_id] = message_id;
                        show_dictionary_page(chat_id, bot, words, page, dict_page[chat_id], last_action[chat_id], dict_msg_id[chat_id], false);
                    }
                    else if (data == "learn_prev") {
                        auto all_words = db.get_user_words_full(chat_id, false);
                        std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>> learned;
                        for (const auto& w : all_words) if (std::get<2>(w)) learned.push_back(w);
                        int page = learn_page[chat_id] - 1;
                        learn_msg_id[chat_id] = message_id;
                        show_learned_page(chat_id, bot, learned, page, learn_page[chat_id], last_action[chat_id], learn_msg_id[chat_id], false);
                    }
                    else if (data == "learn_next") {
                        auto all_words = db.get_user_words_full(chat_id, false);
                        std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>> learned;
                        for (const auto& w : all_words) if (std::get<2>(w)) learned.push_back(w);
                        int page = learn_page[chat_id] + 1;
                        learn_msg_id[chat_id] = message_id;
                        show_learned_page(chat_id, bot, learned, page, learn_page[chat_id], last_action[chat_id], learn_msg_id[chat_id], false);
                    }
                    else if (data.rfind("daily_", 0) == 0) {
                        int page = std::stoi(data.substr(6));
                        auto words = db.get_user_words_full(chat_id, true);
                        show_daily_review_page(chat_id, bot, words, page, message_id, false);
                    }
                    continue;
                }

                //  only process text messages


                if (!update.contains("message") || !update["message"].contains("text")) continue;

                long long chat_id = update["message"]["chat"]["id"];
                std::string text = update["message"]["text"];
                std::string name = update["message"]["from"]["first_name"];

                LOG("[" + name + "]: " + text);
                db.save_conversation(chat_id, "user", text);

                // command handling


                if (text == "/start" || text == "start") {
                    send_main_menu(chat_id, bot);
                }
                else if (text == "📚 Словарь" || text == "Словарь" || text == "словарь") {
                    auto words = db.get_user_words_full(chat_id, true);
                    dict_page[chat_id] = 0;
                    show_dictionary_page(chat_id, bot, words, 0, dict_page[chat_id], last_action[chat_id], dict_msg_id[chat_id], true);
                }
                else if (text == "✅ Выученные" || text == "Выученные" || text == "выученные") {
                    auto words = db.get_user_words_full(chat_id, false);
                    std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>> learned;
                    for (const auto& w : words) if (std::get<2>(w)) learned.push_back(w);
                    learn_page[chat_id] = 0;
                    show_learned_page(chat_id, bot, learned, 0, learn_page[chat_id], last_action[chat_id], learn_msg_id[chat_id], true);
                }
                else if (text == "➕ Новые слова" || text == "Новые слова" || text == "новые слова") {
                    send_topic_menu(chat_id, bot);
                }
                else if (text == "🤖 Спросить AI" || text == "Спросить AI" || text == "спросить ai") {
                    bot.send_message(chat_id, "🤖 *Режим AI*\n\nЗадай любой вопрос по английскому!\n\nНапиши /menu для возврата");
                }
                else if (text == "📊 Статистика" || text == "Статистика" || text == "статистика") {
                    show_stats(chat_id, bot, db);
                }
                else if (text == "/menu" || text == "🔙 Главное меню") {
                    send_main_menu(chat_id, bot);
                }
                else if (text == "🏠 Быт и дом") {
                    generate_words(chat_id, bot, db, ai, "daily life", "Быт и дом");
                    send_main_menu(chat_id, bot);
                }
                else if (text == "✈️ Путешествия") {
                    generate_words(chat_id, bot, db, ai, "travel", "Путешествия");
                    send_main_menu(chat_id, bot);
                }
                else if (text == "🍕 Еда") {
                    generate_words(chat_id, bot, db, ai, "food", "Еда");
                    send_main_menu(chat_id, bot);
                }
                else if (text == "💼 Работа") {
                    generate_words(chat_id, bot, db, ai, "business", "Работа");
                    send_main_menu(chat_id, bot);
                }
                else if (text == "💻 IT и C++") {
                    generate_words(chat_id, bot, db, ai, "IT programming", "IT и C++");
                    send_main_menu(chat_id, bot);
                }
                else if (text == "🗣️ Общение") {
                    generate_words(chat_id, bot, db, ai, "communication", "Общение");
                    send_main_menu(chat_id, bot);
                }
                else {
                    if (!try_mark_words_from_input(chat_id, text, bot, db)) {
                        bot.send_message(chat_id, "🤖 *Думаю...*");
                        std::string response = ai.ask(text);
                        bot.send_message(chat_id, format_ai_response_box(response));
                        db.save_conversation(chat_id, "assistant", response);
                    }
                }
            }
        } catch (const std::exception& e) {
            LOG_ERROR(std::string(e.what()));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return 0;
}

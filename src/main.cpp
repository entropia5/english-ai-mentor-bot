
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
#include <nlohmann/json.hpp>

using json = nlohmann::json;

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
    std::string menu_text = "📚 *Главное меню*\n\n▫️ Нажми на кнопку:";
    bot.send_message_with_keyboard(chat_id, menu_text, keyboard, true, false);
}

// send topic selection menu with KEYBOARD buttons
void send_topic_menu(long long chat_id, TelegramClient& bot) {
    std::vector<std::vector<std::string>> keyboard = {
        {"🏠 Быт и дом", "✈️ Путешествия", "🍕 Еда"},
        {"💼 Работа", "💻 IT и C++", "🗣️ Общение"},
        {"🔙 Главное меню"}
    };
    std::string menu_text = "📚 *Выбери тему для новых слов:*\n\n▫️ AI подберет 10 слов.";
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

// generate words using AI and add to database
void generate_words(long long chat_id, TelegramClient& bot, Database& db, GroqClient& ai,
                    const std::string& topic_keyword, const std::string& topic_name) {
    bot.send_message(chat_id, "🤖 *Генерирую слова на тему: " + topic_name + "*\n⏳ Подожди...");

    std::string prompt = "Generate 10 English words on topic: " + topic_keyword +
        ". Return ONLY in this exact format:\n"
        "WORD: word1\n"
        "TRANS: /transcription1/\n"
        "PRON: russian pronunciation1\n"
        "MEAN: translation1\n"
        "---\n"
        "WORD: word2\n"
        "TRANS: /transcription2/\n"
        "PRON: russian pronunciation2\n"
        "MEAN: translation2\n"
        "---\n";

    std::string response = ai.ask(prompt);
    LOG("AI Response length: " + std::to_string(response.length()));

    std::stringstream ss(response);
    std::string line, eng, transcr, pron, trans;
    int added = 0;

    while (std::getline(ss, line)) {
        if (line.empty()) continue;
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.find("WORD:") == 0) {
            eng = line.substr(5);
            eng.erase(0, eng.find_first_not_of(" \t\r\n"));
            eng.erase(eng.find_last_not_of(" \t\r\n") + 1);
        }
        else if (line.find("TRANS:") == 0) {
            transcr = line.substr(6);
            transcr.erase(0, transcr.find_first_not_of(" \t\r\n"));
            transcr.erase(transcr.find_last_not_of(" \t\r\n") + 1);
        }
        else if (line.find("PRON:") == 0) {
            pron = line.substr(5);
            pron.erase(0, pron.find_first_not_of(" \t\r\n"));
            pron.erase(pron.find_last_not_of(" \t\r\n") + 1);
        }
        else if (line.find("MEAN:") == 0) {
            trans = line.substr(5);
            trans.erase(0, trans.find_first_not_of(" \t\r\n"));
            trans.erase(trans.find_last_not_of(" \t\r\n") + 1);

            if (!eng.empty() && !trans.empty()) {
                LOG("Adding word: " + eng + " -> " + trans);
                if (db.add_word(chat_id, eng, trans, pron, topic_keyword)) {
                    added++;
                }
                eng = transcr = pron = trans = "";
            }
        }
    }

    if (added > 0) {
        bot.send_message(chat_id, "✅ *Добавлено " + std::to_string(added) + " новых слов!*\n📚 Смотри в разделе «Словарь»");
    } else {
        bot.send_message(chat_id, "❌ *Не удалось сгенерировать слова*\nПопробуй другую тему");
    }
}

// send daily review with words to repeat
void send_daily_review(long long chat_id, TelegramClient& bot, Database& db) {
    auto words = db.get_user_words_full(chat_id, true);

    if (words.empty()) {
        bot.send_message(chat_id, "🌅 *Доброе утро!*\n\n📚 У тебя нет новых слов для повторения.\n\n➕ Нажми «➕ Новые слова» чтобы добавить!");
        return;
    }

    std::string msg = "🌅 *Доброе утро!*\n\n";
    msg += "📚 *Слова для повторения сегодня:*\n\n";

    int count = 0;
    for (const auto& [eng, trans, learned, pron, transcr] : words) {
        if (count >= 10) break;
        msg += format_word(eng, trans, transcr, pron) + "\n\n";
        count++;
    }

    if (words.size() > 10) {
        msg += "\n_... и ещё " + std::to_string(words.size() - 10) + " слов в словаре_";
    }

    msg += "\n💡 *Совет:* Напиши слово, чтобы отметить его как выученное!";

    bot.send_message(chat_id, msg);
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
                    if (db.word_exists(chat_id, text)) {
                        db.mark_word_learned(chat_id, text);
                        bot.send_message(chat_id, "🎉 *Отлично!*\n\n▫️ Слово *" + text + "* отмечено как выученное!");
                    } else {
                        bot.send_message(chat_id, "🤖 *Думаю...*");
                        std::string response = ai.ask(text);
                        bot.send_message(chat_id, clean_markdown(response));
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

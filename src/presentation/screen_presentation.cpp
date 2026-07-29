#include "domain/progress.h"
#include "presentation/bot_presentation.h"
#include "presentation/bot_state.h"
#include "presentation/screen_components.h"
#include "presentation/screen_transport.h"
#include "rendering/bot_renderer.h"

bool show_status_screen(long long chat_id, TelegramClient& bot, const std::string& key,
                        const std::string& title, const std::string& subtitle,
                        const std::string& note, const InlineKeyboard& buttons, int message_id) {
    const std::string image_path = render_status_image(key, title, subtitle, note);
    if (!image_path.empty()) {
        return upsert_photo_screen(chat_id, bot, image_path, buttons, message_id,
                                   screen_caption(title));
    }

    return upsert_screen(chat_id, bot, "*" + title + "*\n\n" + subtitle + "\n\n" + note, buttons,
                         message_id);
}

void send_main_menu(long long chat_id, TelegramClient& bot, Database& database, int message_id) {
    remember_screen_context(chat_id, "main");
    const int learned = database.get_words_count(chat_id, true);
    const std::string user_level = english_level_from_learned(learned);
    const std::string image_path = render_main_menu_image(user_level);
    if (!image_path.empty()) {
        upsert_photo_screen(chat_id, bot, image_path, main_menu_keyboard(), message_id,
                            screen_caption("Главное меню"));
        return;
    }

    upsert_screen(chat_id, bot, "*Главное меню*\n\nВыбери действие:", main_menu_keyboard(),
                  message_id);
}

void send_topic_menu(long long chat_id, TelegramClient& bot, int message_id) {
    remember_screen_context(chat_id, "topics");
    const InlineKeyboard buttons = topic_keyboard();
    const std::string image_path = render_topic_menu_image();
    if (!image_path.empty()) {
        upsert_photo_screen(chat_id, bot, image_path, buttons, message_id,
                            screen_caption("Добавление слов"));
        return;
    }

    upsert_screen(chat_id, bot, "*Выбери тему для новых слов:*\n\nAI подберет 10 новых слов.",
                  buttons, message_id);
}

void show_ai_prompt(long long chat_id, TelegramClient& bot, int message_id) {
    remember_screen_context(chat_id, "ai");
    const InlineKeyboard buttons = column_keyboard({{"Главное меню", "menu_main"}});
    const std::string image_path = render_ai_prompt_image();
    if (!image_path.empty()) {
        upsert_photo_screen(chat_id, bot, image_path, buttons, message_id,
                            screen_caption("Спросить AI"));
        return;
    }

    upsert_screen(
        chat_id, bot,
        "*Режим AI*\n\nЗадай любой вопрос по английскому.\n\nДля возврата нажми кнопку ниже.",
        buttons, message_id);
}

void show_stats(long long chat_id, TelegramClient& bot, Database& database, int message_id) {
    remember_screen_context(chat_id, "stats");
    const int total =
        database.get_words_count(chat_id, false) + database.get_words_count(chat_id, true);
    const int learned = database.get_words_count(chat_id, true);

    std::string level;
    int next_level;
    std::string next_name;
    if (learned < 300) {
        level = "A1";
        next_level = 300 - learned;
        next_name = "A2";
    } else if (learned < 600) {
        level = "A2";
        next_level = 600 - learned;
        next_name = "B1";
    } else if (learned < 1000) {
        level = "B1";
        next_level = 1000 - learned;
        next_name = "B2";
    } else if (learned < 1500) {
        level = "B2";
        next_level = 1500 - learned;
        next_name = "C1";
    } else {
        level = "C1";
        next_level = 0;
        next_name = "";
    }

    const int percent = total > 0 ? learned * 100 / total : 0;
    const InlineKeyboard buttons = column_keyboard({{"Главное меню", "menu_main"}});
    const std::string image_path =
        render_stats_image(chat_id, total, learned, level, next_name, next_level, percent);
    if (!image_path.empty()) {
        upsert_photo_screen(chat_id, bot, image_path, buttons, message_id,
                            screen_caption("Статистика"));
        return;
    }

    std::string message = "*Статистика*\n\n";
    message += "Уровень: " + level + "\n";
    message += "Всего слов: " + std::to_string(total) + "\n";
    message += "Выучено: " + std::to_string(learned) + "\n";
    message += "Прогресс: " + std::to_string(percent) + "%\n\n";
    message += next_level > 0
                   ? "До " + next_name + " осталось " + std::to_string(next_level) + " слов"
                   : "Ты достиг уровня C1!";
    upsert_screen(chat_id, bot, message, buttons, message_id);
}

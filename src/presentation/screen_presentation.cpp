#include "presentation/bot_presentation.h"
#include "presentation/bot_state.h"
#include "presentation/screen_components.h"
#include "presentation/screen_transport.h"
#include "rendering/bot_renderer.h"
#include "services/course_catalog.h"

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
    const int learned = database.get_words_count(chat_id, true, "all");
    const std::string user_level = std::to_string(learned);
    const std::string image_path = render_main_menu_image(user_level);
    if (!image_path.empty()) {
        upsert_photo_screen(chat_id, bot, image_path, main_menu_keyboard(), message_id,
                            screen_caption("Главное меню"));
        return;
    }

    upsert_screen(chat_id, bot, "*Главное меню*\n\nВыберите действие:", main_menu_keyboard(),
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

    upsert_screen(chat_id, bot,
                  "*Выберите тему для новых слов:*\n\nВыберите направление. Слова идут по порядку из "
                  "учебного каталога.",
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
        "*Режим AI*\n\nЗадайте любой вопрос по английскому.\n\nДля возврата нажмите кнопку ниже.",
        buttons, message_id);
}

void show_stats(long long chat_id, TelegramClient& bot, Database& database, int message_id) {
    remember_screen_context(chat_id, "stats");
    const int total =
        database.get_words_count(chat_id, false) + database.get_words_count(chat_id, true);
    const int learned = database.get_words_count(chat_id, true);

    const auto course = database.get_active_course(chat_id);
    int catalog_size = 0;
    try {
        catalog_size = static_cast<int>(load_course_catalog(course).size());
    } catch (const std::exception&) {
    }
    const int percent = catalog_size > 0 ? learned * 100 / catalog_size : 0;
    const auto buttons =
        column_keyboard({{"Выбрать направление", "menu_new_words"}, {"Главное меню", "menu_main"}});
    const auto image_path = render_stats_image(chat_id, total, learned, course_title(course), "",
                                               catalog_size, percent);
    if (!image_path.empty()) {
        upsert_photo_screen(chat_id, bot, image_path, buttons, message_id,
                            screen_caption("Статистика"));
        return;
    }
    upsert_screen(chat_id, bot,
                  "*" + course_title(course) + "*\nДобавлено: " + std::to_string(total) +
                      "\nВыучено: " + std::to_string(learned) + " из " +
                      std::to_string(catalog_size) +
                      "\nКоличество слов не определяет уровень владения языком.",
                  buttons, message_id);
}

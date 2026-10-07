#include "bot_application_internal.h"
#include "core/text.h"
#include "database.h"
#include "groq_client.h"
#include "presentation/bot_presentation.h"
#include "presentation/bot_state.h"
#include "presentation/screen_components.h"
#include "presentation/screen_transport.h"
#include "services/broadcast_service.h"
#include "services/course_catalog.h"
#include "services/dictionary_service.h"
#include "services/dictionary_export.h"
#include "services/vocabulary_presentation.h"
#include "telegram_client.h"

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace app::detail {

void handle_callback(const nlohmann::json& update, TelegramClient& bot, Database& database,
                     GroqClient& ai, SessionState& state) {
    const auto& callback = update["callback_query"];
    const std::string callback_id = callback["id"];
    const long long chat_id = callback["message"]["chat"]["id"];
    const int clicked_message_id = callback["message"]["message_id"];
    int message_id = get_active_screen_message(chat_id);
    const std::string data = callback["data"];

    bot.answer_callback_query(callback_id);
    if (message_id <= 0) {
        message_id = clicked_message_id;
        remember_active_screen_message(chat_id, message_id,
                                       screen_message_type_from_callback(callback["message"]));
    } else if (clicked_message_id != message_id) {
        remember_obsolete_screen(chat_id, clicked_message_id);
    }
    delete_tracked_broadcast_hint(chat_id, bot);
    delete_tracked_exports(chat_id, bot);
    delete_tracked_ai_input(chat_id, bot);

    if (data == "menu_main") {
        send_main_menu(chat_id, bot, database, message_id);
    } else if (data == "menu_export_pdf") {
        show_dictionary_export_menu(chat_id, bot, message_id);
    } else if (data == "export_conversation_pdf" || data == "export_learned_pdf") {
        send_dictionary_pdf(chat_id, bot, database,
            data == "export_conversation_pdf" ? DictionaryExportKind::Conversation
                                               : DictionaryExportKind::Learned);
    } else if (data == "menu_dictionary" || data == "menu_learned") {
        show_user_dictionary(chat_id, bot, database, data == "menu_learned", 0, message_id);
    } else if (data.rfind("dict_filter_", 0) == 0 || data.rfind("learn_filter_", 0) == 0) {
        const bool learned = data.rfind("learn_", 0) == 0;
        const auto filter = data.substr(learned ? 13 : 12);
        if (database.set_dictionary_filter(chat_id, filter))
            show_user_dictionary(chat_id, bot, database, learned, 0, message_id);
    } else if (data == "menu_reminders") {
        show_reminder_settings(chat_id, bot, database, message_id);
    } else if (data.rfind("rem_", 0) == 0) {
        for (const std::string key : {"morning_enabled", "evening_enabled", "morning_course",
                                      "evening_add_new"}) {
            const std::string prefix = "rem_" + key + "_";
            if (data.rfind(prefix, 0) == 0 &&
                database.set_reminder_setting(chat_id, key, data.substr(prefix.size()))) {
                show_reminder_settings(chat_id, bot, database, message_id);
                break;
            }
        }
    } else if (data == "menu_new_words") {
        send_topic_menu(chat_id, bot, message_id);
    } else if (data == "menu_ai") {
        show_ai_prompt(chat_id, bot, message_id);
    } else if (data == "menu_stats") {
        show_stats(chat_id, bot, database, message_id);
    } else if (data.rfind("course_", 0) == 0) {
        const auto course = canonical_course(data.substr(7));
        if (!course.empty() && database.set_active_course(chat_id, course)) {
            remember_screen_context(chat_id, "topics");
            show_status_screen(chat_id, bot, "course_" + course, course_title(course),
                               "Добавляйте слова по порядку и используйте их в своих предложениях.",
                               "Произнесите пример вслух и составьте свою фразу. Напоминания "
                               "настраиваются отдельно в главном меню.",
                               column_keyboard({{"Добавить 5 новых слов", "add_" + course},
                                                {"Другие направления", "menu_new_words"},
                                                {"Главное меню", "menu_main"}}),
                               message_id);
        }
    } else if (data.rfind("add_", 0) == 0) {
        const auto course = canonical_course(data.substr(4));
        if (!course.empty())
            generate_words(chat_id, bot, database, ai, course, course_title(course), message_id);
    } else if (data == "practice_course") {
        send_main_menu(chat_id, bot, database, message_id);
    } else if (data.rfind("topic_", 0) == 0) {
        // Old Telegram messages may still contain the retired topic buttons.
        send_topic_menu(chat_id, bot, message_id);
    } else if (data.rfind("dict_prev_", 0) == 0 || data.rfind("dict_next_", 0) == 0 ||
               data.rfind("dict_info_", 0) == 0) {
        const int page = std::stoi(data.substr(data.find_last_of('_') + 1));
        show_user_dictionary(chat_id, bot, database, false, page, message_id);
    } else if (data.rfind("learn_prev_", 0) == 0 || data.rfind("learn_next_", 0) == 0 ||
               data.rfind("learn_info_", 0) == 0) {
        const int page = std::stoi(data.substr(data.find_last_of('_') + 1));
        show_user_dictionary(chat_id, bot, database, true, page, message_id);
    } else if (data.rfind("daily_prev_", 0) == 0 || data.rfind("daily_next_", 0) == 0 ||
               data.rfind("daily_info_", 0) == 0) {
        const std::size_t last_underscore = data.find_last_of('_');
        const int page = std::stoi(data.substr(last_underscore + 1));
        const auto filter = database.get_reminder_settings(chat_id).morning_course;
        const auto words = get_learned_words_for_review(chat_id, database, filter);
        show_daily_review_page(chat_id, bot, words, page, message_id, false,
                               &state.last_action[chat_id], filter);
    } else if (data.rfind("evening_prev_", 0) == 0 || data.rfind("evening_next_", 0) == 0 ||
               data.rfind("evening_info_", 0) == 0) {
        const std::size_t last_underscore = data.find_last_of('_');
        const int page = std::stoi(data.substr(last_underscore + 1));
        const auto filter = database.get_reminder_settings(chat_id).evening_course;
        const auto words = database.get_user_words_full(chat_id, true, filter);
        show_evening_words_page(chat_id, bot, words, page, message_id, false,
                                &state.last_action[chat_id], filter);
    }
}

} // namespace app::detail

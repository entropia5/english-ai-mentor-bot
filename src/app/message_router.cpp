#include "bot_application_internal.h"
#include "core/text.h"
#include "database.h"
#include "groq_client.h"
#include "logger.h"
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
#include "user_config.h"

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace {

bool is_primary_user(long long chat_id, long long sender_user_id) {
    const long long primary_user_id = configured_user_id("USER_1_ID");
    return primary_user_id > 0 && (chat_id == primary_user_id || sender_user_id == primary_user_id);
}

} // namespace

namespace app::detail {

void handle_text_message(const nlohmann::json& update, TelegramClient& bot, Database& database,
                         GroqClient& ai, SessionState& state) {
    const long long chat_id = update["message"]["chat"]["id"];
    const long long sender_user_id = update["message"]["from"].value("id", 0LL);
    const int incoming_message_id = update["message"]["message_id"];
    const std::string text = update["message"]["text"];
    const std::string name = update["message"]["from"]["first_name"];

    LOG("[" + name + "]: " + text);
    database.add_user(chat_id, name);
    ensure_reply_keyboard_removed(chat_id, bot);
    delete_tracked_ai_input(chat_id, bot, incoming_message_id);
    delete_tracked_broadcast_hint(chat_id, bot);
    delete_tracked_exports(chat_id, bot);

    bool delete_incoming_after_handled = false;
    const std::string normalized_text = to_lower_ascii(trim(text));

    if (text == "/start" || text == "start") {
        // Clearing Telegram history can hide a message while edits still succeed.
        // Start must send a fresh screen, preserving the old one if sending fails.
        const int previous_id = get_active_screen_message(chat_id);
        const auto previous_type = get_active_screen_message_type(chat_id);
        remember_obsolete_screen(chat_id, previous_id);
        clear_active_screen_message(chat_id);
        send_main_menu(chat_id, bot, database);
        if (get_active_screen_message(chat_id) > 0) {
            delete_incoming_after_handled = true;
        } else {
            remember_active_screen_message(chat_id, previous_id, previous_type);
        }
    } else if (normalized_text == "testing" || normalized_text == "/testing") {
        if (is_primary_user(chat_id, sender_user_id)) {
            send_daily_review(chat_id, bot, database);
        } else {
            LOG("testing command ignored for unauthorized chat " + std::to_string(chat_id) +
                ", sender " + std::to_string(sender_user_id));
        }
        delete_incoming_after_handled = true;
    } else if (normalized_text == "testing_evening" || normalized_text == "/testing_evening" ||
               normalized_text == "testing evening") {
        if (is_primary_user(chat_id, sender_user_id)) {
            send_evening_new_words(chat_id, bot, database);
        } else {
            LOG("testing_evening command ignored for unauthorized chat " + std::to_string(chat_id) +
                ", sender " + std::to_string(sender_user_id));
        }
        delete_incoming_after_handled = true;
    } else if (text == "Слова, которые я ещё учу" || normalized_text == "слова, которые я ещё учу" || text == "Словарь для изучения" || text == "словарь для изучения" ||
               text == "📚 Словарь" || text == "Словарь" || text == "словарь") {
        show_user_dictionary(chat_id, bot, database, false, 0, 0, true);
        delete_incoming_after_handled = true;
    } else if (text == "Словарь выученных слов" || normalized_text == "словарь выученных слов" || text == "Словарь для повторения" || text == "словарь для повторения" ||
               text == "✅ Выученные" || text == "Выученные" || text == "выученные") {
        show_user_dictionary(chat_id, bot, database, true, 0, 0, true);
        delete_incoming_after_handled = true;
    } else if (text == "Добавить новые слова" || normalized_text == "добавить новые слова" || text == "Добавить слова" || text == "добавить слова" || text == "➕ Новые слова" ||
               text == "Новые слова" || text == "новые слова") {
        send_topic_menu(chat_id, bot);
        delete_incoming_after_handled = true;
    } else if (text == "🤖 Спросить AI" || text == "Спросить AI" ||
               text == "спросить ai") {
        send_main_menu(chat_id, bot, database);
        delete_incoming_after_handled = true;
    } else if (text == "📊 Статистика" || text == "Статистика" || text == "статистика") {
        show_stats(chat_id, bot, database);
        delete_incoming_after_handled = true;
    } else if (text == "Скачать словарь в PDF") {
        show_dictionary_export_menu(chat_id, bot);
        delete_incoming_after_handled = true;
    } else if (text == "Напоминания" || text == "напоминания") {
        show_reminder_settings(chat_id, bot, database);
        delete_incoming_after_handled = true;
    } else if (text == "/menu" || text == "🔙 Главное меню" || text == "Главное меню") {
        send_main_menu(chat_id, bot, database);
        delete_incoming_after_handled = true;
    } else if (text == "Разговорный английский" || text == "Медицинский английский" ||
               text == "IT") {
        const std::string course =
            text == "IT" ? "it" : (text == "Медицинский английский" ? "medicine" : "conversation");
        generate_words(chat_id, bot, database, ai, course, text);
        delete_incoming_after_handled = true;
    } else if (get_screen_context(chat_id) != "ai" &&
               try_mark_words_from_input(chat_id, text, database)) {
        bot.delete_message(chat_id, incoming_message_id);
        int confirmation_message_id = 0;
        bot.send_message(chat_id,
                         "Готово ✅",
                         "", &confirmation_message_id);
        remember_broadcast_hint(chat_id, confirmation_message_id);
        delete_messages_after_delay(bot, chat_id, {confirmation_message_id}, 1);
        if (has_tracked_reminder(chat_id)) {
            show_user_dictionary(chat_id, bot, database, false, 0);
        } else refresh_after_marking_words(chat_id, bot, database, state.dictionary_page[chat_id],
                                    state.dictionary_message_id[chat_id],
                                    state.learned_page[chat_id], state.learned_message_id[chat_id],
                                    state.last_action[chat_id]);
    } else if (get_screen_context(chat_id) == "ai") {
        remember_ai_input(chat_id, incoming_message_id);
        upsert_screen(chat_id, bot, "*Думаю...*", column_keyboard({{"Главное меню", "menu_main"}}));
        std::string prompt = "Продолжи учебный диалог. Направление: " +
                             course_title(database.get_active_course(chat_id)) +
                             ". Последнее сообщение пользователя в конце.\n";
        const auto history = database.get_conversation_history(chat_id, 6);
        for (const auto& entry : history)
            prompt += entry.first + ": " + entry.second + "\n";
        prompt += "user: " + text;
        database.save_conversation(chat_id, "user", text);
        const std::string response = ai.ask(prompt);
        upsert_screen(chat_id, bot, format_ai_response_box(response),
                      column_keyboard({{"Главное меню", "menu_main"}}));
        database.save_conversation(chat_id, "assistant", response);
        delete_tracked_ai_input(chat_id, bot);
    } else {
        int hint_id = 0;
        bot.send_message(chat_id,
                         "Не нашёл слова в выбранном словаре. Отправьте слова из него через "
                         "пробел, запятую или с новой строки. Для вопроса помощнику нажмите «Спросить AI».",
                         "", &hint_id);
        remember_broadcast_hint(chat_id, hint_id);
    }

    if (delete_incoming_after_handled) {
        bot.delete_message(chat_id, incoming_message_id);
    }
}

} // namespace app::detail

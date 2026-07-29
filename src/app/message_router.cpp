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
#include "services/dictionary_service.h"
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
    database.save_conversation(chat_id, "user", text);
    ensure_reply_keyboard_removed(chat_id, bot);
    delete_tracked_ai_input(chat_id, bot, incoming_message_id);
    delete_tracked_broadcast_hint(chat_id, bot);

    bool delete_incoming_after_handled = false;
    const std::string normalized_text = to_lower_ascii(trim(text));

    if (text == "/start" || text == "start") {
        delete_active_screen_message(chat_id, bot);
        send_main_menu(chat_id, bot, database);
        delete_incoming_after_handled = true;
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
    } else if (text == "Словарь для изучения" || text == "словарь для изучения" ||
               text == "📚 Словарь" || text == "Словарь" || text == "словарь") {
        const auto words = database.get_user_words_full(chat_id, true);
        state.dictionary_page[chat_id] = 0;
        show_dictionary_page(chat_id, bot, words, 0, state.dictionary_page[chat_id],
                             state.last_action[chat_id], state.dictionary_message_id[chat_id],
                             true);
        delete_incoming_after_handled = true;
    } else if (text == "Словарь для повторения" || text == "словарь для повторения" ||
               text == "✅ Выученные" || text == "Выученные" || text == "выученные") {
        const auto words = database.get_user_words_full(chat_id, false);
        std::vector<WordView> learned;
        for (const auto& word : words) {
            if (word.learned) {
                learned.push_back(word);
            }
        }
        state.learned_page[chat_id] = 0;
        show_learned_page(chat_id, bot, learned, 0, state.learned_page[chat_id],
                          state.last_action[chat_id], state.learned_message_id[chat_id], true);
        delete_incoming_after_handled = true;
    } else if (text == "Добавить слова" || text == "добавить слова" || text == "➕ Новые слова" ||
               text == "Новые слова" || text == "новые слова") {
        send_topic_menu(chat_id, bot);
        delete_incoming_after_handled = true;
    } else if (text == "🤖 Спросить AI" || text == "Спросить AI" ||
               text == "спросить ai") {
        show_ai_prompt(chat_id, bot);
        delete_incoming_after_handled = true;
    } else if (text == "📊 Статистика" || text == "Статистика" || text == "статистика") {
        show_stats(chat_id, bot, database);
        delete_incoming_after_handled = true;
    } else if (text == "/menu" || text == "🔙 Главное меню" || text == "Главное меню") {
        send_main_menu(chat_id, bot, database);
        delete_incoming_after_handled = true;
    } else if (text == "🏠 Быт и дом" || text == "Быт и дом") {
        generate_words(chat_id, bot, database, ai, "daily life", "Быт и дом");
        delete_incoming_after_handled = true;
    } else if (text == "✈️ Путешествия" || text == "Путешествия") {
        generate_words(chat_id, bot, database, ai, "travel", "Путешествия");
        delete_incoming_after_handled = true;
    } else if (text == "🍕 Еда" || text == "Еда") {
        generate_words(chat_id, bot, database, ai, "food", "Еда");
        delete_incoming_after_handled = true;
    } else if (text == "💼 Работа" || text == "Работа") {
        generate_words(chat_id, bot, database, ai, "business", "Работа");
        delete_incoming_after_handled = true;
    } else if (text == "💻 IT и C++" || text == "IT и C++") {
        generate_words(chat_id, bot, database, ai, "IT programming", "IT и C++");
        delete_incoming_after_handled = true;
    } else if (text == "🗣️ Общение" || text == "Общение") {
        generate_words(chat_id, bot, database, ai, "communication", "Общение");
        delete_incoming_after_handled = true;
    } else if (try_mark_words_from_input(chat_id, text, database)) {
        refresh_after_marking_words(chat_id, bot, database, state.dictionary_page[chat_id],
                                    state.dictionary_message_id[chat_id],
                                    state.learned_page[chat_id], state.learned_message_id[chat_id],
                                    state.last_action[chat_id]);
        int confirmation_message_id = 0;
        bot.send_message(chat_id,
                         "Готово.\n\n Новое слово внесено в Словарь для повторения "
                         "и было отмечено, как выученное.",
                         "", &confirmation_message_id);
        remember_broadcast_hint(chat_id, confirmation_message_id);
        delete_messages_after_delay(bot, chat_id, {incoming_message_id, confirmation_message_id},
                                    10);
    } else {
        remember_ai_input(chat_id, incoming_message_id);
        upsert_screen(chat_id, bot, "*Думаю...*", column_keyboard({{"Главное меню", "menu_main"}}));
        const std::string response = ai.ask(text);
        upsert_screen(chat_id, bot, format_ai_response_box(response),
                      column_keyboard({{"Главное меню", "menu_main"}}));
        database.save_conversation(chat_id, "assistant", response);
        delete_tracked_ai_input(chat_id, bot);
    }

    if (delete_incoming_after_handled) {
        bot.delete_message(chat_id, incoming_message_id);
    }
}

} // namespace app::detail

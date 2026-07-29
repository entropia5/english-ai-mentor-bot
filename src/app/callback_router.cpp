#include "bot_application_internal.h"
#include "database.h"
#include "presentation/bot_presentation.h"
#include "presentation/bot_state.h"
#include "services/broadcast_service.h"
#include "services/dictionary_service.h"
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
    const int message_id = callback["message"]["message_id"];
    const std::string data = callback["data"];

    bot.answer_callback_query(callback_id);
    remember_active_screen_message(chat_id, message_id,
                                   screen_message_type_from_callback(callback["message"]));
    delete_tracked_broadcast_hint(chat_id, bot);
    delete_tracked_ai_input(chat_id, bot);

    if (data == "menu_main") {
        send_main_menu(chat_id, bot, database, message_id);
    } else if (data == "menu_dictionary") {
        const auto words = database.get_user_words_full(chat_id, true);
        state.dictionary_page[chat_id] = 0;
        state.dictionary_message_id[chat_id] = message_id;
        show_dictionary_page(chat_id, bot, words, 0, state.dictionary_page[chat_id],
                             state.last_action[chat_id], state.dictionary_message_id[chat_id],
                             false);
    } else if (data == "menu_learned") {
        const auto words = database.get_user_words_full(chat_id, false);
        std::vector<WordView> learned;
        for (const auto& word : words) {
            if (word.learned) {
                learned.push_back(word);
            }
        }
        state.learned_page[chat_id] = 0;
        state.learned_message_id[chat_id] = message_id;
        show_learned_page(chat_id, bot, learned, 0, state.learned_page[chat_id],
                          state.last_action[chat_id], state.learned_message_id[chat_id], false);
    } else if (data == "menu_new_words") {
        send_topic_menu(chat_id, bot, message_id);
    } else if (data == "menu_ai") {
        show_ai_prompt(chat_id, bot, message_id);
    } else if (data == "menu_stats") {
        show_stats(chat_id, bot, database, message_id);
    } else if (data == "topic_daily_life") {
        generate_words(chat_id, bot, database, ai, "daily life", "Быт и дом", message_id);
    } else if (data == "topic_travel") {
        generate_words(chat_id, bot, database, ai, "travel", "Путешествия", message_id);
    } else if (data == "topic_food") {
        generate_words(chat_id, bot, database, ai, "food", "Еда", message_id);
    } else if (data == "topic_business") {
        generate_words(chat_id, bot, database, ai, "business", "Работа", message_id);
    } else if (data == "topic_it_cpp") {
        generate_words(chat_id, bot, database, ai, "IT programming", "IT и C++", message_id);
    } else if (data == "topic_communication") {
        generate_words(chat_id, bot, database, ai, "communication", "Общение", message_id);
    } else if (data.rfind("dict_prev_", 0) == 0 || data.rfind("dict_next_", 0) == 0 ||
               data.rfind("dict_info_", 0) == 0) {
        const auto words = database.get_user_words_full(chat_id, true);
        const std::size_t last_underscore = data.find_last_of('_');
        const int page = std::stoi(data.substr(last_underscore + 1));
        state.dictionary_message_id[chat_id] = message_id;
        show_dictionary_page(chat_id, bot, words, page, state.dictionary_page[chat_id],
                             state.last_action[chat_id], state.dictionary_message_id[chat_id],
                             false);
    } else if (data.rfind("learn_prev_", 0) == 0 || data.rfind("learn_next_", 0) == 0 ||
               data.rfind("learn_info_", 0) == 0) {
        const auto all_words = database.get_user_words_full(chat_id, false);
        std::vector<WordView> learned;
        for (const auto& word : all_words) {
            if (word.learned) {
                learned.push_back(word);
            }
        }
        const std::size_t last_underscore = data.find_last_of('_');
        const int page = std::stoi(data.substr(last_underscore + 1));
        state.learned_message_id[chat_id] = message_id;
        show_learned_page(chat_id, bot, learned, page, state.learned_page[chat_id],
                          state.last_action[chat_id], state.learned_message_id[chat_id], false);
    } else if (data.rfind("daily_prev_", 0) == 0 || data.rfind("daily_next_", 0) == 0 ||
               data.rfind("daily_info_", 0) == 0) {
        const std::size_t last_underscore = data.find_last_of('_');
        const int page = std::stoi(data.substr(last_underscore + 1));
        const auto words = get_learned_words_for_review(chat_id, database);
        show_daily_review_page(chat_id, bot, words, page, message_id, false,
                               &state.last_action[chat_id]);
    } else if (data.rfind("evening_prev_", 0) == 0 || data.rfind("evening_next_", 0) == 0 ||
               data.rfind("evening_info_", 0) == 0) {
        const std::size_t last_underscore = data.find_last_of('_');
        const int page = std::stoi(data.substr(last_underscore + 1));
        const auto words = database.get_user_words_full(chat_id, true);
        show_evening_words_page(chat_id, bot, words, page, message_id, false,
                                &state.last_action[chat_id]);
    }
}

} // namespace app::detail

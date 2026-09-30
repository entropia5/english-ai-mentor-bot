#include "services/dictionary_service.h"

#include "core/text.h"
#include "presentation/bot_presentation.h"
#include "presentation/bot_state.h"

bool try_mark_words_from_input(long long chat_id, const std::string& text, Database& db) {
    std::string filter = db.get_dictionary_filter(chat_id);
    const auto context = get_screen_context(chat_id);
    if (context == "daily")
        filter = db.get_reminder_settings(chat_id).morning_course;
    if (context == "evening")
        filter = db.get_reminder_settings(chat_id).evening_course;
    std::vector<std::string> dictionary;
    for (const auto& word : db.get_user_words_full(chat_id, false, filter))
        dictionary.push_back(word.english);
    const auto requested_words = match_words_input(text, dictionary);
    bool marked = false;
    for (const auto& word : requested_words)
        if (db.mark_word_learned(chat_id, word, filter)) marked = true;
    return marked;
}

std::vector<WordView> get_learned_words_for_review(long long chat_id, Database& db,
                                                   const std::string& filter) {
    const auto all_words = db.get_user_words_full(chat_id, false, filter);
    std::vector<WordView> learned;
    for (const auto& word : all_words) {
        if (word.learned) {
            learned.push_back(word);
        }
    }
    return learned;
}

void refresh_after_marking_words(long long chat_id, TelegramClient& bot, Database& db,
                                 int& dict_current_page, int& dict_message_id,
                                 int& learn_current_page, int& learn_message_id,
                                 std::string& current_action) {
    std::string persisted_context = get_screen_context(chat_id);
    std::string context =
        is_persistable_screen_context(persisted_context) ? persisted_context : current_action;
    current_action = context;

    if (context == "daily") {
        const auto filter = db.get_reminder_settings(chat_id).morning_course;
        const auto words = get_learned_words_for_review(chat_id, db, filter);
        show_daily_review_page(chat_id, bot, words, 0, get_active_screen_message(chat_id), false,
                               &current_action, filter);
        return;
    }
    if (context == "evening") {
        const auto filter = db.get_reminder_settings(chat_id).evening_course;
        const auto words = db.get_user_words_full(chat_id, true, filter);
        show_evening_words_page(chat_id, bot, words, 0, get_active_screen_message(chat_id), false,
                                &current_action, filter);
        return;
    }
    dict_current_page = 0;
    learn_current_page = 0;
    dict_message_id = get_active_screen_message(chat_id);
    learn_message_id = dict_message_id;
    show_user_dictionary(chat_id, bot, db, context == "learned", 0, dict_message_id);
}

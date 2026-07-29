#include "services/dictionary_service.h"

#include "core/text.h"
#include "presentation/bot_presentation.h"
#include "presentation/bot_state.h"

bool try_mark_words_from_input(long long chat_id, const std::string& text, Database& db) {
    auto requested_words = split_words_input(text);
    if (requested_words.empty())
        return false;

    std::vector<std::string> marked_words;
    std::vector<std::string> not_found_words;

    for (const auto& word : requested_words) {
        if (db.word_exists(chat_id, word)) {
            if (db.mark_word_learned(chat_id, word)) {
                marked_words.push_back(word);
            }
        } else if (requested_words.size() > 1) {
            not_found_words.push_back(word);
        }
    }

    if (marked_words.empty())
        return false;

    return true;
}

std::vector<WordView> get_learned_words_for_review(long long chat_id, Database& db) {
    const auto all_words = db.get_user_words_full(chat_id, false);
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
        auto words = get_learned_words_for_review(chat_id, db);
        show_daily_review_page(chat_id, bot, words, 0, get_active_screen_message(chat_id), false,
                               &current_action);
        return;
    }

    if (context == "evening") {
        auto words = db.get_user_words_full(chat_id, true);
        show_evening_words_page(chat_id, bot, words, 0, get_active_screen_message(chat_id), false,
                                &current_action);
        return;
    }

    if (context == "learned") {
        auto all_words = db.get_user_words_full(chat_id, false);
        std::vector<WordView> learned;
        for (const auto& word : all_words) {
            if (word.learned) {
                learned.push_back(word);
            }
        }
        learn_message_id = get_active_screen_message(chat_id);
        show_learned_page(chat_id, bot, learned, learn_current_page, learn_current_page,
                          current_action, learn_message_id, false);
        return;
    }

    auto words = db.get_user_words_full(chat_id, true);
    int page = context == "dictionary" ? dict_current_page : 0;
    dict_message_id = get_active_screen_message(chat_id);
    show_dictionary_page(chat_id, bot, words, page, dict_current_page, current_action,
                         dict_message_id, false);
}

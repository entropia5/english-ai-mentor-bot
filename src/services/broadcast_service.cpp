#include "services/broadcast_service.h"

#include "config.h"
#include "core/text.h"
#include "database.h"
#include "groq_client.h"
#include "logger.h"
#include "presentation/bot_presentation.h"
#include "presentation/bot_state.h"
#include "presentation/screen_components.h"
#include "services/dictionary_service.h"
#include "services/vocabulary_service.h"
#include "user_config.h"

#include <string>
#include <vector>

std::vector<long long> configured_broadcast_users() {
    std::vector<long long> users;

    const std::string allowed_users = trim(g_config.get("ALLOWED_USERS", ""));
    if (!allowed_users.empty()) {
        for (const auto& id_text : split_words_input(allowed_users)) {
            try {
                users.push_back(std::stoll(id_text));
            } catch (const std::exception& error) {
                LOG_ERROR("Invalid ALLOWED_USERS id: " + id_text + "; " + error.what());
            }
        }
    }

    if (users.empty()) {
        for (int index = 1; index <= 10; ++index) {
            const long long user_id = configured_user_id("USER_" + std::to_string(index) + "_ID");
            if (user_id > 0) {
                users.push_back(user_id);
            }
        }
    }
    return users;
}

BroadcastResult send_daily_review(long long chat_id, TelegramClient& bot, Database& database) {
    delete_tracked_broadcast_hint(chat_id, bot);

    const auto words = get_learned_words_for_review(chat_id, database);
    if (!show_daily_review_page(chat_id, bot, words, 0)) {
        LOG_ERROR("Morning review screen delivery failed for " + std::to_string(chat_id));
        return BroadcastResult::Failed;
    }
    if (words.empty()) {
        LOG("Morning review skipped for " + std::to_string(chat_id) + ": no learned words");
        return BroadcastResult::NoContent;
    }
    return BroadcastResult::Delivered;
}

BroadcastResult send_evening_new_words(long long chat_id, TelegramClient& bot, Database& database) {
    delete_tracked_broadcast_hint(chat_id, bot);
    remember_screen_context(chat_id, "generation");

    const bool status_ok =
        show_status_screen(chat_id, bot, "evening_generation", "Вечерняя подборка",
                           "Генерирую новые слова для изучения.",
                           "AI проверяет дубли, отсеивает слабые варианты и готовит произношение.",
                           column_keyboard({{"Главное меню", "menu_main"}}));
    if (!status_ok) {
        LOG_WARNING("Evening generation status screen delivery failed for " +
                    std::to_string(chat_id));
    }

    GroqClient ai;
    const WordGenerationResult generation = add_generated_words_to_db(
        chat_id, database, ai,
        "mixed practical English for daily life, work, travel, food, IT and communication", 10);
    LOG("Evening words generated for " + std::to_string(chat_id) + ": added " +
        std::to_string(generation.added) + ", duplicates " + std::to_string(generation.duplicates) +
        ", response duplicates " + std::to_string(generation.duplicate_in_response) +
        ", rejected quality " + std::to_string(generation.rejected_quality) + ", fallback added " +
        std::to_string(generation.fallback_added));

    const auto words = database.get_user_words_full(chat_id, true);
    if (!show_evening_words_page(chat_id, bot, words, 0)) {
        LOG_ERROR("Evening words screen delivery failed for " + std::to_string(chat_id));
        return BroadcastResult::Failed;
    }
    if (words.empty()) {
        LOG("Evening words skipped for " + std::to_string(chat_id) + ": no words to show");
        return BroadcastResult::NoContent;
    }
    return generation.added > 0 ? BroadcastResult::Delivered : BroadcastResult::NoContent;
}

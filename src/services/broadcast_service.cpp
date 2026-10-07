#include "services/broadcast_service.h"

#include "config.h"
#include "core/text.h"
#include "database.h"
#include "groq_client.h"
#include "logger.h"
#include "presentation/bot_presentation.h"
#include "presentation/bot_state.h"
#include "presentation/screen_components.h"
#include "services/course_catalog.h"
#include "services/dictionary_service.h"
#include "services/vocabulary_service.h"
#include "user_config.h"

#include "storage/broadcast_run_state.h"

#include <ctime>
#include <stdexcept>
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

ReminderContent prepare_morning_review(long long chat_id, Database& database) {
    ReminderContent result;
    try {
        const auto settings = database.get_reminder_settings(chat_id);
        result.enabled = settings.morning_enabled;
        result.filter = settings.morning_course;
        if (!result.enabled)
            return result;
        result.words = get_learned_words_for_review(chat_id, database, result.filter);
    } catch (const std::exception& e) {
        LOG_ERROR("Morning reminder preparation failed: " + std::string(e.what()));
        result.failed = true;
    }
    return result;
}

ReminderContent prepare_evening_review(long long chat_id, Database& database) {
    ReminderContent result;
    try {
        const auto settings = database.get_reminder_settings(chat_id);
        result.enabled = settings.evening_enabled;
        result.filter = settings.evening_course;
        if (!result.enabled)
            return result;
        result.new_words = settings.evening_add_new;
        if (settings.evening_add_new) {
            const auto now = std::time(nullptr);
            std::tm local{};
            if (localtime_r(&now, &local) == nullptr)
                throw std::runtime_error("Cannot read evening date");
            result.words = database.prepare_evening_batch(chat_id, result.filter,
                                                          local_date_key(local));
        } else {
            result.words = database.get_user_words_full(chat_id, true, result.filter);
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Evening reminder preparation failed: " + std::string(e.what()));
        result.failed = true;
    }
    return result;
}

BroadcastResult send_daily_review(long long chat_id, TelegramClient& bot, Database& database) {
    const auto content = prepare_morning_review(chat_id, database);
    if (content.failed)
        return BroadcastResult::Failed;
    if (!content.enabled)
        return BroadcastResult::Disabled;
    if (content.words.empty())
        return BroadcastResult::NoContent;
    delete_tracked_broadcast_hint(chat_id, bot);
    const bool delivered = show_daily_review_page(chat_id, bot, content.words, 0, 0, true, nullptr, content.filter);
    if (delivered) {
        finish_reminder_transition(chat_id, bot, get_active_screen_message(chat_id), true);
        delete_tracked_exports(chat_id, bot);
    }
    return delivered ? BroadcastResult::Delivered : BroadcastResult::Failed;
}

BroadcastResult send_evening_new_words(long long chat_id, TelegramClient& bot, Database& database) {
    const auto content = prepare_evening_review(chat_id, database);
    if (content.failed)
        return BroadcastResult::Failed;
    if (!content.enabled)
        return BroadcastResult::Disabled;
    if (content.words.empty())
        return BroadcastResult::NoContent;
    delete_tracked_broadcast_hint(chat_id, bot);
    const bool delivered = show_evening_words_page(chat_id, bot, content.words, 0, 0, true, nullptr, content.filter,
                                   content.new_words);
    if (delivered) {
        finish_reminder_transition(chat_id, bot, get_active_screen_message(chat_id), true);
        delete_tracked_exports(chat_id, bot);
    }
    return delivered ? BroadcastResult::Delivered : BroadcastResult::Failed;
}

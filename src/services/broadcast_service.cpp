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

#include <set>
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
        if (settings.evening_add_new) {
            const std::vector<std::string> courses =
                result.filter == "all" ? std::vector<std::string>{"conversation", "medicine", "it"}
                                       : std::vector<std::string>{result.filter};
            std::vector<std::vector<CourseWord>> catalogs;
            std::vector<int> available;
            std::vector<int> allocations(courses.size(), 0);
            for (const auto& course : courses) {
                catalogs.push_back(load_course_catalog(course));
                std::set<std::string> existing;
                for (const auto& word : database.get_user_words_full(chat_id, false, course))
                    existing.insert(to_lower_ascii(trim(word.english)));
                available.push_back(
                    static_cast<int>(select_course_words(catalogs.back(), existing, 5).size()));
            }
            // 'All' adds at most five words total, distributed across non-exhausted courses.
            int remaining = 5;
            while (remaining > 0) {
                bool found = false;
                for (std::size_t i = 0; i < courses.size() && remaining > 0; ++i) {
                    if (allocations[i] >= available[i])
                        continue;
                    ++allocations[i];
                    --remaining;
                    found = true;
                }
                if (!found)
                    break;
            }
            for (std::size_t i = 0; i < courses.size(); ++i) {
                if (allocations[i] > 0 &&
                    database.add_course_words(chat_id, courses[i], catalogs[i], allocations[i]) < 0)
                    throw std::runtime_error("Could not add evening words");
            }
        }
        result.words = database.get_user_words_full(chat_id, true, result.filter);
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
    return show_daily_review_page(chat_id, bot, content.words, 0, 0, true, nullptr, content.filter)
               ? BroadcastResult::Delivered
               : BroadcastResult::Failed;
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
    return show_evening_words_page(chat_id, bot, content.words, 0, 0, true, nullptr, content.filter)
               ? BroadcastResult::Delivered
               : BroadcastResult::Failed;
}

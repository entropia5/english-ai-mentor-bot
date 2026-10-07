#include "bot_state_internal.h"
#include "presentation/bot_state.h"

#include <algorithm>
#include <chrono>
#include <mutex>
#include <utility>
#include <vector>

void delete_tracked_ai_input(long long chat_id, TelegramClient& bot, int except_message_id) {
    int message_id = 0;
    {
        std::lock_guard<std::mutex> lock(g_bot_state.mutex);
        const auto entry = g_bot_state.last_ai_user_messages.find(chat_id);
        if (entry == g_bot_state.last_ai_user_messages.end() ||
            entry->second == except_message_id) {
            return;
        }
        message_id = entry->second;
        g_bot_state.last_ai_user_messages.erase(entry);
    }
    bot.delete_message(chat_id, message_id);
}

void delete_tracked_broadcast_hint(long long chat_id, TelegramClient& bot) {
    int message_id = 0;
    {
        std::lock_guard<std::mutex> lock(g_bot_state.mutex);
        const auto entry = g_bot_state.broadcast_hint_messages.find(chat_id);
        if (entry == g_bot_state.broadcast_hint_messages.end()) {
            return;
        }
        message_id = entry->second;
        g_bot_state.broadcast_hint_messages.erase(entry);
        save_bot_state_locked();
    }
    bot.delete_message(chat_id, message_id);
}

void remember_ai_input(long long chat_id, int message_id) {
    if (message_id <= 0) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_bot_state.mutex);
    g_bot_state.last_ai_user_messages[chat_id] = message_id;
}

void remember_broadcast_hint(long long chat_id, int message_id) {
    if (message_id <= 0) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_bot_state.mutex);
    g_bot_state.broadcast_hint_messages[chat_id] = message_id;
    save_bot_state_locked();
}

void delete_messages_after_delay(TelegramClient& bot, long long chat_id,
                                 std::vector<int> message_ids, int delay_seconds) {
    (void)bot;
    std::lock_guard<std::mutex> lock(g_bot_state.mutex);
    g_bot_state.deferred_deletions.push_back(
        {std::chrono::steady_clock::now() + std::chrono::seconds(delay_seconds), chat_id,
         std::move(message_ids)});
}

void process_deferred_message_deletions(TelegramClient& bot) {
    struct DueMessage {
        long long chat_id;
        int message_id;
    };

    std::vector<DueMessage> due_messages;
    const auto now = std::chrono::steady_clock::now();
    {
        std::lock_guard<std::mutex> lock(g_bot_state.mutex);
        auto task = g_bot_state.deferred_deletions.begin();
        while (task != g_bot_state.deferred_deletions.end()) {
            if (task->due_at > now) {
                ++task;
                continue;
            }
            for (const int message_id : task->message_ids) {
                if (message_id <= 0) {
                    continue;
                }
                const auto tracked_hint = g_bot_state.broadcast_hint_messages.find(task->chat_id);
                if (tracked_hint != g_bot_state.broadcast_hint_messages.end() &&
                    tracked_hint->second == message_id) {
                    g_bot_state.broadcast_hint_messages.erase(tracked_hint);
                    save_bot_state_locked();
                }
                due_messages.push_back({task->chat_id, message_id});
            }
            task = g_bot_state.deferred_deletions.erase(task);
        }
    }

    for (const DueMessage& message : due_messages) {
        bot.delete_message(message.chat_id, message.message_id);
    }
}

void ensure_reply_keyboard_removed(long long chat_id, TelegramClient& bot) {
    {
        std::lock_guard<std::mutex> lock(g_bot_state.mutex);
        if (g_bot_state.removed_reply_keyboards[chat_id]) {
            return;
        }
        g_bot_state.removed_reply_keyboards[chat_id] = true;
    }
    bot.remove_reply_keyboard(chat_id);
}

// Export documents have an event-based lifetime, independent of short-lived hints.
void remember_export_message(long long chat_id, int message_id) {
    if (message_id <= 0) return;
    std::lock_guard<std::mutex> lock(g_bot_state.mutex);
    auto& messages = g_bot_state.export_messages[chat_id];
    if (std::find(messages.begin(), messages.end(), message_id) == messages.end())
        messages.push_back(message_id);
    save_bot_state_locked();
}

void delete_tracked_exports(long long chat_id, TelegramClient& bot) {
    std::vector<int> messages;
    {
        std::lock_guard<std::mutex> lock(g_bot_state.mutex);
        const auto it = g_bot_state.export_messages.find(chat_id);
        if (it == g_bot_state.export_messages.end()) return;
        messages = it->second;
    }
    for (const int id : messages) {
        // Retain unsuccessful deletions for the next action, including after a restart.
        if (!bot.delete_message(chat_id, id)) continue;
        std::lock_guard<std::mutex> lock(g_bot_state.mutex);
        const auto it = g_bot_state.export_messages.find(chat_id);
        if (it == g_bot_state.export_messages.end()) continue;
        auto& tracked = it->second;
        tracked.erase(std::remove(tracked.begin(), tracked.end(), id), tracked.end());
        if (tracked.empty()) g_bot_state.export_messages.erase(it);
        save_bot_state_locked();
    }
}


bool has_tracked_reminder(long long chat_id, int message_id) {
    std::lock_guard<std::mutex> lock(g_bot_state.mutex);
    const auto it = g_bot_state.reminder_messages.find(chat_id);
    return it != g_bot_state.reminder_messages.end() && !it->second.empty() &&
        (message_id == 0 || std::find(it->second.begin(), it->second.end(), message_id) != it->second.end());
}

void finish_reminder_transition(long long chat_id, TelegramClient& bot, int current_message_id,
                                bool new_reminder) {
    std::vector<int> messages;
    {
        std::lock_guard<std::mutex> lock(g_bot_state.mutex);
        auto& tracked = g_bot_state.reminder_messages[chat_id];
        const auto context = g_bot_state.screen_contexts.find(chat_id);
        const bool in_lesson = context != g_bot_state.screen_contexts.end() &&
            (context->second == "daily" || context->second == "evening");
        if ((new_reminder || !tracked.empty()) && in_lesson && current_message_id > 0 &&
            std::find(tracked.begin(), tracked.end(), current_message_id) == tracked.end())
            tracked.push_back(current_message_id);
        if (!in_lesson) {
            // The same message has been successfully repurposed as a navigation screen.
            tracked.erase(std::remove(tracked.begin(), tracked.end(), current_message_id), tracked.end());
        }
        messages = tracked;
        save_bot_state_locked();
    }
    for (const int id : messages) {
        if (id == current_message_id || !bot.delete_message(chat_id, id)) continue;
        std::lock_guard<std::mutex> lock(g_bot_state.mutex);
        auto& tracked = g_bot_state.reminder_messages[chat_id];
        tracked.erase(std::remove(tracked.begin(), tracked.end(), id), tracked.end());
        save_bot_state_locked();
    }
}

// Also tracks replaced navigation screens so failed deletions survive restarts.
void remember_obsolete_screen(long long chat_id, int message_id) {
    if (message_id <= 0) return;
    std::lock_guard<std::mutex> lock(g_bot_state.mutex);
    auto& tracked = g_bot_state.reminder_messages[chat_id];
    if (std::find(tracked.begin(), tracked.end(), message_id) == tracked.end())
        tracked.push_back(message_id);
    save_bot_state_locked();
}

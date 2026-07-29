#include "bot_state_internal.h"
#include "presentation/bot_state.h"

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

#include "presentation/bot_state.h"

#include "bot_state_internal.h"

#include <mutex>

using json = nlohmann::json;

BotStateStore g_bot_state;

int get_active_screen_message(long long chat_id) {
    std::lock_guard<std::mutex> lock(g_bot_state.mutex);
    const auto entry = g_bot_state.active_screen_messages.find(chat_id);
    return entry == g_bot_state.active_screen_messages.end() ? 0 : entry->second.message_id;
}

ScreenMessageType get_active_screen_message_type(long long chat_id) {
    std::lock_guard<std::mutex> lock(g_bot_state.mutex);
    const auto entry = g_bot_state.active_screen_messages.find(chat_id);
    return entry == g_bot_state.active_screen_messages.end() ? ScreenMessageType::Unknown
                                                             : entry->second.type;
}

std::string screen_message_type_to_string(ScreenMessageType type) {
    switch (type) {
    case ScreenMessageType::Text:
        return "text";
    case ScreenMessageType::Photo:
        return "photo";
    case ScreenMessageType::Unknown:
    default:
        return "unknown";
    }
}

ScreenMessageType screen_message_type_from_string(const std::string& type) {
    if (type == "text") {
        return ScreenMessageType::Text;
    }
    if (type == "photo") {
        return ScreenMessageType::Photo;
    }
    return ScreenMessageType::Unknown;
}

ScreenMessageType screen_message_type_from_callback(const json& message) {
    if (message.contains("photo")) {
        return ScreenMessageType::Photo;
    }
    if (message.contains("text")) {
        return ScreenMessageType::Text;
    }
    return ScreenMessageType::Unknown;
}

bool is_persistable_screen_context(const std::string& context) {
    return context == "reminders" || context == "main" || context == "topics" || context == "ai" ||
           context == "stats" || context == "generation" || context == "dictionary" ||
           context == "learned" || context == "daily" || context == "evening";
}

void remember_screen_context(long long chat_id, const std::string& context) {
    if (!is_persistable_screen_context(context)) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_bot_state.mutex);
    g_bot_state.screen_contexts[chat_id] = context;
    save_bot_state_locked();
}

std::string get_screen_context(long long chat_id) {
    std::lock_guard<std::mutex> lock(g_bot_state.mutex);
    const auto entry = g_bot_state.screen_contexts.find(chat_id);
    return entry == g_bot_state.screen_contexts.end() ? "" : entry->second;
}

void remember_active_screen_message(long long chat_id, int message_id, ScreenMessageType type) {
    if (message_id <= 0) {
        return;
    }
    std::lock_guard<std::mutex> lock(g_bot_state.mutex);
    ScreenMessageState& state = g_bot_state.active_screen_messages[chat_id];
    state.message_id = message_id;
    if (type != ScreenMessageType::Unknown) {
        state.type = type;
    }
    save_bot_state_locked();
}

void clear_active_screen_message(long long chat_id) {
    std::lock_guard<std::mutex> lock(g_bot_state.mutex);
    g_bot_state.active_screen_messages.erase(chat_id);
    save_bot_state_locked();
}

void delete_active_screen_message(long long chat_id, TelegramClient& bot) {
    int message_id = 0;
    {
        std::lock_guard<std::mutex> lock(g_bot_state.mutex);
        const auto entry = g_bot_state.active_screen_messages.find(chat_id);
        if (entry == g_bot_state.active_screen_messages.end()) {
            return;
        }
        message_id = entry->second.message_id;
        g_bot_state.active_screen_messages.erase(entry);
        save_bot_state_locked();
    }
    bot.delete_message(chat_id, message_id);
}

#pragma once

#include "presentation/bot_state.h"

#include <chrono>
#include <map>
#include <mutex>
#include <string>
#include <vector>

struct ScreenMessageState {
    int message_id = 0;
    ScreenMessageType type = ScreenMessageType::Unknown;
};

struct DeferredMessageDeletion {
    std::chrono::steady_clock::time_point due_at;
    long long chat_id = 0;
    std::vector<int> message_ids;
};

struct BotStateStore {
    std::mutex mutex;
    std::map<long long, ScreenMessageState> active_screen_messages;
    std::map<long long, bool> removed_reply_keyboards;
    std::map<long long, int> last_ai_user_messages;
    std::map<long long, int> broadcast_hint_messages;
    std::map<long long, std::vector<int>> export_messages;
    std::map<long long, std::vector<int>> reminder_messages;
    std::map<long long, std::string> chat_languages;
    std::map<long long, std::string> screen_contexts;
    std::vector<DeferredMessageDeletion> deferred_deletions;
};

extern BotStateStore g_bot_state;

std::string screen_message_type_to_string(ScreenMessageType type);
ScreenMessageType screen_message_type_from_string(const std::string& type);
void save_bot_state_locked();

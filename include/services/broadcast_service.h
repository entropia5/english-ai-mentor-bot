#pragma once

#include "domain/word.h"

#include <string>
#include <vector>

class Database;
class TelegramClient;

enum class BroadcastResult { Delivered, NoContent, Disabled, Failed };

std::vector<long long> configured_broadcast_users();
BroadcastResult send_daily_review(long long chat_id, TelegramClient& bot, Database& database);
BroadcastResult send_evening_new_words(long long chat_id, TelegramClient& bot, Database& database);

struct ReminderContent {
    bool enabled = false;
    bool failed = false;
    std::string filter = "all";
    std::vector<Word> words;
};
ReminderContent prepare_morning_review(long long chat_id, Database& database);
ReminderContent prepare_evening_review(long long chat_id, Database& database);

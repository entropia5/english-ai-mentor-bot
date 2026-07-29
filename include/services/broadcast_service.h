#pragma once

#include <vector>

class Database;
class TelegramClient;

enum class BroadcastResult { Delivered, NoContent, Failed };

std::vector<long long> configured_broadcast_users();
BroadcastResult send_daily_review(long long chat_id, TelegramClient& bot, Database& database);
BroadcastResult send_evening_new_words(long long chat_id, TelegramClient& bot, Database& database);

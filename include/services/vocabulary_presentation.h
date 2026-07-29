#pragma once

#include <string>

class Database;
class GroqClient;
class TelegramClient;

void generate_words(long long chat_id, TelegramClient& bot, Database& database, GroqClient& ai,
                    const std::string& topic_keyword, const std::string& topic_name,
                    int message_id = 0);

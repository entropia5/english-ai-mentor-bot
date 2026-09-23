#pragma once

#include "database.h"

#include <string>
#include <vector>

class TelegramClient;

bool try_mark_words_from_input(long long chat_id, const std::string& text, Database& database);
std::vector<WordView> get_learned_words_for_review(long long chat_id, Database& database,
                                                   const std::string& filter = "");
void refresh_after_marking_words(long long chat_id, TelegramClient& bot, Database& database,
                                 int& dictionary_page, int& dictionary_message_id,
                                 int& learned_page, int& learned_message_id,
                                 std::string& current_action);

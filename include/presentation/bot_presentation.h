#pragma once

#include "database.h"
#include "presentation/inline_keyboard.h"
#include "telegram_client.h"

#include <string>
#include <vector>

bool show_status_screen(long long chat_id, TelegramClient& bot, const std::string& key,
                        const std::string& title, const std::string& subtitle,
                        const std::string& note, const InlineKeyboard& buttons, int message_id = 0);

void send_main_menu(long long chat_id, TelegramClient& bot, Database& database, int message_id = 0);
void send_topic_menu(long long chat_id, TelegramClient& bot, int message_id = 0);
void show_ai_prompt(long long chat_id, TelegramClient& bot, int message_id = 0);
void show_dictionary_page(long long chat_id, TelegramClient& bot,
                          const std::vector<WordView>& words, int requested_page, int& current_page,
                          std::string& last_action, int& message_id, bool force_new);
void show_learned_page(long long chat_id, TelegramClient& bot, const std::vector<WordView>& words,
                       int requested_page, int& current_page, std::string& last_action,
                       int& message_id, bool force_new);
bool show_daily_review_page(long long chat_id, TelegramClient& bot,
                            const std::vector<WordView>& words, int requested_page,
                            int message_id = 0, bool force_new = true,
                            std::string* last_action = nullptr);
bool show_evening_words_page(long long chat_id, TelegramClient& bot,
                             const std::vector<WordView>& words, int requested_page,
                             int message_id = 0, bool force_new = true,
                             std::string* last_action = nullptr);
void show_stats(long long chat_id, TelegramClient& bot, Database& database, int message_id = 0);

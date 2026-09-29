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
                          std::string& last_action, int& message_id, bool force_new,
                          const std::string& filter = "all");
void show_learned_page(long long chat_id, TelegramClient& bot, const std::vector<WordView>& words,
                       int requested_page, int& current_page, std::string& last_action,
                       int& message_id, bool force_new, const std::string& filter = "all");
bool show_daily_review_page(long long chat_id, TelegramClient& bot,
                            const std::vector<WordView>& words, int requested_page,
                            int message_id = 0, bool force_new = true,
                            std::string* last_action = nullptr, const std::string& filter = "all");
bool show_evening_words_page(long long chat_id, TelegramClient& bot,
                             const std::vector<WordView>& words, int requested_page,
                             int message_id = 0, bool force_new = true,
                             std::string* last_action = nullptr, const std::string& filter = "all",
                             bool new_words = false);
void show_stats(long long chat_id, TelegramClient& bot, Database& database, int message_id = 0);

void show_reminder_settings(long long chat_id, TelegramClient& bot, Database& database,
                            int message_id = 0);
void show_user_dictionary(long long chat_id, TelegramClient& bot, Database& database, bool learned,
                          int page = 0, int message_id = 0, bool is_new = false);

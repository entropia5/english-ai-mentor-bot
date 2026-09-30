#pragma once

#include "telegram_client.h"

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

enum class ScreenMessageType { Unknown, Text, Photo };

void load_bot_state();

int get_active_screen_message(long long chat_id);
ScreenMessageType get_active_screen_message_type(long long chat_id);
bool is_persistable_screen_context(const std::string& context);
std::string get_screen_context(long long chat_id);
void remember_screen_context(long long chat_id, const std::string& context);
ScreenMessageType screen_message_type_from_callback(const nlohmann::json& message);
void remember_active_screen_message(long long chat_id, int message_id,
                                    ScreenMessageType type = ScreenMessageType::Unknown);
void clear_active_screen_message(long long chat_id);
void delete_active_screen_message(long long chat_id, TelegramClient& bot);
void delete_tracked_ai_input(long long chat_id, TelegramClient& bot, int except_message_id = 0);
void delete_tracked_broadcast_hint(long long chat_id, TelegramClient& bot);
void remember_export_message(long long chat_id, int message_id);
void delete_tracked_exports(long long chat_id, TelegramClient& bot);
void remember_ai_input(long long chat_id, int message_id);
void remember_broadcast_hint(long long chat_id, int message_id);
void delete_messages_after_delay(TelegramClient& bot, long long chat_id,
                                 std::vector<int> message_ids, int delay_seconds);
void process_deferred_message_deletions(TelegramClient& bot);
void ensure_reply_keyboard_removed(long long chat_id, TelegramClient& bot);

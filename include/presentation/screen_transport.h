#pragma once

#include "presentation/inline_keyboard.h"

class TelegramClient;

bool upsert_screen(long long chat_id, TelegramClient& bot, const std::string& text,
                   const InlineKeyboard& buttons, int preferred_message_id = 0);
bool upsert_photo_screen(long long chat_id, TelegramClient& bot, const std::string& photo_path,
                         const InlineKeyboard& buttons, int preferred_message_id = 0,
                         const std::string& caption = "", const std::string& parse_mode = "");

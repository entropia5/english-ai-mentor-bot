#include "presentation/screen_transport.h"

#include "logger.h"
#include "presentation/bot_state.h"
#include "telegram_client.h"

bool upsert_screen(long long chat_id, TelegramClient& bot, const std::string& text,
                   const InlineKeyboard& buttons, int preferred_message_id) {
    const int message_id =
        preferred_message_id > 0 ? preferred_message_id : get_active_screen_message(chat_id);
    if (message_id > 0) {
        const ScreenMessageType current_type = get_active_screen_message_type(chat_id);
        if (current_type == ScreenMessageType::Text || current_type == ScreenMessageType::Unknown) {
            TelegramRequestResult edit_result;
            if (bot.edit_message(chat_id, message_id, text, buttons, &edit_result)) {
                remember_active_screen_message(chat_id, message_id, ScreenMessageType::Text);
                return true;
            }

            if (edit_result.failure_kind != TelegramFailureKind::Permanent) {
                LOG_WARNING("Keeping live dashboard message " + std::to_string(message_id) +
                            " after temporary editMessageText failure for chat " +
                            std::to_string(chat_id));
                return false;
            }
        } else {
            LOG("Replacing photo live dashboard message " + std::to_string(message_id) +
                " with text screen for chat " + std::to_string(chat_id));
        }

        LOG_WARNING("Resetting stale live dashboard message " + std::to_string(message_id) +
                    " for chat " + std::to_string(chat_id));
        bot.delete_message(chat_id, message_id);
        clear_active_screen_message(chat_id);
    }

    int sent_message_id = 0;
    if (bot.send_inline_keyboard(chat_id, text, buttons, &sent_message_id)) {
        remember_active_screen_message(chat_id, sent_message_id, ScreenMessageType::Text);
        return true;
    }
    return false;
}

bool upsert_photo_screen(long long chat_id, TelegramClient& bot, const std::string& photo_path,
                         const InlineKeyboard& buttons, int preferred_message_id,
                         const std::string& caption, const std::string& parse_mode) {
    const int message_id =
        preferred_message_id > 0 ? preferred_message_id : get_active_screen_message(chat_id);
    if (message_id > 0) {
        const ScreenMessageType current_type = get_active_screen_message_type(chat_id);
        if (current_type == ScreenMessageType::Photo ||
            current_type == ScreenMessageType::Unknown) {
            TelegramRequestResult edit_result;
            if (bot.edit_message_photo(chat_id, message_id, photo_path, buttons, caption,
                                       &edit_result, parse_mode)) {
                remember_active_screen_message(chat_id, message_id, ScreenMessageType::Photo);
                return true;
            }

            if (edit_result.failure_kind != TelegramFailureKind::Permanent) {
                LOG_WARNING("Keeping live dashboard message " + std::to_string(message_id) +
                            " after temporary editMessageMedia failure for chat " +
                            std::to_string(chat_id));
                return false;
            }
        } else {
            LOG("Replacing text live dashboard message " + std::to_string(message_id) +
                " with photo screen for chat " + std::to_string(chat_id));
        }

        LOG_WARNING("Resetting stale live dashboard message " + std::to_string(message_id) +
                    " for chat " + std::to_string(chat_id));
        bot.delete_message(chat_id, message_id);
        clear_active_screen_message(chat_id);
    }

    int sent_message_id = 0;
    if (bot.send_photo(chat_id, photo_path, buttons, caption, &sent_message_id, parse_mode)) {
        remember_active_screen_message(chat_id, sent_message_id, ScreenMessageType::Photo);
        return true;
    }
    return false;
}

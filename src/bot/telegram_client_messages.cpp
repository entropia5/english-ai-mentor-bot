#include "logger.h"
#include "telegram_client.h"
#include "telegram_client_internal.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {

void log_transport_failure(const std::string& action, const TelegramHttpResponse& response) {
    LOG_ERROR(action + " CURL failed: HTTP " + std::to_string(response.http_status) +
              ", error: " + response.error);
}

} // namespace

bool TelegramClient::send_message(long long chat_id, const std::string& text,
                                  const std::string& parse_mode, int* message_id) {
    json payload = {{"chat_id", chat_id}, {"text", text}};
    if (!parse_mode.empty()) {
        payload["parse_mode"] = parse_mode;
    }

    const TelegramHttpResponse response = telegram_post_json(api_url + "/sendMessage", payload);
    if (!response.transport_ok) {
        log_transport_failure("sendMessage", response);
        return false;
    }

    const bool ok = telegram_ok(response.body, "sendMessage", message_id, response.http_status);
    if (ok) {
        LOG("Message sent to " + std::to_string(chat_id));
    }
    return ok;
}

bool TelegramClient::remove_reply_keyboard(long long chat_id) {
    const json payload = {
        {"chat_id", chat_id},
        {"text", "."},
        {"reply_markup", {{"remove_keyboard", true}}},
    };
    const TelegramHttpResponse response = telegram_post_json(api_url + "/sendMessage", payload);
    if (!response.transport_ok) {
        log_transport_failure("remove reply keyboard", response);
        return false;
    }

    int message_id = 0;
    const bool ok =
        telegram_ok(response.body, "remove reply keyboard", &message_id, response.http_status);
    if (ok) {
        LOG("Reply keyboard removed for " + std::to_string(chat_id));
        if (message_id > 0) {
            delete_message(chat_id, message_id);
        }
    }
    return ok;
}

bool TelegramClient::send_inline_keyboard(
    long long chat_id, const std::string& text,
    const std::vector<std::vector<std::pair<std::string, std::string>>>& buttons, int* message_id) {
    const json payload = {
        {"chat_id", chat_id},
        {"text", text},
        {"parse_mode", "Markdown"},
        {"reply_markup", build_inline_keyboard_json(buttons)},
    };
    const TelegramHttpResponse response = telegram_post_json(api_url + "/sendMessage", payload);
    if (!response.transport_ok) {
        log_transport_failure("send inline keyboard", response);
        return false;
    }

    const bool ok =
        telegram_ok(response.body, "sendMessage inline", message_id, response.http_status);
    if (ok) {
        LOG("Inline keyboard sent to " + std::to_string(chat_id));
    }
    return ok;
}

bool TelegramClient::edit_message(
    long long chat_id, int message_id, const std::string& text,
    const std::vector<std::vector<std::pair<std::string, std::string>>>& buttons,
    TelegramRequestResult* result) {
    json payload = {
        {"chat_id", chat_id},
        {"message_id", message_id},
        {"text", text},
        {"parse_mode", "Markdown"},
    };
    if (!buttons.empty() && !buttons.front().empty()) {
        payload["reply_markup"] = build_inline_keyboard_json(buttons);
    }

    const TelegramHttpResponse response = telegram_post_json(api_url + "/editMessageText", payload);
    if (!response.transport_ok) {
        if (result != nullptr) {
            result->ok = false;
            result->failure_kind = TelegramFailureKind::Temporary;
            result->http_status = response.http_status;
            result->description = response.error;
            result->response = response.body;
        }
        log_transport_failure("editMessageText", response);
        return false;
    }

    const bool ok =
        telegram_ok(response.body, "editMessageText", nullptr, response.http_status, result);
    if (ok) {
        LOG("Message edited for " + std::to_string(chat_id));
    }
    return ok;
}

bool TelegramClient::answer_callback_query(const std::string& callback_query_id) {
    const json payload = {{"callback_query_id", callback_query_id}};
    const TelegramHttpResponse response =
        telegram_post_json(api_url + "/answerCallbackQuery", payload);
    if (!response.transport_ok) {
        log_transport_failure("answerCallbackQuery", response);
        return false;
    }
    return telegram_ok(response.body, "answerCallbackQuery", nullptr, response.http_status);
}

bool TelegramClient::delete_message(long long chat_id, int message_id) {
    const json payload = {{"chat_id", chat_id}, {"message_id", message_id}};
    const TelegramHttpResponse response = telegram_post_json(api_url + "/deleteMessage", payload);
    if (!response.transport_ok) {
        log_transport_failure("deleteMessage", response);
        return false;
    }
    return telegram_ok(response.body, "deleteMessage", nullptr, response.http_status);
}

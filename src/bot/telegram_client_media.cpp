#include "logger.h"
#include "telegram_client.h"
#include "telegram_client_internal.h"

#include <nlohmann/json.hpp>

namespace {

void populate_transport_failure(TelegramRequestResult* result,
                                const TelegramHttpResponse& response) {
    if (result == nullptr) {
        return;
    }
    result->ok = false;
    result->failure_kind = TelegramFailureKind::Temporary;
    result->http_status = response.http_status;
    result->description = response.error;
    result->response = response.body;
}

void log_media_transport_failure(const std::string& action, const TelegramHttpResponse& response) {
    LOG_ERROR(action + " CURL failed: HTTP " + std::to_string(response.http_status) +
              ", error: " + response.error + ", response: " + response.body);
}

} // namespace

bool TelegramClient::send_photo(
    long long chat_id, const std::string& photo_path,
    const std::vector<std::vector<std::pair<std::string, std::string>>>& buttons,
    const std::string& caption, int* message_id) {
    std::vector<TelegramMultipartField> fields = {
        {"chat_id", std::to_string(chat_id), false},
        {"photo", photo_path, true},
    };
    if (!caption.empty()) {
        fields.push_back({"caption", caption, false});
    }
    fields.push_back({"reply_markup", build_inline_keyboard_json(buttons).dump(), false});

    const TelegramHttpResponse response = telegram_post_multipart(api_url + "/sendPhoto", fields);
    if (!response.transport_ok) {
        log_media_transport_failure("sendPhoto", response);
        return false;
    }

    const bool ok = telegram_ok(response.body, "sendPhoto", message_id, response.http_status);
    if (ok) {
        LOG("Photo sent to " + std::to_string(chat_id));
    }
    return ok;
}

bool TelegramClient::edit_message_photo(
    long long chat_id, int message_id, const std::string& photo_path,
    const std::vector<std::vector<std::pair<std::string, std::string>>>& buttons,
    const std::string& caption, TelegramRequestResult* result) {
    nlohmann::json media = {{"type", "photo"}, {"media", "attach://photo"}};
    if (!caption.empty()) {
        media["caption"] = caption;
    }
    const std::vector<TelegramMultipartField> fields = {
        {"chat_id", std::to_string(chat_id), false},
        {"message_id", std::to_string(message_id), false},
        {"media", media.dump(), false},
        {"photo", photo_path, true},
        {"reply_markup", build_inline_keyboard_json(buttons).dump(), false},
    };

    const TelegramHttpResponse response =
        telegram_post_multipart(api_url + "/editMessageMedia", fields);
    if (!response.transport_ok) {
        populate_transport_failure(result, response);
        log_media_transport_failure("editMessageMedia", response);
        return false;
    }

    const bool ok =
        telegram_ok(response.body, "editMessageMedia", nullptr, response.http_status, result);
    if (ok) {
        LOG("Photo message edited for " + std::to_string(chat_id));
    }
    return ok;
}

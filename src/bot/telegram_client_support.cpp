#include "logger.h"
#include "telegram_client_internal.h"

#include <algorithm>
#include <cctype>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static bool is_edit_action(const std::string& action) {
    return action.find("editMessage") != std::string::npos ||
           action.find("edit message") != std::string::npos;
}

static TelegramFailureKind classify_telegram_failure(long http_status, int error_code,
                                                     const std::string& description,
                                                     const std::string& action) {
    std::string lower = description;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    if (http_status >= 500 || http_status == 429 || error_code == 429 ||
        lower.find("too many requests") != std::string::npos ||
        lower.find("timeout") != std::string::npos ||
        lower.find("temporarily") != std::string::npos) {
        return TelegramFailureKind::Temporary;
    }

    if (is_edit_action(action) &&
        (lower.find("message to edit not found") != std::string::npos ||
         lower.find("message not found") != std::string::npos ||
         lower.find("message can't be edited") != std::string::npos ||
         lower.find("message cannot be edited") != std::string::npos ||
         lower.find("there is no text in the message to edit") != std::string::npos ||
         lower.find("there is no photo in the message to edit") != std::string::npos ||
         lower.find("wrong file identifier/http url specified") != std::string::npos ||
         lower.find("message is not a photo") != std::string::npos)) {
        return TelegramFailureKind::Permanent;
    }

    return TelegramFailureKind::Temporary;
}

bool telegram_ok(const std::string& response, const std::string& action, int* message_id,
                 long http_status, TelegramRequestResult* result) {
    if (result != nullptr) {
        result->ok = false;
        result->failure_kind = TelegramFailureKind::None;
        result->http_status = http_status;
        result->api_error_code = 0;
        result->description.clear();
        result->response = response;
    }

    try {
        auto resp = json::parse(response);
        if (resp.value("ok", false)) {
            if (message_id != nullptr && resp.contains("result") &&
                resp["result"].contains("message_id")) {
                *message_id = resp["result"]["message_id"].get<int>();
            }
            if (result != nullptr) {
                result->ok = true;
            }
            return true;
        }

        std::string description = resp.value("description", "unknown Telegram API error");
        int error_code = resp.value("error_code", 0);
        if (description.find("message is not modified") != std::string::npos) {
            if (result != nullptr) {
                result->ok = true;
            }
            return true;
        }

        TelegramFailureKind failure_kind =
            classify_telegram_failure(http_status, error_code, description, action);
        if (result != nullptr) {
            result->failure_kind = failure_kind;
            result->api_error_code = error_code;
            result->description = description;
        }

        LOG_ERROR(action + " failed: HTTP " + std::to_string(http_status) +
                  ", Telegram error_code " + std::to_string(error_code) +
                  ", description: " + description + ", response: " + response);
    } catch (const std::exception& e) {
        if (result != nullptr) {
            result->failure_kind = TelegramFailureKind::Temporary;
            result->description = e.what();
        }
        LOG_ERROR(action + " response parse failed: HTTP " + std::to_string(http_status) +
                  ", error: " + std::string(e.what()) + "; response: " + response);
    }
    return false;
}

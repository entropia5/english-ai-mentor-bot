
#include "telegram_client.h"
#include "logger.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cctype>
#include <sstream>

using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t total = size * nmemb;
    response->append((char*)contents, total);
    return total;
}

TelegramClient::TelegramClient(const std::string& bot_token)
    : token(bot_token), api_url("https://api.telegram.org/bot" + token) {}

static bool is_edit_action(const std::string& action) {
    return action.find("editMessage") != std::string::npos ||
           action.find("edit message") != std::string::npos;
}

static TelegramFailureKind classify_telegram_failure(long http_status, int error_code,
                                                     const std::string& description,
                                                     const std::string& action) {
    std::string lower = description;
    std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) {
        return std::tolower(c);
    });

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

static bool telegram_ok(const std::string& response, const std::string& action,
                        int* message_id = nullptr, long http_status = 0,
                        TelegramRequestResult* result = nullptr) {
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
            if (message_id != nullptr && resp.contains("result") && resp["result"].contains("message_id")) {
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
                  ", description: " + description +
                  ", response: " + response);
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

static json build_inline_keyboard_json(const std::vector<std::vector<std::pair<std::string, std::string>>>& buttons) {
    json kb;
    kb["inline_keyboard"] = json::array();

    for (const auto& row : buttons) {
        json kb_row = json::array();
        for (const auto& btn : row) {
            json btn_json;
            btn_json["text"] = btn.first;
            btn_json["callback_data"] = btn.second;
            kb_row.push_back(btn_json);
        }
        kb["inline_keyboard"].push_back(kb_row);
    }

    return kb;
}

bool TelegramClient::send_message(long long chat_id, const std::string& text, const std::string& parse_mode,
                                  int* message_id) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        LOG_ERROR("Failed to init CURL");
        return false;
    }

    json payload;
    payload["chat_id"] = chat_id;
    payload["text"] = text;
    if (!parse_mode.empty()) {
        payload["parse_mode"] = parse_mode;
    }

    std::string post_data = payload.dump();
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, (api_url + "/sendMessage").c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    long http_status = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_status);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK) {
        bool ok = telegram_ok(response, "sendMessage", message_id, http_status);
        if (ok) {
            LOG("Message sent to " + std::to_string(chat_id));
        }
        return ok;
    }
    LOG_ERROR("sendMessage CURL failed: HTTP " + std::to_string(http_status) +
              ", error: " + std::string(curl_easy_strerror(res)));
    return false;
}

bool TelegramClient::remove_reply_keyboard(long long chat_id) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        LOG_ERROR("Failed to init CURL");
        return false;
    }

    json payload;
    payload["chat_id"] = chat_id;
    payload["text"] = ".";
    payload["reply_markup"] = {
        {"remove_keyboard", true}
    };

    std::string post_data = payload.dump();
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, (api_url + "/sendMessage").c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    long http_status = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_status);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK) {
        int message_id = 0;
        bool ok = telegram_ok(response, "remove reply keyboard", &message_id, http_status);
        if (ok) {
            LOG("Reply keyboard removed for " + std::to_string(chat_id));
            if (message_id > 0) {
                delete_message(chat_id, message_id);
            }
        }
        return ok;
    }

    LOG_ERROR("Failed to remove reply keyboard: HTTP " + std::to_string(http_status) +
              ", error: " + std::string(curl_easy_strerror(res)));
    return false;
}

std::string TelegramClient::get_updates(int offset, int timeout) {
    CURL* curl = curl_easy_init();
    if (!curl) return "{}";

    std::string url = api_url + "/getUpdates?offset=" + std::to_string(offset) +
                      "&timeout=" + std::to_string(timeout);
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)timeout + 15L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        LOG_ERROR("getUpdates CURL failed: " + std::string(curl_easy_strerror(res)));
        return "{}";
    }

    return response;
}

bool TelegramClient::test_token() {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    std::string url = api_url + "/getMe";
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK) {
        try {
            auto resp = json::parse(response);
            if (resp.contains("ok") && resp["ok"] == true) {
                LOG("Bot authorized: @" + resp["result"]["username"].get<std::string>());
                return true;
            }
        } catch (...) {}
    }
    return false;
}

std::string TelegramClient::get_me() {
    CURL* curl = curl_easy_init();
    if (!curl) return "{}";

    std::string url = api_url + "/getMe";
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return response;
}

bool TelegramClient::delete_webhook(bool drop_pending_updates) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        LOG_ERROR("Failed to init CURL");
        return false;
    }

    json payload;
    payload["drop_pending_updates"] = drop_pending_updates;

    std::string post_data = payload.dump();
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, (api_url + "/deleteWebhook").c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    long http_status = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_status);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK) {
        bool ok = telegram_ok(response, "deleteWebhook", nullptr, http_status);
        if (ok) {
            LOG(std::string("Webhook disabled") +
                (drop_pending_updates ? " and pending updates dropped" : ""));
        }
        return ok;
    }

    LOG_ERROR("deleteWebhook CURL failed: HTTP " + std::to_string(http_status) +
              ", error: " + std::string(curl_easy_strerror(res)));
    return false;
}

bool TelegramClient::send_inline_keyboard(long long chat_id, const std::string& text,
                                          const std::vector<std::vector<std::pair<std::string, std::string>>>& buttons,
                                          int* message_id) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        LOG_ERROR("Failed to init CURL");
        return false;
    }

    json payload;
    payload["chat_id"] = chat_id;
    payload["text"] = text;
    payload["parse_mode"] = "Markdown";
    payload["reply_markup"] = build_inline_keyboard_json(buttons);

    std::string post_data = payload.dump();
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, (api_url + "/sendMessage").c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    long http_status = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_status);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK) {
        bool ok = telegram_ok(response, "sendMessage inline", message_id, http_status);
        if (ok) {
            LOG("Inline keyboard sent to " + std::to_string(chat_id));
        }
        return ok;
    }

    LOG_ERROR("Failed to send inline keyboard: HTTP " + std::to_string(http_status) +
              ", error: " + std::string(curl_easy_strerror(res)));
    return false;
}

bool TelegramClient::send_photo(long long chat_id, const std::string& photo_path,
                                const std::vector<std::vector<std::pair<std::string, std::string>>>& buttons,
                                const std::string& caption, int* message_id) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        LOG_ERROR("Failed to init CURL");
        return false;
    }

    std::string response;
    std::string chat = std::to_string(chat_id);
    std::string reply_markup = build_inline_keyboard_json(buttons).dump();

    curl_mime* mime = curl_mime_init(curl);
    curl_mimepart* part = curl_mime_addpart(mime);
    curl_mime_name(part, "chat_id");
    curl_mime_data(part, chat.c_str(), CURL_ZERO_TERMINATED);

    part = curl_mime_addpart(mime);
    curl_mime_name(part, "photo");
    curl_mime_filedata(part, photo_path.c_str());

    if (!caption.empty()) {
        part = curl_mime_addpart(mime);
        curl_mime_name(part, "caption");
        curl_mime_data(part, caption.c_str(), CURL_ZERO_TERMINATED);
    }

    part = curl_mime_addpart(mime);
    curl_mime_name(part, "reply_markup");
    curl_mime_data(part, reply_markup.c_str(), CURL_ZERO_TERMINATED);

    curl_easy_setopt(curl, CURLOPT_URL, (api_url + "/sendPhoto").c_str());
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    long http_status = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_status);
    curl_mime_free(mime);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK) {
        bool ok = telegram_ok(response, "sendPhoto", message_id, http_status);
        if (ok) {
            LOG("Photo sent to " + std::to_string(chat_id));
        }
        return ok;
    }

    LOG_ERROR("Failed to send photo: HTTP " + std::to_string(http_status) +
              ", error: " + std::string(curl_easy_strerror(res)));
    return false;
}

bool TelegramClient::edit_message_photo(long long chat_id, int message_id, const std::string& photo_path,
                                        const std::vector<std::vector<std::pair<std::string, std::string>>>& buttons,
                                        const std::string& caption,
                                        TelegramRequestResult* result) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        LOG_ERROR("Failed to init CURL");
        if (result != nullptr) {
            result->ok = false;
            result->failure_kind = TelegramFailureKind::Temporary;
            result->description = "Failed to init CURL";
        }
        return false;
    }

    json media;
    media["type"] = "photo";
    media["media"] = "attach://photo";
    if (!caption.empty()) {
        media["caption"] = caption;
    }

    std::string response;
    std::string chat = std::to_string(chat_id);
    std::string msg_id = std::to_string(message_id);
    std::string media_data = media.dump();
    std::string reply_markup = build_inline_keyboard_json(buttons).dump();

    curl_mime* mime = curl_mime_init(curl);
    curl_mimepart* part = curl_mime_addpart(mime);
    curl_mime_name(part, "chat_id");
    curl_mime_data(part, chat.c_str(), CURL_ZERO_TERMINATED);

    part = curl_mime_addpart(mime);
    curl_mime_name(part, "message_id");
    curl_mime_data(part, msg_id.c_str(), CURL_ZERO_TERMINATED);

    part = curl_mime_addpart(mime);
    curl_mime_name(part, "media");
    curl_mime_data(part, media_data.c_str(), CURL_ZERO_TERMINATED);

    part = curl_mime_addpart(mime);
    curl_mime_name(part, "photo");
    curl_mime_filedata(part, photo_path.c_str());

    part = curl_mime_addpart(mime);
    curl_mime_name(part, "reply_markup");
    curl_mime_data(part, reply_markup.c_str(), CURL_ZERO_TERMINATED);

    curl_easy_setopt(curl, CURLOPT_URL, (api_url + "/editMessageMedia").c_str());
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    long http_status = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_status);
    curl_mime_free(mime);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK) {
        bool ok = telegram_ok(response, "editMessageMedia", nullptr, http_status, result);
        if (ok) {
            LOG("Photo message edited for " + std::to_string(chat_id));
        }
        return ok;
    }

    if (result != nullptr) {
        result->ok = false;
        result->failure_kind = TelegramFailureKind::Temporary;
        result->http_status = http_status;
        result->description = curl_easy_strerror(res);
        result->response = response;
    }
    LOG_ERROR("Failed to edit photo message: HTTP " + std::to_string(http_status) +
              ", error: " + std::string(curl_easy_strerror(res)) +
              ", response: " + response);
    return false;
}

bool TelegramClient::edit_message(long long chat_id, int message_id, const std::string& text,
                                  const std::vector<std::vector<std::pair<std::string, std::string>>>& buttons,
                                  TelegramRequestResult* result) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        LOG_ERROR("Failed to init CURL");
        if (result != nullptr) {
            result->ok = false;
            result->failure_kind = TelegramFailureKind::Temporary;
            result->description = "Failed to init CURL";
        }
        return false;
    }

    json payload;
    payload["chat_id"] = chat_id;
    payload["message_id"] = message_id;
    payload["text"] = text;
    payload["parse_mode"] = "Markdown";

    if (!buttons.empty() && !buttons[0].empty()) {
        payload["reply_markup"] = build_inline_keyboard_json(buttons);
    }

    std::string post_data = payload.dump();
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, (api_url + "/editMessageText").c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    long http_status = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_status);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK) {
        bool ok = telegram_ok(response, "editMessageText", nullptr, http_status, result);
        if (ok) {
            LOG("Message edited for " + std::to_string(chat_id));
        }
        return ok;
    }

    if (result != nullptr) {
        result->ok = false;
        result->failure_kind = TelegramFailureKind::Temporary;
        result->http_status = http_status;
        result->description = curl_easy_strerror(res);
        result->response = response;
    }
    LOG_ERROR("Failed to edit message: HTTP " + std::to_string(http_status) +
              ", error: " + std::string(curl_easy_strerror(res)) +
              ", response: " + response);
    return false;
}

bool TelegramClient::answer_callback_query(const std::string& callback_query_id) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        LOG_ERROR("Failed to init CURL");
        return false;
    }

    json payload;
    payload["callback_query_id"] = callback_query_id;

    std::string post_data = payload.dump();
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, (api_url + "/answerCallbackQuery").c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK) {
        return telegram_ok(response, "answerCallbackQuery");
    }

    LOG_ERROR("Failed to answer callback query: " + std::string(curl_easy_strerror(res)));
    return false;
}

bool TelegramClient::delete_message(long long chat_id, int message_id) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        LOG_ERROR("Failed to init CURL");
        return false;
    }

    json payload;
    payload["chat_id"] = chat_id;
    payload["message_id"] = message_id;

    std::string post_data = payload.dump();
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, (api_url + "/deleteMessage").c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK) {
        return telegram_ok(response, "deleteMessage");
    }

    LOG_ERROR("Failed to delete message: " + std::string(curl_easy_strerror(res)));
    return false;
}

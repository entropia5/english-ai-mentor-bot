#include "logger.h"
#include "telegram_client.h"
#include "telegram_client_internal.h"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

TelegramClient::TelegramClient(const std::string& bot_token)
    : token(bot_token), api_url("https://api.telegram.org/bot" + token) {}

std::string TelegramClient::get_updates(int offset, int timeout) {
    CURL* curl = curl_easy_init();
    if (!curl)
        return "{}";

    std::string url = api_url + "/getUpdates?offset=" + std::to_string(offset) +
                      "&timeout=" + std::to_string(timeout);
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, (long)timeout + 15L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, telegram_write_callback);
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
    if (!curl)
        return false;

    std::string url = api_url + "/getMe";
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, telegram_write_callback);
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
        } catch (...) {
        }
    }
    return false;
}

std::string TelegramClient::get_me() {
    CURL* curl = curl_easy_init();
    if (!curl)
        return "{}";

    std::string url = api_url + "/getMe";
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, telegram_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return response;
}

bool TelegramClient::delete_webhook(bool drop_pending_updates) {
    const json payload = {{"drop_pending_updates", drop_pending_updates}};
    const TelegramHttpResponse response = telegram_post_json(api_url + "/deleteWebhook", payload);
    if (!response.transport_ok) {
        LOG_ERROR("deleteWebhook CURL failed: HTTP " + std::to_string(response.http_status) +
                  ", error: " + response.error);
        return false;
    }

    const bool ok = telegram_ok(response.body, "deleteWebhook", nullptr, response.http_status);
    if (ok) {
        LOG(std::string("Webhook disabled") +
            (drop_pending_updates ? " and pending updates dropped" : ""));
    }
    return ok;
}

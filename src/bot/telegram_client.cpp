
#include "telegram_client.h"
#include "logger.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t total = size * nmemb;
    response->append((char*)contents, total);
    return total;
}

TelegramClient::TelegramClient(const std::string& bot_token)
    : token(bot_token), api_url("https://api.telegram.org/bot" + token) {}

bool TelegramClient::send_message(long long chat_id, const std::string& text, const std::string& parse_mode) {
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
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK) {
        LOG("Message sent to " + std::to_string(chat_id));
        return true;
    }
    return false;
}

bool TelegramClient::send_message_with_keyboard(long long chat_id, const std::string& text,
                                                const std::vector<std::vector<std::string>>& keyboard,
                                                bool resize, bool one_time) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        LOG_ERROR("Failed to init CURL");
        return false;
    }

    json kb;
    kb["keyboard"] = json::array();
    for (const auto& row : keyboard) {
        json kb_row = json::array();
        for (const auto& btn : row) {
            kb_row.push_back(btn);
        }
        kb["keyboard"].push_back(kb_row);
    }
    kb["resize_keyboard"] = resize;
    kb["one_time_keyboard"] = one_time;

    json payload;
    payload["chat_id"] = chat_id;
    payload["text"] = text;
    payload["parse_mode"] = "Markdown";
    payload["reply_markup"] = kb;

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
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK) {
        LOG("Message with keyboard sent to " + std::to_string(chat_id));
        return true;
    }

    LOG_ERROR("Failed to send message with keyboard");
    return false;
}

std::string TelegramClient::get_updates(int offset, int timeout) {
    CURL* curl = curl_easy_init();
    if (!curl) return "{}";

    std::string url = api_url + "/getUpdates?offset=" + std::to_string(offset) +
                      "&timeout=" + std::to_string(timeout);
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_perform(curl);
    curl_easy_cleanup(curl);

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

bool TelegramClient::send_inline_keyboard(long long chat_id, const std::string& text,
                                          const std::vector<std::vector<std::pair<std::string, std::string>>>& buttons) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        LOG_ERROR("Failed to init CURL");
        return false;
    }

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

    json payload;
    payload["chat_id"] = chat_id;
    payload["text"] = text;
    payload["parse_mode"] = "Markdown";
    payload["reply_markup"] = kb;

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
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK) {
        LOG("Inline keyboard sent to " + std::to_string(chat_id));
        return true;
    }

    LOG_ERROR("Failed to send inline keyboard");
    return false;
}

bool TelegramClient::edit_message(long long chat_id, int message_id, const std::string& text,
                                  const std::vector<std::vector<std::pair<std::string, std::string>>>& buttons) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        LOG_ERROR("Failed to init CURL");
        return false;
    }

    json payload;
    payload["chat_id"] = chat_id;
    payload["message_id"] = message_id;
    payload["text"] = text;
    payload["parse_mode"] = "Markdown";

    if (!buttons.empty() && !buttons[0].empty()) {
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
        payload["reply_markup"] = kb;
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
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK) {
        LOG("Message edited for " + std::to_string(chat_id));
        return true;
    }

    LOG_ERROR("Failed to edit message");
    return false;
}

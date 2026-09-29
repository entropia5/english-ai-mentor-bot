#include "logger.h"
#include "telegram_client.h"
#include "telegram_client_internal.h"

#include <nlohmann/json.hpp>
#include <openssl/sha.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

namespace {

// Ключ зависит от содержимого: новая карточка не получит старый file_id.
std::string photo_key(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return {};
    std::ostringstream bytes;
    bytes << file.rdbuf();
    if (file.bad()) return {};
    const auto content = bytes.str();
    unsigned char digest[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(content.data()), content.size(), digest);
    std::ostringstream key;
    for (const auto byte : digest)
        key << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    return key.str();
}

bool rejected_file_id(const TelegramHttpResponse& response) {
    if (!response.transport_ok || response.http_status != 400) return false;
    const auto body = nlohmann::json::parse(response.body, nullptr, false);
    if (body.is_discarded()) return false;
    auto description = body.value("description", std::string{});
    std::transform(description.begin(), description.end(), description.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return description.find("file_id") != std::string::npos ||
           description.find("file identifier") != std::string::npos ||
           description.find("file reference") != std::string::npos ||
           description.find("wrong remote file") != std::string::npos;
}

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

std::string TelegramClient::cached_photo(const std::string& key) {
    std::lock_guard<std::mutex> lock(photo_cache_mutex);
    const auto now = std::chrono::steady_clock::now();
    for (auto it = photo_cache.begin(); it != photo_cache.end();) {
        if (now - it->second.used >= std::chrono::hours(24)) it = photo_cache.erase(it);
        else ++it;
    }
    const auto it = photo_cache.find(key);
    if (it == photo_cache.end()) return {};
    it->second.used = now;
    return it->second.file_id;
}

void TelegramClient::forget_photo(const std::string& key) {
    std::lock_guard<std::mutex> lock(photo_cache_mutex);
    photo_cache.erase(key);
}

void TelegramClient::remember_photo(const std::string& key, const std::string& response) {
    if (key.empty()) return;
    const auto body = nlohmann::json::parse(response, nullptr, false);
    if (body.is_discarded() || !body.value("ok", false) || !body.contains("result") ||
        !body["result"].is_object() || !body["result"].contains("photo")) return;
    const auto& photos = body["result"]["photo"];
    std::string id;
    long long largest = -1;
    for (const auto& photo : photos) {
        const auto area = photo.value("width", 0LL) * photo.value("height", 0LL);
        if (area > largest && photo.contains("file_id")) {
            largest = area;
            id = photo["file_id"].get<std::string>();
        }
    }
    if (id.empty()) return;
    std::lock_guard<std::mutex> lock(photo_cache_mutex);
    photo_cache[key] = {id, std::chrono::steady_clock::now()};
    while (photo_cache.size() > 256) {
        const auto oldest = std::min_element(photo_cache.begin(), photo_cache.end(),
            [](const auto& a, const auto& b) { return a.second.used < b.second.used; });
        photo_cache.erase(oldest);
    }
}

bool TelegramClient::send_photo(
    long long chat_id, const std::string& photo_path,
    const std::vector<std::vector<std::pair<std::string, std::string>>>& buttons,
    const std::string& caption, int* message_id, const std::string& parse_mode) {
    const auto key = photo_key(photo_path);
    const auto id = cached_photo(key);
    if (!id.empty()) {
        nlohmann::json payload = {{"chat_id", chat_id}, {"photo", id},
            {"caption", caption}, {"reply_markup", build_inline_keyboard_json(buttons)}};
        if (!parse_mode.empty()) payload["parse_mode"] = parse_mode;
        const auto cached = telegram_post_json(api_url + "/sendPhoto", payload);
        if (!rejected_file_id(cached)) {
            if (!cached.transport_ok) return false;
            const bool ok = telegram_ok(cached.body, "sendPhoto cached", message_id, cached.http_status);
            if (ok) LOG("Photo reused by file_id");
            return ok;
        }
        forget_photo(key);
    }
    std::vector<TelegramMultipartField> fields = {
        {"chat_id", std::to_string(chat_id), false},
        {"photo", photo_path, true},
    };
    if (!caption.empty()) {
        fields.push_back({"caption", caption, false});
        if (!parse_mode.empty())
            fields.push_back({"parse_mode", parse_mode, false});
    }
    fields.push_back({"reply_markup", build_inline_keyboard_json(buttons).dump(), false});

    const TelegramHttpResponse response = telegram_post_multipart(api_url + "/sendPhoto", fields);
    if (!response.transport_ok) {
        log_media_transport_failure("sendPhoto", response);
        return false;
    }

    const bool ok = telegram_ok(response.body, "sendPhoto", message_id, response.http_status);
    if (ok) {
        if (photo_key(photo_path) == key) remember_photo(key, response.body);
        LOG("Photo sent to " + std::to_string(chat_id));
    }
    return ok;
}

bool TelegramClient::edit_message_photo(
    long long chat_id, int message_id, const std::string& photo_path,
    const std::vector<std::vector<std::pair<std::string, std::string>>>& buttons,
    const std::string& caption, TelegramRequestResult* result, const std::string& parse_mode) {
    nlohmann::json media = {{"type", "photo"}, {"media", "attach://photo"}};
    if (!caption.empty()) {
        media["caption"] = caption;
        if (!parse_mode.empty())
            media["parse_mode"] = parse_mode;
    }
    const auto key = photo_key(photo_path);
    const auto id = cached_photo(key);
    if (!id.empty()) {
        auto cached_media = media;
        cached_media["media"] = id;
        const auto cached = telegram_post_json(api_url + "/editMessageMedia",
            {{"chat_id", chat_id}, {"message_id", message_id}, {"media", cached_media},
             {"reply_markup", build_inline_keyboard_json(buttons)}});
        if (!rejected_file_id(cached)) {
            if (!cached.transport_ok) {
                populate_transport_failure(result, cached);
                return false;
            }
            const bool ok = telegram_ok(cached.body, "editMessageMedia cached", nullptr,
                                        cached.http_status, result);
            if (ok) LOG("Photo edit reused file_id");
            return ok;
        }
        forget_photo(key);
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
        if (photo_key(photo_path) == key) remember_photo(key, response.body);
        LOG("Photo message edited for " + std::to_string(chat_id));
    }
    return ok;
}

#pragma once

#include "telegram_client.h"

#include <cstddef>
#include <nlohmann/json.hpp>
#include <string>

struct TelegramHttpResponse {
    bool transport_ok = false;
    long http_status = 0;
    std::string body;
    std::string error;
};

struct TelegramMultipartField {
    std::string name;
    std::string value;
    bool file = false;
};

std::size_t telegram_write_callback(void* contents, std::size_t size, std::size_t count,
                                    std::string* response);

TelegramHttpResponse telegram_post_json(const std::string& url, const nlohmann::json& payload);
TelegramHttpResponse telegram_post_multipart(const std::string& url,
                                             const std::vector<TelegramMultipartField>& fields);

bool telegram_ok(const std::string& response, const std::string& action, int* message_id = nullptr,
                 long http_status = 0, TelegramRequestResult* result = nullptr);

nlohmann::json build_inline_keyboard_json(
    const std::vector<std::vector<std::pair<std::string, std::string>>>& buttons);

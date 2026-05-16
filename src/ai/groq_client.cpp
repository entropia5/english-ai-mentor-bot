#include "groq_client.h"
#include "logger.h"
#include "config.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t total = size * nmemb;
    response->append((char*)contents, total);
    return total;
}

GroqClient::GroqClient() {
    api_key = g_config.get("GROQ_API_KEY");
    model = g_config.get("GROQ_MODEL", "llama-3.3-70b-versatile");
    api_url = "https://api.groq.com/openai/v1/chat/completions";

    if (api_key.empty()) {
        LOG_WARNING("GROQ_API_KEY not found in .env");
    } else {
        LOG("GroqClient initialized with model: " + model);
    }
}

std::string GroqClient::ask(const std::string& prompt, const std::string& system_prompt) {
    if (api_key.empty()) {
        return "⚠️ API ключ не настроен. Добавьте GROQ_API_KEY в .env файл";
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        LOG_ERROR("Failed to init CURL for Groq");
        return "❌ Ошибка соединения";
    }

    json messages = json::array();

    // set system prompt or default
    std::string sys_prompt = system_prompt.empty() ?
        "Ты опытный преподаватель английского языка. Отвечай полезно, кратко и по делу. "
        "Если просят слова - давай список с переводом на русский и транскрипцией. "
        "Используй русский язык для объяснений, английский для примеров." :
        system_prompt;

    messages.push_back({{"role", "system"}, {"content", sys_prompt}});
    messages.push_back({{"role", "user"}, {"content", prompt}});

    json payload;
    payload["model"] = model;
    payload["messages"] = messages;
    payload["temperature"] = 0.7;
    payload["max_tokens"] = 800;

    std::string post_data = payload.dump();
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, api_url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + api_key).c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res == CURLE_OK) {
        try {
            auto resp_json = json::parse(response);
            if (resp_json.contains("choices") && !resp_json["choices"].empty()) {
                std::string content = resp_json["choices"][0]["message"]["content"];
                LOG("Groq response received, length: " + std::to_string(content.length()));
                return content;
            } else if (resp_json.contains("error")) {
                std::string error = resp_json["error"].value("message", "Unknown error");
                LOG_ERROR("Groq API error: " + error);
                return "⚠️ Ошибка API: " + error;
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Groq parse error: " + std::string(e.what()));
        }
    } else {
        LOG_ERROR("CURL error: " + std::string(curl_easy_strerror(res)));
        return "❌ Ошибка сети. Попробуйте позже.";
    }

    return "❌ Не удалось получить ответ от AI";
}

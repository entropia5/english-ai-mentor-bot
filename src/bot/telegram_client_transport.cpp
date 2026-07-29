#include "telegram_client_internal.h"

#include <curl/curl.h>

std::size_t telegram_write_callback(void* contents, std::size_t size, std::size_t count,
                                    std::string* response) {
    const std::size_t total = size * count;
    response->append(static_cast<char*>(contents), total);
    return total;
}

TelegramHttpResponse telegram_post_json(const std::string& url, const nlohmann::json& payload) {
    TelegramHttpResponse response;
    CURL* curl = curl_easy_init();
    if (curl == nullptr) {
        response.error = "Failed to init CURL";
        return response;
    }

    const std::string post_data = payload.dump();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, telegram_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.body);

    curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    const CURLcode result = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.http_status);
    if (result == CURLE_OK) {
        response.transport_ok = true;
    } else {
        response.error = curl_easy_strerror(result);
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    return response;
}

TelegramHttpResponse telegram_post_multipart(const std::string& url,
                                             const std::vector<TelegramMultipartField>& fields) {
    TelegramHttpResponse response;
    CURL* curl = curl_easy_init();
    if (curl == nullptr) {
        response.error = "Failed to init CURL";
        return response;
    }

    curl_mime* mime = curl_mime_init(curl);
    if (mime == nullptr) {
        response.error = "Failed to init CURL MIME";
        curl_easy_cleanup(curl);
        return response;
    }
    for (const TelegramMultipartField& field : fields) {
        curl_mimepart* part = curl_mime_addpart(mime);
        curl_mime_name(part, field.name.c_str());
        if (field.file) {
            curl_mime_filedata(part, field.value.c_str());
        } else {
            curl_mime_data(part, field.value.c_str(), CURL_ZERO_TERMINATED);
        }
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, telegram_write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response.body);

    const CURLcode result = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.http_status);
    if (result == CURLE_OK) {
        response.transport_ok = true;
    } else {
        response.error = curl_easy_strerror(result);
    }

    curl_mime_free(mime);
    curl_easy_cleanup(curl);
    return response;
}

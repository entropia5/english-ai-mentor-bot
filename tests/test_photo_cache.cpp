#include "telegram_client.h"
#include "telegram_client_internal.h"
#include "rendering/cache_maintenance.h"
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <chrono>
#include <unistd.h>

namespace fs = std::filesystem;
namespace {
int uploads = 0, references = 0;
TelegramHttpResponse next;
nlohmann::json last_payload;
void check(bool ok) { if (!ok) throw std::runtime_error("Photo cache regression"); }
TelegramHttpResponse success() {
    return {true, 200, R"({"ok":true,"result":{"message_id":42,"photo":[{"width":100,"height":100,"file_id":"small"},{"width":1000,"height":1000,"file_id":"large"}]}})", {}};
}
}
TelegramClient::TelegramClient(const std::string& value) : token(value), api_url(value) {}
TelegramHttpResponse telegram_post_json(const std::string&, const nlohmann::json& payload) {
    ++references;
    last_payload = payload;
    if (next.http_status) { auto result = next; next = {}; return result; }
    return success();
}
TelegramHttpResponse telegram_post_multipart(const std::string&, const std::vector<TelegramMultipartField>&) {
    ++uploads;
    return success();
}
int main() {
    const auto dir = fs::temp_directory_path() / ("mentor-cache-test-" + std::to_string(getpid()));
    fs::create_directories(dir);
    try {
        const auto file = (dir / "card.jpg").string();
        std::ofstream(file) << "first";
        TelegramClient bot("bot-one");
        check(bot.send_photo(1, file, {}));
        check(uploads == 1 && references == 0);
        check(bot.edit_message_photo(1, 42, file, {{{"next", "next"}}}, "new caption"));
        check(uploads == 1 && references == 1);
        check(last_payload["media"]["media"] == "large" && last_payload["media"]["caption"] == "new caption");
        check(last_payload["reply_markup"]["inline_keyboard"][0][0]["callback_data"] == "next");
        std::ofstream(file) << "other"; // Тот же путь и размер, другое содержимое.
        check(bot.send_photo(1, file, {}) && uploads == 2);
        next = {true, 400, R"({"ok":false,"error_code":400,"description":"Bad Request: wrong file identifier/HTTP URL specified"})", {}};
        check(bot.edit_message_photo(1, 42, file, {}) && uploads == 3);
        next = {true, 429, R"({"ok":false,"error_code":429,"description":"Too Many Requests"})", {}};
        TelegramRequestResult result;
        check(!bot.edit_message_photo(1, 42, file, {}, "", &result));
        check(uploads == 3 && result.failure_kind == TelegramFailureKind::Temporary);
        next = {false, 503, {}, "timeout"};
        check(!bot.send_photo(1, file, {}) && uploads == 3);
        next = {true, 400, R"({"ok":false,"error_code":400,"description":"Bad Request: message is not modified"})", {}};
        check(bot.edit_message_photo(1, 42, file, {}) && uploads == 3);
        TelegramClient other("bot-two");
        check(other.send_photo(1, file, {}) && uploads == 4);
        for (int i = 0; i < 257; ++i) {
            std::ofstream(file) << i;
            check(bot.send_photo(1, file, {}));
        }
        const int before = uploads;
        std::ofstream(file) << "other";
        check(bot.send_photo(1, file, {}) && uploads == before + 1);
        const auto now = fs::file_time_type::clock::now();
        for (const auto& name : {"old.jpg", "old.hash", "recent.jpg", "recent.hash", "unused.png", "keep.json"}) {
            std::ofstream(dir / name) << "data";
            fs::last_write_time(dir / name, now - std::chrono::hours(24 * 8));
        }
        fs::last_write_time(dir / "recent.hash", now);
        fs::last_write_time(dir / "unused.png", now - std::chrono::hours(2));
        fs::create_symlink(dir / "keep.json", dir / "link.jpg");
        prune_render_cache(dir);
        check(!fs::exists(dir / "old.jpg") && fs::exists(dir / "unused.png"));
        prune_render_cache(dir, 0);
        check(!fs::exists(dir / "old.jpg") && !fs::exists(dir / "old.hash"));
        check(!fs::exists(dir / "unused.png"));
        check(fs::exists(dir / "recent.jpg") && fs::exists(dir / "recent.hash"));
        check(fs::exists(dir / "keep.json") && fs::is_symlink(dir / "link.jpg"));
        fs::remove_all(dir);
    } catch (...) { fs::remove_all(dir); throw; }
}

#include "presentation/bot_state.h"
#include "bot_state_internal.h"
#include <filesystem>
#include <stdexcept>
#include <utility>
#include <unistd.h>

namespace {
std::vector<std::pair<long long, int>> deleted;
bool deletion_ok = true;
void expect(bool ok) { if (!ok) throw std::runtime_error("Export cleanup regression"); }
}
TelegramClient::TelegramClient(const std::string&) {}
bool TelegramClient::delete_message(long long chat, int id) {
    deleted.emplace_back(chat, id);
    return deletion_ok;
}
bool TelegramClient::remove_reply_keyboard(long long) { return true; }

int main() {
    const auto original = std::filesystem::current_path();
    const auto directory = std::filesystem::temp_directory_path() /
                           ("mentor-export-cleanup-" + std::to_string(getpid()));
    std::filesystem::create_directories(directory);
    std::filesystem::current_path(directory);
    try {
        TelegramClient bot("unused");
        remember_export_message(1, 100);
        remember_export_message(2, 200);
        remember_broadcast_hint(1, 50);
        delete_messages_after_delay(bot, 1, {50}, 0);
        process_deferred_message_deletions(bot);
        expect(deleted.size() == 1 && deleted.back().second == 50);
        expect(g_bot_state.export_messages.at(1).front() == 100);
        g_bot_state.export_messages.clear();
        load_bot_state();
        expect(g_bot_state.export_messages.at(1).front() == 100);
        expect(g_bot_state.export_messages.at(2).front() == 200);
        delete_tracked_exports(1, bot);
        expect(deleted.size() == 2 && deleted.back() == std::make_pair(1LL, 100));
        expect(g_bot_state.export_messages.count(1) == 0 && g_bot_state.export_messages.count(2) == 1);
        load_bot_state();
        expect(g_bot_state.export_messages.count(1) == 0);
        deletion_ok = false;
        delete_tracked_exports(2, bot);
        expect(g_bot_state.export_messages.at(2).front() == 200);
        remember_export_message(2, 201);
        load_bot_state();
        expect(g_bot_state.export_messages.at(2).size() == 2);
        deletion_ok = true;
        delete_tracked_exports(2, bot);
        expect(g_bot_state.export_messages.empty());
        expect(deleted.back() == std::make_pair(2LL, 201));
        const auto count = deleted.size();
        delete_tracked_exports(2, bot);
        expect(deleted.size() == count);
        std::filesystem::current_path(original);
        std::filesystem::remove_all(directory);
    } catch (...) {
        std::filesystem::current_path(original);
        std::filesystem::remove_all(directory);
        throw;
    }
}

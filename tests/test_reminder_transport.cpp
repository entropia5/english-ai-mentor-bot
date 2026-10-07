#include "presentation/screen_transport.h"
#include "presentation/bot_state.h"

#include <stdexcept>
#include <algorithm>
#include <vector>

namespace {
int active = 10, sends = 0, edits = 0, deletes = 0;
bool send_ok = true, delete_ok = true, edit_ok = true;
TelegramFailureKind edit_failure = TelegramFailureKind::Temporary;
std::vector<int> pending;
ScreenMessageType type = ScreenMessageType::Photo;
void expect(bool value) {
    if (!value) throw std::runtime_error("Reminder transport regression");
}
}

// Проверяем выбор отправки/редактирования без запросов в Telegram.
TelegramClient::TelegramClient(const std::string&) {}
bool TelegramClient::send_photo(long long, const std::string&, const InlineKeyboard&,
                                const std::string&, int* id, const std::string&) {
    ++sends;
    if (send_ok) *id = 100 + sends;
    return send_ok;
}
bool TelegramClient::send_inline_keyboard(long long, const std::string&,
                                          const InlineKeyboard&, int* id) {
    ++sends;
    if (send_ok) *id = 100 + sends;
    return send_ok;
}
bool TelegramClient::edit_message_photo(long long, int id, const std::string&,
                                        const InlineKeyboard&, const std::string&,
                                        TelegramRequestResult* result, const std::string&) {
    expect(id == active); ++edits;
    if (result) result->failure_kind = edit_failure;
    return edit_ok;
}
bool TelegramClient::edit_message(long long, int id, const std::string&,
                                  const InlineKeyboard&, TelegramRequestResult* result) {
    expect(id == active); ++edits;
    if (result) result->failure_kind = edit_failure;
    return edit_ok;
}
bool TelegramClient::delete_message(long long, int) { ++deletes; return delete_ok; }
int get_active_screen_message(long long) { return active; }
ScreenMessageType get_active_screen_message_type(long long) { return type; }
void remember_active_screen_message(long long, int id, ScreenMessageType value) {
    active = id; type = value;
}
bool has_tracked_reminder(long long, int) { return false; }
void remember_obsolete_screen(long long, int id) {
    if (id > 0 && std::find(pending.begin(), pending.end(), id) == pending.end())
        pending.push_back(id);
}
void finish_reminder_transition(long long chat, TelegramClient& bot, int current, bool) {
    pending.erase(std::remove_if(pending.begin(), pending.end(), [&](int id) {
        return id == current || bot.delete_message(chat, id);
    }), pending.end());
}
void clear_active_screen_message(long long) { active = 0; }

int main() {
    TelegramClient bot("unused");
    expect(upsert_photo_screen(1, bot, "card.jpg", {}, 10, "Morning", "", true));
    expect(sends == 1 && edits == 0 && deletes == 1 && active == 101);
    expect(upsert_photo_screen(1, bot, "card.jpg", {}, active));
    expect(sends == 1 && edits == 1);
    expect(upsert_photo_screen(1, bot, "card.jpg", {}, 0, "Evening", "", true));
    expect(sends == 2 && edits == 1 && deletes == 2 && active == 102);
    send_ok = false;
    expect(!upsert_photo_screen(1, bot, "card.jpg", {}, 0, "Retry", "", true));
    expect(active == 102 && edits == 1 && deletes == 2);
    send_ok = true;
    expect(upsert_screen(1, bot, "Text fallback", {}, 0, true));
    expect(active == 104 && type == ScreenMessageType::Text && edits == 1 && deletes == 3);
    expect(upsert_screen(1, bot, "Updated text", {}, active));
    expect(sends == 4 && edits == 2);
    // A photo-to-text navigation failure must preserve the existing screen.
    type = ScreenMessageType::Photo;
    const int old_active = active;
    send_ok = false;
    expect(!upsert_screen(1, bot, "Menu", {}, active));
    expect(active == old_active && deletes == 3);
    send_ok = true;
    expect(upsert_screen(1, bot, "Menu", {}, active));
    expect(active != old_active && deletes == 4);
    // Temporary edit errors preserve the panel; permanent target loss replaces it.
    const int before_failure = active;
    const int before_sends = sends;
    edit_ok = false;
    expect(!upsert_screen(1, bot, "Retry", {}));
    expect(active == before_failure && sends == before_sends);
    edit_failure = TelegramFailureKind::Permanent;
    delete_ok = false;
    expect(upsert_screen(1, bot, "Recovered", {}));
    expect(active != before_failure && pending == std::vector<int>{before_failure});
    delete_ok = true;
    edit_ok = true;
    expect(upsert_screen(1, bot, "Next", {}));
    expect(pending.empty());

}

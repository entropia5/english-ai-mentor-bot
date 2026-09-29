#include "presentation/screen_transport.h"
#include "presentation/bot_state.h"

#include <stdexcept>

namespace {
int active = 10, sends = 0, edits = 0, deletes = 0;
bool send_ok = true;
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
                                        TelegramRequestResult*, const std::string&) {
    expect(id == active); ++edits; return true;
}
bool TelegramClient::edit_message(long long, int id, const std::string&,
                                  const InlineKeyboard&, TelegramRequestResult*) {
    expect(id == active); ++edits; return true;
}
bool TelegramClient::delete_message(long long, int) { ++deletes; return true; }
int get_active_screen_message(long long) { return active; }
ScreenMessageType get_active_screen_message_type(long long) { return type; }
void remember_active_screen_message(long long, int id, ScreenMessageType value) {
    active = id; type = value;
}
void clear_active_screen_message(long long) { active = 0; }

int main() {
    TelegramClient bot("unused");
    expect(upsert_photo_screen(1, bot, "card.jpg", {}, 10, "Morning", "", true));
    expect(sends == 1 && edits == 0 && deletes == 0 && active == 101);
    expect(upsert_photo_screen(1, bot, "card.jpg", {}, active));
    expect(sends == 1 && edits == 1);
    expect(upsert_photo_screen(1, bot, "card.jpg", {}, 0, "Evening", "", true));
    expect(sends == 2 && edits == 1 && active == 102);
    send_ok = false;
    expect(!upsert_photo_screen(1, bot, "card.jpg", {}, 0, "Retry", "", true));
    expect(active == 102 && edits == 1 && deletes == 0);
    send_ok = true;
    expect(upsert_screen(1, bot, "Text fallback", {}, 0, true));
    expect(active == 104 && type == ScreenMessageType::Text && edits == 1 && deletes == 0);
    expect(upsert_screen(1, bot, "Updated text", {}, active));
    expect(sends == 4 && edits == 2);
}

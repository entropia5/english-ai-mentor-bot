
#pragma once

#include <string>
#include <vector>

enum class TelegramFailureKind {
    None,
    Permanent,
    Temporary
};

struct TelegramRequestResult {
    bool ok = false;
    TelegramFailureKind failure_kind = TelegramFailureKind::None;
    long http_status = 0;
    int api_error_code = 0;
    std::string description;
    std::string response;
};

class TelegramClient {
private:
    std::string token;
    std::string api_url;

public:
    TelegramClient(const std::string& bot_token);

    // send simple message
    bool send_message(long long chat_id, const std::string& text,
                      const std::string& parse_mode = "Markdown",
                      int* message_id = nullptr);

    // get updates (messages, button clicks)
    std::string get_updates(int offset = 0, int timeout = 30);

    // check token validity
    bool test_token();

    // get bot info
    std::string get_me();

    // disable webhook and optionally clear queued updates before polling
    bool delete_webhook(bool drop_pending_updates = false);

    bool send_inline_keyboard(long long chat_id, const std::string& text,
                              const std::vector<std::vector<std::pair<std::string, std::string>>>& buttons,
                              int* message_id = nullptr);

    bool send_photo(long long chat_id, const std::string& photo_path,
                    const std::vector<std::vector<std::pair<std::string, std::string>>>& buttons,
                    const std::string& caption = "",
                    int* message_id = nullptr);

    bool edit_message_photo(long long chat_id, int message_id, const std::string& photo_path,
                            const std::vector<std::vector<std::pair<std::string, std::string>>>& buttons,
                            const std::string& caption = "",
                            TelegramRequestResult* result = nullptr);

    // edit message with inline keyboard
    bool edit_message(long long chat_id, int message_id, const std::string& text,
                      const std::vector<std::vector<std::pair<std::string, std::string>>>& buttons = {},
                      TelegramRequestResult* result = nullptr);

    // stop Telegram callback loading spinner
    bool answer_callback_query(const std::string& callback_query_id);

    // remove old reply keyboard from Telegram input panel
    bool remove_reply_keyboard(long long chat_id);

    // delete bot message
    bool delete_message(long long chat_id, int message_id);

};

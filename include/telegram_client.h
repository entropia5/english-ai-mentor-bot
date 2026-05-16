
#pragma once

#include <string>
#include <vector>
#include <map>

class TelegramClient {
private:
    std::string token;
    std::string api_url;

public:
    TelegramClient(const std::string& bot_token);

    // send simple message
    bool send_message(long long chat_id, const std::string& text,
                      const std::string& parse_mode = "Markdown");

    // send message with custom keyboard
    bool send_message_with_keyboard(long long chat_id, const std::string& text,
                                    const std::vector<std::vector<std::string>>& keyboard,
                                    bool resize = true, bool one_time = false);

    // get updates (messages, button clicks)
    std::string get_updates(int offset = 0, int timeout = 30);

    // check token validity
    bool test_token();

    // get bot info
    std::string get_me();


        bool send_inline_keyboard(long long chat_id, const std::string& text,
                              const std::vector<std::vector<std::pair<std::string, std::string>>>& buttons);


    // edit message with inline keyboard
    bool edit_message(long long chat_id, int message_id, const std::string& text,
                      const std::vector<std::vector<std::pair<std::string, std::string>>>& buttons = {});


};


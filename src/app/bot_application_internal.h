#pragma once

#include <map>
#include <nlohmann/json_fwd.hpp>
#include <string>

class Database;
class GroqClient;
class TelegramClient;

namespace app::detail {

struct SessionState {
    std::map<long long, int> dictionary_page;
    std::map<long long, int> learned_page;
    std::map<long long, int> dictionary_message_id;
    std::map<long long, int> learned_message_id;
    std::map<long long, std::string> last_action;
};

void handle_callback(const nlohmann::json& update, TelegramClient& bot, Database& database,
                     GroqClient& ai, SessionState& state);
void handle_text_message(const nlohmann::json& update, TelegramClient& bot, Database& database,
                         GroqClient& ai, SessionState& state);

} // namespace app::detail

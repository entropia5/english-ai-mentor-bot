#include "telegram_client_internal.h"

nlohmann::json build_inline_keyboard_json(
    const std::vector<std::vector<std::pair<std::string, std::string>>>& buttons) {
    nlohmann::json keyboard;
    keyboard["inline_keyboard"] = nlohmann::json::array();

    for (const auto& row : buttons) {
        nlohmann::json keyboard_row = nlohmann::json::array();
        for (const auto& button : row) {
            keyboard_row.push_back({{"text", button.first}, {"callback_data", button.second}});
        }
        keyboard["inline_keyboard"].push_back(keyboard_row);
    }
    return keyboard;
}

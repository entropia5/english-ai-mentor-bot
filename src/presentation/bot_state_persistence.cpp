#include "bot_state_internal.h"
#include "file_utils.h"
#include "logger.h"
#include "presentation/bot_state.h"

#include <filesystem>
#include <nlohmann/json.hpp>
#include <set>

using json = nlohmann::json;
namespace fs = std::filesystem;

fs::path bot_state_dir() {
    fs::path state_dir = fs::path(project_data_dir()) / "state";
    std::error_code ec;
    fs::create_directories(state_dir, ec);
    if (ec) {
        LOG_ERROR("Failed to create state directory: " + state_dir.string() + "; " + ec.message());
    }
    return state_dir;
}

fs::path bot_live_state_path() {
    return bot_state_dir() / "bot_state.json";
}

void save_bot_state_locked() {
    json state;
    state["chats"] = json::object();

    std::set<long long> chat_ids;
    for (const auto& [chat_id, _] : g_bot_state.active_screen_messages)
        chat_ids.insert(chat_id);
    for (const auto& [chat_id, _] : g_bot_state.broadcast_hint_messages)
        chat_ids.insert(chat_id);
    for (const auto& [chat_id, _] : g_bot_state.chat_languages)
        chat_ids.insert(chat_id);
    for (const auto& [chat_id, _] : g_bot_state.screen_contexts)
        chat_ids.insert(chat_id);

    for (long long chat_id : chat_ids) {
        json chat_state;

        auto live_it = g_bot_state.active_screen_messages.find(chat_id);
        if (live_it != g_bot_state.active_screen_messages.end() && live_it->second.message_id > 0) {
            chat_state["live_dashboard_message_id"] = live_it->second.message_id;
            chat_state["live_dashboard_message_type"] =
                screen_message_type_to_string(live_it->second.type);
        }

        auto alert_it = g_bot_state.broadcast_hint_messages.find(chat_id);
        if (alert_it != g_bot_state.broadcast_hint_messages.end() && alert_it->second > 0) {
            chat_state["last_alert_text_message_id"] = alert_it->second;
        }

        auto language_it = g_bot_state.chat_languages.find(chat_id);
        if (language_it != g_bot_state.chat_languages.end() && !language_it->second.empty()) {
            chat_state["language"] = language_it->second;
        }

        auto context_it = g_bot_state.screen_contexts.find(chat_id);
        if (context_it != g_bot_state.screen_contexts.end() && !context_it->second.empty()) {
            chat_state["screen_context"] = context_it->second;
        }

        state["chats"][std::to_string(chat_id)] = chat_state;
    }

    fs::path state_path = bot_live_state_path();
    if (!write_text_file_atomic(state_path, state.dump(2))) {
        LOG_ERROR("Failed to save bot state: " + state_path.string());
    }
}

void load_bot_state() {
    fs::path state_path = bot_live_state_path();
    std::string state_text = read_text_file(state_path.string());
    if (state_text.empty()) {
        LOG("Bot state file not found or empty: " + state_path.string());
        return;
    }

    try {
        json state = json::parse(state_text);
        std::lock_guard<std::mutex> lock(g_bot_state.mutex);
        g_bot_state.active_screen_messages.clear();
        g_bot_state.broadcast_hint_messages.clear();
        g_bot_state.chat_languages.clear();
        g_bot_state.screen_contexts.clear();

        if (!state.contains("chats") || !state["chats"].is_object()) {
            LOG_WARNING("Bot state has no chats object: " + state_path.string());
            return;
        }

        for (auto it = state["chats"].begin(); it != state["chats"].end(); ++it) {
            long long chat_id = 0;
            try {
                chat_id = std::stoll(it.key());
            } catch (const std::exception& e) {
                LOG_WARNING("Skipping invalid bot state chat id '" + it.key() + "': " + e.what());
                continue;
            }

            const json& chat_state = it.value();
            int live_message_id = chat_state.value("live_dashboard_message_id", 0);
            std::string live_message_type =
                chat_state.value("live_dashboard_message_type", "photo");
            int alert_message_id = chat_state.value("last_alert_text_message_id", 0);
            std::string language = chat_state.value("language", "");
            std::string screen_context = chat_state.value("screen_context", "");

            if (live_message_id > 0) {
                g_bot_state.active_screen_messages[chat_id] = {
                    live_message_id, screen_message_type_from_string(live_message_type)};
            }
            if (alert_message_id > 0) {
                g_bot_state.broadcast_hint_messages[chat_id] = alert_message_id;
            }
            if (!language.empty()) {
                g_bot_state.chat_languages[chat_id] = language;
            }
            if (is_persistable_screen_context(screen_context)) {
                g_bot_state.screen_contexts[chat_id] = screen_context;
            }
        }

        LOG("Loaded bot state for " + std::to_string(g_bot_state.active_screen_messages.size()) +
            " live dashboard chats from " + state_path.string());
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to load bot state " + state_path.string() + ": " + e.what());
    }
}

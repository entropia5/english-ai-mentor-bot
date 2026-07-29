#include "storage/broadcast_run_state.h"

#include "file_utils.h"
#include "logger.h"
#include "runtime_state_internal.h"

#include <filesystem>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::json;

namespace {

std::filesystem::path broadcast_runs_path() {
    return storage::detail::state_directory() / "broadcast_runs.json";
}

json load_broadcast_runs() {
    const std::string text = read_text_file(broadcast_runs_path().string());
    if (text.empty()) {
        return json{{"runs", json::object()}};
    }
    try {
        json state = json::parse(text);
        if (!state.contains("runs") || !state["runs"].is_object()) {
            state["runs"] = json::object();
        }
        return state;
    } catch (const std::exception& error) {
        LOG_ERROR("Failed to parse broadcast state: " + std::string(error.what()));
        return json{{"runs", json::object()}};
    }
}

} // namespace

std::string local_date_key(const std::tm& time) {
    std::stringstream stream;
    stream << std::put_time(&time, "%Y-%m-%d");
    return stream.str();
}

bool broadcast_was_sent(const std::string& date_key, const std::string& kind, long long user_id) {
    const json state = load_broadcast_runs();
    const std::string user_key = std::to_string(user_id);
    return state["runs"].contains(date_key) && state["runs"][date_key].contains(kind) &&
           state["runs"][date_key][kind].contains(user_key) &&
           state["runs"][date_key][kind][user_key].value("sent", false);
}

void remember_broadcast_sent(const std::string& date_key, const std::string& kind,
                             long long user_id) {
    json state = load_broadcast_runs();
    state["runs"][date_key][kind][std::to_string(user_id)] = {{"sent", true},
                                                              {"updated_at", std::time(nullptr)}};
    if (!write_text_file_atomic(broadcast_runs_path(), state.dump(2))) {
        LOG_ERROR("Failed to save broadcast runs state");
    }
}

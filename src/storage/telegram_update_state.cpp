#include "storage/telegram_update_state.h"

#include "file_utils.h"
#include "logger.h"
#include "runtime_state_internal.h"

int load_update_offset() {
    const std::string value = read_text_file(
        (storage::detail::state_directory() / "telegram_update_offset.txt").string());
    if (value.empty()) {
        return 0;
    }
    try {
        return std::stoi(value);
    } catch (const std::exception& error) {
        LOG_ERROR("Failed to parse update offset: " + std::string(error.what()));
        return 0;
    }
}

void save_update_offset(int update_id) {
    if (!write_text_file_atomic(storage::detail::state_directory() / "telegram_update_offset.txt",
                                std::to_string(update_id))) {
        LOG_ERROR("Failed to save Telegram update offset");
    }
}

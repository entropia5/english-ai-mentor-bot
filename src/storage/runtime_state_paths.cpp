#include "file_utils.h"
#include "logger.h"
#include "runtime_state_internal.h"

namespace fs = std::filesystem;

namespace storage::detail {

fs::path state_directory() {
    const fs::path directory = fs::path(project_data_dir()) / "state";
    std::error_code error;
    fs::create_directories(directory, error);
    if (error) {
        LOG_ERROR("Failed to create state directory: " + error.message());
    }
    return directory;
}

} // namespace storage::detail

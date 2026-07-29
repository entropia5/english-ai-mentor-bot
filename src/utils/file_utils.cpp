#include "file_utils.h"

#include "logger.h"

#include <fstream>
#include <sstream>
#include <unistd.h>

namespace fs = std::filesystem;

std::string project_data_dir() {
    if (fs::exists("CMakeLists.txt")) {
        return "data";
    }
    if (fs::exists("../CMakeLists.txt")) {
        return "../data";
    }
    return "data";
}

std::string read_text_file(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        return {};
    }
    std::stringstream buffer;
    buffer << file.rdbuf();

    const std::string content = buffer.str();
    const std::size_t start = content.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return {};
    }
    const std::size_t end = content.find_last_not_of(" \t\r\n");
    return content.substr(start, end - start + 1);
}

bool write_text_file(const std::string& path, const std::string& text) {
    std::ofstream file(path);
    if (!file) {
        return false;
    }
    file << text;
    return static_cast<bool>(file);
}

bool replace_file_atomic(const fs::path& temporary_path, const fs::path& destination,
                         const std::string& log_name) {
    std::error_code error;
    fs::rename(temporary_path, destination, error);
    if (error) {
        std::error_code remove_error;
        fs::remove(destination, remove_error);
        error.clear();
        fs::rename(temporary_path, destination, error);
    }
    if (!error) {
        return true;
    }

    LOG_ERROR("Failed to replace " + log_name + " file " + destination.string() + ": " +
              error.message());
    std::error_code cleanup_error;
    fs::remove(temporary_path, cleanup_error);
    return false;
}

bool write_text_file_atomic(const fs::path& path, const std::string& text) {
    std::error_code error;
    fs::create_directories(path.parent_path(), error);
    if (error) {
        LOG_ERROR("Failed to create directory for " + path.string() + ": " + error.message());
        return false;
    }

    fs::path temporary_path = path;
    temporary_path += ".tmp." + std::to_string(getpid());
    {
        std::ofstream file(temporary_path);
        if (!file) {
            LOG_ERROR("Failed to open temp file: " + temporary_path.string());
            return false;
        }
        file << text;
        if (!file) {
            LOG_ERROR("Failed to write temp file: " + temporary_path.string());
            return false;
        }
    }
    return replace_file_atomic(temporary_path, path, "state");
}

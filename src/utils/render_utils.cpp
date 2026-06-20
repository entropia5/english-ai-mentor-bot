#include "render_utils.h"
#include "logger.h"

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <unistd.h>

namespace fs = std::filesystem;

namespace {

std::string shell_quote(const std::string& text) {
    std::string result = "'";
    for (char c : text) {
        if (c == '\'') {
            result += "'\\''";
        } else {
            result += c;
        }
    }
    result += "'";
    return result;
}

bool replace_file_atomic(const fs::path& tmp_path, const fs::path& path, const std::string& log_name) {
    std::error_code ec;
    fs::rename(tmp_path, path, ec);
    if (ec) {
        std::error_code remove_ec;
        fs::remove(path, remove_ec);
        ec.clear();
        fs::rename(tmp_path, path, ec);
    }

    if (ec) {
        LOG_ERROR("Failed to replace " + log_name + " file " + path.string() + ": " + ec.message());
        std::error_code cleanup_ec;
        fs::remove(tmp_path, cleanup_ec);
        return false;
    }

    return true;
}

} // namespace

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
    if (!file) return "";
    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string result = buffer.str();
    size_t start = result.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = result.find_last_not_of(" \t\r\n");
    return result.substr(start, end - start + 1);
}

bool write_text_file(const std::string& path, const std::string& text) {
    std::ofstream file(path);
    if (!file) return false;
    file << text;
    return static_cast<bool>(file);
}

bool write_text_file_atomic(const fs::path& path, const std::string& text) {
    std::error_code ec;
    fs::create_directories(path.parent_path(), ec);
    if (ec) {
        LOG_ERROR("Failed to create directory for " + path.string() + ": " + ec.message());
        return false;
    }

    fs::path tmp_path = path;
    tmp_path += ".tmp." + std::to_string(getpid());

    {
        std::ofstream file(tmp_path);
        if (!file) {
            LOG_ERROR("Failed to open temp file: " + tmp_path.string());
            return false;
        }
        file << text;
        if (!file) {
            LOG_ERROR("Failed to write temp file: " + tmp_path.string());
            return false;
        }
    }

    return replace_file_atomic(tmp_path, path, "state");
}

std::string render_html_image(const fs::path& base, const std::string& hash_value,
                              const std::string& html_content, const std::string& log_name,
                              int width) {
    std::error_code ec;
    fs::create_directories(base.parent_path(), ec);
    if (ec) {
        LOG_ERROR("Failed to create render directory for " + log_name + ": " +
                  base.parent_path().string() + "; " + ec.message());
        return "";
    }

    fs::path html_path = base;
    html_path += ".html";
    fs::path png_path = base;
    png_path += ".png";
    fs::path hash_path = base;
    hash_path += ".hash";

    if (fs::exists(png_path) && read_text_file(hash_path.string()) == hash_value) {
        LOG("Reusing " + log_name + " image: " + png_path.string());
        return png_path.string();
    }

    if (!write_text_file(html_path.string(), html_content)) {
        LOG_ERROR("Failed to write " + log_name + " HTML: " + html_path.string());
        return "";
    }

    fs::path tmp_png_path = png_path.parent_path() /
                            (png_path.stem().string() + ".tmp." +
                             std::to_string(getpid()) + png_path.extension().string());

    std::string command = "timeout 45s wkhtmltoimage --quiet --width " +
                          std::to_string(width) + " --disable-smart-width " +
                          shell_quote(html_path.string()) + " " + shell_quote(tmp_png_path.string());
    int rc = std::system(command.c_str());
    if (rc != 0) {
        LOG_ERROR("wkhtmltoimage " + log_name + " failed with code " + std::to_string(rc));
        std::error_code cleanup_ec;
        fs::remove(tmp_png_path, cleanup_ec);
        return "";
    }

    if (!fs::exists(tmp_png_path) || fs::file_size(tmp_png_path, ec) == 0 || ec) {
        LOG_ERROR("wkhtmltoimage " + log_name + " produced an empty image: " + tmp_png_path.string());
        std::error_code cleanup_ec;
        fs::remove(tmp_png_path, cleanup_ec);
        return "";
    }

    if (!replace_file_atomic(tmp_png_path, png_path, log_name)) {
        return "";
    }

    if (!write_text_file_atomic(hash_path, hash_value)) {
        LOG_ERROR("Failed to write " + log_name + " image hash: " + hash_path.string());
    }

    return png_path.string();
}

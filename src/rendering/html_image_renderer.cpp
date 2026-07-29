#include "rendering/html_image_renderer.h"

#include "file_utils.h"
#include "logger.h"

#include <cstdlib>
#include <unistd.h>

namespace fs = std::filesystem;

namespace {

std::string shell_quote(const std::string& text) {
    std::string result = "'";
    for (const char character : text) {
        if (character == '\'') {
            result += "'\\''";
        } else {
            result += character;
        }
    }
    return result + "'";
}

} // namespace

std::string render_html_image(const fs::path& base, const std::string& hash_value,
                              const std::string& html_content, const std::string& log_name,
                              int width) {
    std::error_code error;
    fs::create_directories(base.parent_path(), error);
    if (error) {
        LOG_ERROR("Failed to create render directory for " + log_name + ": " +
                  base.parent_path().string() + "; " + error.message());
        return {};
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
        return {};
    }

    const fs::path temporary_png =
        png_path.parent_path() / (png_path.stem().string() + ".tmp." + std::to_string(getpid()) +
                                  png_path.extension().string());
    const std::string command = "timeout 45s wkhtmltoimage --quiet --width " +
                                std::to_string(width) + " --disable-smart-width " +
                                shell_quote(html_path.string()) + " " +
                                shell_quote(temporary_png.string());
    const int result = std::system(command.c_str());
    if (result != 0) {
        LOG_ERROR("wkhtmltoimage " + log_name + " failed with code " + std::to_string(result));
        std::error_code cleanup_error;
        fs::remove(temporary_png, cleanup_error);
        return {};
    }

    if (!fs::exists(temporary_png) || fs::file_size(temporary_png, error) == 0 || error) {
        LOG_ERROR("wkhtmltoimage " + log_name +
                  " produced an empty image: " + temporary_png.string());
        std::error_code cleanup_error;
        fs::remove(temporary_png, cleanup_error);
        return {};
    }
    if (!replace_file_atomic(temporary_png, png_path, log_name)) {
        return {};
    }
    if (!write_text_file_atomic(hash_path, hash_value)) {
        LOG_ERROR("Failed to write " + log_name + " image hash: " + hash_path.string());
    }
    return png_path.string();
}

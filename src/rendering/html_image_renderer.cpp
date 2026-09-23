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
    fs::path jpeg_path = base;
    jpeg_path += ".jpg";
    const std::string render_hash = "photo-size-v1:" + hash_value;
    constexpr std::uintmax_t max_photo_bytes = 9 * 1024 * 1024;
    const auto usable_photo = [&](const fs::path& path) {
        std::error_code size_error;
        const auto size = fs::file_size(path, size_error);
        return !size_error && size > 0 && size <= max_photo_bytes;
    };
    fs::path hash_path = base;
    hash_path += ".hash";
    if (read_text_file(hash_path.string()) == render_hash) {
        for (const auto& cached : {png_path, jpeg_path}) {
            if (usable_photo(cached)) {
                LOG("Reusing " + log_name + " image: " + cached.string());
                return cached.string();
            }
        }
    }

    if (!write_text_file(html_path.string(), html_content)) {
        LOG_ERROR("Failed to write " + log_name + " HTML: " + html_path.string());
        return {};
    }

    fs::path output_path = png_path;
    fs::path temporary_image;
    const auto render = [&](const fs::path& destination) {
        temporary_image = destination.parent_path() /
                          (destination.stem().string() + ".tmp." + std::to_string(getpid()) +
                           destination.extension().string());
        const std::string command = "timeout 45s wkhtmltoimage --quiet --width " +
                                    std::to_string(width) + " --disable-smart-width --quality 92 " +
                                    shell_quote(html_path.string()) + " " +
                                    shell_quote(temporary_image.string());
        const int result = std::system(command.c_str());
        if (result != 0) {
            LOG_ERROR("wkhtmltoimage " + log_name + " failed with code " + std::to_string(result));
            std::error_code cleanup_error;
            fs::remove(temporary_image, cleanup_error);
            return false;
        }
        return true;
    };
    if (!render(output_path))
        return {};
    // wkhtmltoimage can emit uncompressed PNGs larger than Telegram's photo limit.
    // Keep small PNGs; render large cards as high-quality JPEG at the same resolution.
    if (!usable_photo(temporary_image)) {
        fs::remove(temporary_image, error);
        output_path = jpeg_path;
        if (!render(output_path))
            return {};
    }
    if (!usable_photo(temporary_image)) {
        LOG_ERROR("Rendered photo is empty or exceeds the size limit: " + log_name);
        fs::remove(temporary_image, error);
        return {};
    }
    if (!replace_file_atomic(temporary_image, output_path, log_name))
        return {};
    fs::remove(output_path == png_path ? jpeg_path : png_path, error);
    if (!write_text_file_atomic(hash_path, render_hash)) {
        LOG_ERROR("Failed to write " + log_name + " image hash: " + hash_path.string());
    }
    return output_path.string();
}

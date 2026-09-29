#include "file_utils.h"
#include "rendering/html_image_renderer.h"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

int main() {
    namespace fs = std::filesystem;
    const auto directory =
        fs::temp_directory_path() / ("mentor-photo-test-" + std::to_string(getpid()));
    try {
        const auto base = directory / "page";
        const std::string html =
            "<html><body style='margin:0;width:1440px;height:2000px;background:#182028'>"
            "<h1 style='color:white'>Dictionary size regression</h1></body></html>";
        const auto first = render_html_image(base, "large", html, "size regression", 1440);
        if (first.empty() || fs::path(first).extension() != ".jpg" ||
            fs::file_size(first) >= 9 * 1024 * 1024)
            throw std::runtime_error("Large PNG did not fall back to a sendable JPEG");
        const auto modified = fs::last_write_time(first);
        const auto cached = render_html_image(base, "large", html, "cache regression", 1440);
        if (cached != first || fs::last_write_time(cached) != modified)
            throw std::runtime_error("Compressed photo was not reused from cache");
        const auto small = render_html_image(
            base, "small", "<html><body style='margin:0;height:100px'>Small card</body></html>",
            "small regression", 1080);
        if (small.empty() || fs::path(small).extension() != ".jpg" ||
            read_text_file(base.string() + ".hash").find("small") == std::string::npos)
            throw std::runtime_error("Changed content reused a stale JPEG");
        fs::remove_all(directory);
        std::cout << "Photo size fallback and cache checks passed\n";
        return 0;
    } catch (const std::exception& error) {
        fs::remove_all(directory);
        std::cerr << error.what() << '\n';
        return 1;
    }
}

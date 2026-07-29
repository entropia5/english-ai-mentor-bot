#include "template_resources.h"

#include "config.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

namespace fs = std::filesystem;

namespace rendering::resources {

std::string read_file(const fs::path& path) {
    std::ifstream file(path);
    if (!file) {
        return {};
    }

    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

std::string load_styles(const fs::path& resources_dir) {
    const fs::path styles_dir = resources_dir / "styles";
    std::vector<fs::path> styles;
    std::error_code error;
    for (const auto& entry : fs::directory_iterator(styles_dir, error)) {
        if (!error && entry.is_regular_file() && entry.path().extension() == ".css") {
            styles.push_back(entry.path());
        }
    }
    std::sort(styles.begin(), styles.end());

    std::string theme;
    for (const fs::path& style : styles) {
        const std::string content = read_file(style);
        if (content.empty()) {
            return {};
        }
        theme += "\n/* " + style.filename().string() + " */\n" + content;
    }
    return theme;
}

fs::path locate() {
    const std::string configured = g_config.get("RENDER_RESOURCES_DIR", "");
    if (!configured.empty()) {
        return configured;
    }

    const std::vector<fs::path> candidates = {
        fs::path("resources") / "rendering",
        fs::path("..") / "resources" / "rendering",
        fs::path("/usr/local/share/english_mentor/resources/rendering"),
        fs::path("/usr/share/english_mentor/resources/rendering"),
    };
    for (const fs::path& candidate : candidates) {
        if (fs::exists(candidate / "styles") && fs::exists(candidate / "templates")) {
            return candidate;
        }
    }
    return candidates.front();
}

} // namespace rendering::resources

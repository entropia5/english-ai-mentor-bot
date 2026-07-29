#include "bot_renderer_internal.h"
#include "file_utils.h"
#include "logger.h"
#include "rendering/bot_renderer.h"

#include <filesystem>
#include <set>
#include <string>

namespace fs = std::filesystem;

namespace {

void remove_render_artifact_set(const fs::path& base_path) {
    for (const char* extension : {".png", ".html", ".hash"}) {
        std::error_code error;
        fs::remove(base_path.string() + extension, error);
    }
}

} // namespace

namespace rendering::detail {

void cleanup_stale_page_artifacts(const fs::path& render_dir, int total_pages) {
    if (total_pages < 1 || !fs::exists(render_dir)) {
        return;
    }

    std::set<int> stale_pages;
    std::error_code error;
    for (const auto& entry : fs::directory_iterator(render_dir, error)) {
        if (error || !entry.is_regular_file()) {
            continue;
        }

        const std::string stem = entry.path().stem().string();
        constexpr const char* prefix = "page_";
        if (stem.rfind(prefix, 0) != 0) {
            continue;
        }

        try {
            const int page_number = std::stoi(stem.substr(std::char_traits<char>::length(prefix)));
            if (page_number > total_pages) {
                stale_pages.insert(page_number);
            }
        } catch (const std::exception&) {
        }
    }

    for (const int page_number : stale_pages) {
        remove_render_artifact_set(render_dir / ("page_" + std::to_string(page_number)));
    }
}

void cleanup_legacy_stats_artifacts(const fs::path& stats_dir) {
    if (!fs::exists(stats_dir)) {
        return;
    }

    std::error_code error;
    for (const auto& entry : fs::directory_iterator(stats_dir, error)) {
        if (!error && entry.is_regular_file() &&
            entry.path().stem().string().rfind("stats_", 0) == 0) {
            std::error_code remove_error;
            fs::remove(entry.path(), remove_error);
        }
    }
}

} // namespace rendering::detail

bool cleanup_render_cache() {
    const fs::path rendered_dir = fs::path(project_data_dir()) / "rendered";
    if (!fs::exists(rendered_dir)) {
        LOG("Render cache directory does not exist: " + rendered_dir.string());
        return true;
    }

    int removed = 0;
    std::error_code error;
    for (const auto& entry : fs::recursive_directory_iterator(rendered_dir, error)) {
        if (error || !entry.is_regular_file()) {
            continue;
        }

        const fs::path& path = entry.path();
        const std::string filename = path.filename().string();
        const std::string stem = path.stem().string();
        const bool should_remove = filename.find(".tmp.") != std::string::npos ||
                                   stem.rfind("stats_", 0) == 0 ||
                                   (path.parent_path().filename() == "menu" && stem == "main_menu");
        if (should_remove) {
            std::error_code remove_error;
            fs::remove(path, remove_error);
            if (!remove_error) {
                ++removed;
            }
        }
    }

    LOG("Render cache cleanup complete, removed " + std::to_string(removed) + " files");
    return !error;
}

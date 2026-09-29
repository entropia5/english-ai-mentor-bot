#include "rendering/cache_maintenance.h"
#include "file_utils.h"
#include "logger.h"

#include <algorithm>
#include <chrono>
#include <map>
#include <mutex>
#include <vector>

namespace fs = std::filesystem;

std::mutex& render_cache_mutex() { static std::mutex mutex; return mutex; }

void prune_render_cache(const fs::path& directory, std::uintmax_t budget) {
    std::lock_guard<std::mutex> lock(render_cache_mutex());
    struct Group {
        std::vector<fs::path> files;
        fs::file_time_type used = fs::file_time_type::min();
        std::uintmax_t bytes = 0;
    };
    std::error_code ec;
    if (fs::is_symlink(directory, ec) || !fs::is_directory(directory, ec)) return;
    std::map<fs::path, Group> grouped;
    std::uintmax_t total = 0;
    for (fs::recursive_directory_iterator it(directory, fs::directory_options::skip_permission_denied, ec), end;
         it != end && !ec; it.increment(ec)) {
        if (it->is_symlink(ec) || !it->is_regular_file(ec)) continue;
        const auto path = it->path();
        const auto ext = path.extension().string();
        if (ext != ".html" && ext != ".hash" && ext != ".png" && ext != ".jpg") continue;
        auto base = path;
        base.replace_extension();
        const auto size = it->file_size(ec);
        if (ec) { ec.clear(); continue; }
        const auto modified = it->last_write_time(ec);
        if (ec) { ec.clear(); continue; }
        auto& group = grouped[base];
        group.files.push_back(path);
        group.bytes += size;
        group.used = std::max(group.used, modified);
        total += size;
    }
    std::vector<Group> groups;
    for (auto& item : grouped) groups.push_back(std::move(item.second));
    std::sort(groups.begin(), groups.end(), [](const auto& a, const auto& b) { return a.used < b.used; });
    const auto now = fs::file_time_type::clock::now();
    int removed = 0;
    for (const auto& group : groups) {
        // Отправка ограничена 35 секундами; час защищает файлы текущих запросов.
        if (now - group.used < std::chrono::hours(1)) continue;
        if (now - group.used < std::chrono::hours(24 * 7) && total <= budget) continue;
        for (const auto& file : group.files) {
            const auto size = fs::file_size(file, ec);
            if (ec) { ec.clear(); continue; }
            if (fs::remove(file, ec)) { total -= size; ++removed; }
            ec.clear();
        }
    }
    if (removed) LOG("Render cache maintenance removed " + std::to_string(removed) + " files");
}

void maintain_render_cache() {
    static std::mutex mutex;
    static auto next = std::chrono::steady_clock::time_point::min();
    std::lock_guard<std::mutex> lock(mutex);
    const auto now = std::chrono::steady_clock::now();
    if (now < next) return;
    next = now + std::chrono::hours(1);
    prune_render_cache(fs::path(project_data_dir()) / "rendered");
}

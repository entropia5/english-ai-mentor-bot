#include "bot_renderer_internal.h"
#include "core/text.h"
#include "file_utils.h"
#include "rendering/bot_renderer.h"
#include "rendering/html_image_renderer.h"

#include <algorithm>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace {

std::string safe_render_key(std::string text) {
    text = to_lower_ascii(text);
    std::string result;
    for (const char character : text) {
        if ((character >= 'a' && character <= 'z') || (character >= '0' && character <= '9')) {
            result += character;
        } else if (character == '_' || character == '-' || character == ' ') {
            result += '_';
        }
    }
    return result.empty() ? "status" : result;
}

} // namespace

namespace rendering::detail {

std::string render_screen(const fs::path& base, const std::string& template_name,
                          const TemplateValues& values, const std::string& data_fingerprint,
                          const std::string& log_name, int width) {
    const RenderedTemplate rendered = render_template(template_name, values);
    if (!rendered) {
        return {};
    }
    return render_html_image(base, rendered.fingerprint + "-" + data_fingerprint, rendered.html,
                             log_name, width);
}

} // namespace rendering::detail

std::string render_main_menu_image(const std::string& user_level) {
    const fs::path base =
        fs::path(project_data_dir()) / "rendered" / "menu" / ("main_menu_" + user_level);
    return rendering::detail::render_screen(base, "main_menu.html", {{"user_level", user_level}},
                                            user_level, "main menu");
}

std::string render_ai_prompt_image() {
    const fs::path base = fs::path(project_data_dir()) / "rendered" / "ai" / "prompt";
    return rendering::detail::render_screen(base, "ai_prompt.html", {}, "static", "AI prompt");
}

std::string render_topic_menu_image() {
    const fs::path base = fs::path(project_data_dir()) / "rendered" / "topics" / "topic_menu";
    return rendering::detail::render_screen(base, "topic_menu.html", {}, "static", "topic menu");
}

std::string render_status_image(const std::string& key, const std::string& title,
                                const std::string& subtitle, const std::string& note) {
    const fs::path base =
        fs::path(project_data_dir()) / "rendered" / "status" / safe_render_key(key);
    return rendering::detail::render_screen(
        base, "status.html", {{"title", title}, {"subtitle", subtitle}, {"note", note}},
        title + "|" + subtitle + "|" + note, "status " + key);
}

std::string render_stats_image(long long chat_id, int total, int learned, const std::string& level,
                               const std::string& next_name, int next_level, int percent) {
    const fs::path stats_dir = fs::path(project_data_dir()) / "rendered" / "stats";
    rendering::detail::cleanup_legacy_stats_artifacts(stats_dir);

    const int clamped_percent = std::clamp(percent, 0, 100);
    const std::string next_text =
        next_level > 0 ? "До " + next_name + " осталось " + std::to_string(next_level) + " слов"
                       : "Уровень C1 достигнут";
    const TemplateValues values = {
        {"level", level},
        {"total", std::to_string(total)},
        {"learned", std::to_string(learned)},
        {"percent", std::to_string(clamped_percent)},
        {"next_text", next_text},
    };
    const std::string data_fingerprint = std::to_string(total) + "-" + std::to_string(learned) +
                                         "-" + level + "-" + std::to_string(next_level) + "-" +
                                         std::to_string(clamped_percent);
    return rendering::detail::render_screen(stats_dir / std::to_string(chat_id) / "stats",
                                            "stats.html", values, data_fingerprint, "stats");
}

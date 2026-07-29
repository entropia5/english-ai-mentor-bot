#pragma once

#include "rendering/template_engine.h"

#include <filesystem>
#include <string>

namespace rendering::detail {

std::string render_screen(const std::filesystem::path& base, const std::string& template_name,
                          const TemplateValues& values, const std::string& data_fingerprint,
                          const std::string& log_name, int width = 1080);

void cleanup_stale_page_artifacts(const std::filesystem::path& render_dir, int total_pages);
void cleanup_legacy_stats_artifacts(const std::filesystem::path& stats_dir);

} // namespace rendering::detail

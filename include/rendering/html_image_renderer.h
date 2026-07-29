#pragma once

#include <filesystem>
#include <string>

std::string render_html_image(const std::filesystem::path& base, const std::string& hash_value,
                              const std::string& html_content, const std::string& log_name,
                              int width = 1080);

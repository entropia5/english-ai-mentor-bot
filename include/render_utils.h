#pragma once

#include <filesystem>
#include <string>

std::string project_data_dir();

std::string read_text_file(const std::string& path);
bool write_text_file(const std::string& path, const std::string& text);
bool write_text_file_atomic(const std::filesystem::path& path, const std::string& text);

std::string render_html_image(const std::filesystem::path& base,
                              const std::string& hash_value,
                              const std::string& html_content,
                              const std::string& log_name,
                              int width = 1080);

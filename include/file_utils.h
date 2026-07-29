#pragma once

#include <filesystem>
#include <string>

std::string project_data_dir();
std::string read_text_file(const std::string& path);
bool write_text_file(const std::string& path, const std::string& text);
bool write_text_file_atomic(const std::filesystem::path& path, const std::string& text);
bool replace_file_atomic(const std::filesystem::path& temporary_path,
                         const std::filesystem::path& destination, const std::string& log_name);

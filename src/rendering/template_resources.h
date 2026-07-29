#pragma once

#include <filesystem>
#include <string>

namespace rendering::resources {

std::filesystem::path locate();
std::string read_file(const std::filesystem::path& path);
std::string load_styles(const std::filesystem::path& resources_dir);

} // namespace rendering::resources

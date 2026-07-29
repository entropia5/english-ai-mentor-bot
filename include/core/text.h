#pragma once

#include <string>
#include <vector>

std::string trim(const std::string& text);
std::string to_lower_ascii(const std::string& text);
std::vector<std::string> split_words_input(const std::string& text);
std::string clean_markdown(const std::string& text);
std::string format_ai_response_box(const std::string& text);

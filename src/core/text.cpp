#include "core/text.h"

#include <algorithm>
#include <cctype>
#include <sstream>

std::string trim(const std::string& text) {
    const auto start = text.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return {};
    }

    const auto end = text.find_last_not_of(" \t\r\n");
    return text.substr(start, end - start + 1);
}

std::string to_lower_ascii(const std::string& text) {
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char character) {
        return static_cast<char>(std::tolower(character));
    });
    return result;
}

std::vector<std::string> split_words_input(const std::string& text) {
    std::vector<std::string> words;
    std::stringstream stream(text);
    std::string item;

    while (std::getline(stream, item, ',')) {
        item = trim(item);
        if (!item.empty()) {
            words.push_back(std::move(item));
        }
    }
    return words;
}

std::string clean_markdown(const std::string& text) {
    std::string result;
    bool in_code_block = false;

    for (std::size_t index = 0; index < text.length(); ++index) {
        const char character = text[index];
        if (index + 2 < text.length() && text.substr(index, 3) == "```") {
            in_code_block = !in_code_block;
            result += character;
            continue;
        }
        if (in_code_block) {
            result += character;
            continue;
        }
        if (character == '*' || character == '_' || character == '`' || character == '[' ||
            character == ']' || character == '(' || character == ')' || character == '~' ||
            character == '>' || character == '#' || character == '+' || character == '-' ||
            character == '=' || character == '|' || character == '{' || character == '}') {
            result += ' ';
        } else if (character == '\n' || character == '\r') {
            result += '\n';
        } else {
            result += character;
        }
    }
    return result;
}

std::string format_ai_response_box(const std::string& text) {
    std::string result;
    result.reserve(text.size() + 12);

    for (const char character : text) {
        if (character == '`') {
            result += '\'';
        } else if (character == '\r') {
            result += '\n';
        } else {
            result += character;
        }
    }

    result = trim(result);
    if (result.empty()) {
        result = "AI не вернул текст ответа.";
    }
    return "```cpp\n" + result + "\n```";
}

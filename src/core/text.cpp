#include "core/text.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <unordered_map>

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

std::vector<std::string> match_words_input(const std::string& text,
                                          const std::vector<std::string>& dictionary) {
    auto normalize = [](const std::string& value) {
        auto result = to_lower_ascii(trim(value));
        while (!result.empty() && std::string("?.!").find(result.back()) != std::string::npos)
            result.pop_back();
        return result;
    };
    std::unordered_map<std::string, std::string> known;
    std::size_t max_length = 0;
    for (const auto& word : dictionary) {
        const auto key = normalize(word);
        known.emplace(key, word);
        max_length = std::max(max_length, key.size());
    }
    std::string separated = text;
    for (char& c : separated)
        if (c == '\n' || c == '\r' || c == ';') c = ',';
    std::vector<std::string> result;
    for (const auto& part : split_words_input(separated)) {
        std::istringstream stream(part);
        std::vector<std::string> tokens;
        std::string token;
        while (stream >> token) tokens.push_back(token);
        for (std::size_t i = 0; i < tokens.size();) {
            std::size_t matched_end = i;
            std::string matched;
            std::string candidate;
            // Сначала ищем самое длинное словосочетание из словаря.
            for (std::size_t j = i; j < tokens.size(); ++j) {
                if (!candidate.empty()) candidate += ' ';
                candidate += tokens[j];
                const auto key = normalize(candidate);
                if (key.size() > max_length) break;
                const auto found = known.find(key);
                if (found != known.end()) {
                    matched = found->second;
                    matched_end = j + 1;
                }
            }
            if (matched_end > i) {
                if (std::find(result.begin(), result.end(), matched) == result.end())
                    result.push_back(matched);
                i = matched_end;
            } else {
                ++i;
            }
        }
    }
    return result;
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

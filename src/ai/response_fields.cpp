#include "response_fields.h"

#include "core/text.h"

#include <cctype>
#include <sstream>

namespace {

std::string strip_list_prefix(std::string line) {
    line = trim(line);
    while (!line.empty() && (line.front() == '-' || line.front() == '*')) {
        line = trim(line.substr(1));
    }

    std::size_t position = 0;
    while (position < line.size() && std::isdigit(static_cast<unsigned char>(line[position]))) {
        ++position;
    }
    if (position > 0 && position + 1 < line.size() &&
        (line[position] == '.' || line[position] == ')')) {
        line = trim(line.substr(position + 1));
    }
    return line;
}

} // namespace

namespace ai::detail {

std::vector<ResponseField> parse_response_fields(const std::string& response) {
    std::vector<ResponseField> fields;
    std::stringstream stream(response);
    std::string line;
    while (std::getline(stream, line)) {
        line = strip_list_prefix(line);
        const std::size_t colon = line.find(':');
        if (colon == std::string::npos) {
            continue;
        }
        fields.push_back(
            {to_lower_ascii(trim(line.substr(0, colon))), trim(line.substr(colon + 1))});
    }
    return fields;
}

} // namespace ai::detail

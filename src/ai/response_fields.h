#pragma once

#include <string>
#include <vector>

namespace ai::detail {

struct ResponseField {
    std::string label;
    std::string value;
};

std::vector<ResponseField> parse_response_fields(const std::string& response);

} // namespace ai::detail

#pragma once

#include <algorithm>
#include <cstddef>

namespace presentation::detail {

struct PageSlice {
    int page;
    int total_pages;
    int start;
    int end;
};

inline PageSlice paginate(std::size_t item_count, int requested_page, int per_page = 5) {
    const int count = static_cast<int>(item_count);
    const int total_pages = std::max(1, (count + per_page - 1) / per_page);
    const int page = std::clamp(requested_page, 0, total_pages - 1);
    const int start = page * per_page;
    return {page, total_pages, start, std::min(start + per_page, count)};
}

} // namespace presentation::detail

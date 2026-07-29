#include "services/maintenance_service.h"

#include "database.h"
#include "logger.h"
#include "services/vocabulary_service.h"

#include <iostream>
#include <string>
#include <vector>

namespace {

std::vector<std::string> suspicious_words_for_cleanup() {
    return {blocked_generated_words().begin(), blocked_generated_words().end()};
}

} // namespace

bool audit_bad_words(Database& database, bool remove_words) {
    const auto rows = database.find_words_by_normalized_english(suspicious_words_for_cleanup());
    if (rows.empty()) {
        std::cout << "Suspicious words: 0\n";
        LOG("Suspicious word audit complete: 0 rows");
        return true;
    }

    std::cout << "Suspicious words: " << rows.size() << '\n';
    std::vector<int> ids;
    ids.reserve(rows.size());
    for (const auto& row : rows) {
        ids.push_back(row.id);
        std::cout << "id=" << row.id << " user=" << row.user_id << " word=" << row.english
                  << " learned=" << (row.is_learned ? "true" : "false")
                  << " translation=" << row.translation << '\n';
    }

    if (!remove_words) {
        LOG("Suspicious word audit complete: " + std::to_string(rows.size()) + " rows");
        return true;
    }

    const int removed = database.delete_words_by_ids(ids);
    std::cout << "Removed suspicious words: " << removed << '\n';
    return removed >= 0;
}

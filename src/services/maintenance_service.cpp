#include "services/maintenance_service.h"

#include "core/text.h"
#include "database.h"
#include "logger.h"
#include "services/course_catalog.h"
#include "services/vocabulary_service.h"

#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace {

std::vector<std::string> suspicious_words_for_cleanup() {
    std::set<std::string> approved;
    for (const std::string course : {"conversation", "medicine", "it"}) {
        for (const auto& word : load_course_catalog(course))
            approved.insert(to_lower_ascii(word.english));
    }
    std::vector<std::string> suspicious;
    for (const auto& word : blocked_generated_words()) {
        if (!approved.count(word))
            suspicious.push_back(word);
    }
    return suspicious;
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

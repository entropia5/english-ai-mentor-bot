#include "services/vocabulary_service.h"

#include "database.h"
#include "logger.h"
#include "services/course_catalog.h"

WordGenerationResult add_generated_words_to_db(long long chat_id, Database& db, GroqClient&,
                                               const std::string& topic_keyword, int target_count) {
    WordGenerationResult result;
    const auto course = canonical_course(topic_keyword);
    try {
        const auto catalog = load_course_catalog(course);
        const int added = db.add_course_words(chat_id, course, catalog, target_count);
        if (added < 0)
            result.failed = 1;
        else {
            result.added = added;
            result.exhausted = added < target_count;
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Course selection failed: " + std::string(e.what()));
        result.failed = 1;
    }
    return result;
}

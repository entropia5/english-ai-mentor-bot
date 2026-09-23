#include "core/text.h"
#include "database.h"
#include "logger.h"

#include <set>

std::string Database::get_active_course(long long user_id) {
    if (!connected)
        return "conversation";
    try {
        pqxx::work tx(*conn);
        const auto rows =
            tx.exec_params("SELECT active_course FROM users WHERE user_id = $1", user_id);
        tx.commit();
        if (!rows.empty())
            return rows[0][0].as<std::string>();
    } catch (const std::exception& e) {
        LOG_ERROR("get_active_course: " + std::string(e.what()));
    }
    return "conversation";
}

bool Database::set_active_course(long long user_id, const std::string& course) {
    if (!connected || course.empty() || canonical_course(course) != course)
        return false;
    try {
        pqxx::work tx(*conn);
        tx.exec_params("INSERT INTO users(user_id, active_course) VALUES ($1, $2) "
                       "ON CONFLICT (user_id) DO UPDATE SET active_course = $2, "
                       "new_words_page = 0, learned_words_page = 0",
                       user_id, course);
        tx.commit();
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("set_active_course: " + std::string(e.what()));
        return false;
    }
}

int Database::add_course_words(long long user_id, const std::string& course,
                               const std::vector<CourseWord>& catalog, int count) {
    if (!connected || course.empty() || canonical_course(course) != course || count <= 0)
        return -1;
    try {
        pqxx::work tx(*conn);
        // Serialize a user's manual and scheduled additions in one transaction.
        tx.exec_params("SELECT pg_advisory_xact_lock($1::bigint)", user_id);
        tx.exec_params("INSERT INTO users(user_id) VALUES ($1) ON CONFLICT DO NOTHING", user_id);
        const auto rows = tx.exec_params("SELECT lower(trim(english)) FROM words "
                                         "WHERE user_id = $1 AND topic = $2",
                                         user_id, course);
        std::set<std::string> existing;
        for (const auto& row : rows)
            existing.insert(row[0].as<std::string>());
        const auto next = select_course_words(catalog, existing, count);
        for (const auto& word : next) {
            tx.exec_params("INSERT INTO words(user_id, english, translation_ru, transcription, "
                           "pronunciation_ru, definition_ru, topic) VALUES ($1,$2,$3,$4,$5,$6,$7)",
                           user_id, word.english, word.translation, word.transcription,
                           word.pronunciation, word.lesson + "\nПример: " + word.example, course);
        }
        tx.commit();
        return static_cast<int>(next.size());
    } catch (const std::exception& e) {
        LOG_ERROR("add_course_words: " + std::string(e.what()));
        return -1;
    }
}

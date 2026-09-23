#include "database.h"
#include "logger.h"

bool Database::add_word(long long user_id, const std::string& english,
                        const std::string& translation, const std::string& pronunciation,
                        const std::string& transcription, const std::string& topic,
                        const std::string& definition) {
    if (!connected) {
        return false;
    }

    try {
        pqxx::work transaction(*conn);
        transaction.exec_params("INSERT INTO users (user_id, name, last_active) VALUES ($1, '', "
                                "EXTRACT(EPOCH FROM NOW())) "
                                "ON CONFLICT (user_id) DO NOTHING",
                                user_id);

        const pqxx::result existing =
            transaction.exec_params("SELECT 1 FROM words WHERE user_id = $1 AND "
                                    "lower(trim(english)) = lower(trim($2)) AND topic = $3 LIMIT 1",
                                    user_id, english, topic);
        if (!existing.empty()) {
            transaction.commit();
            LOG("Word skipped as duplicate: " + english + " for user " + std::to_string(user_id));
            return false;
        }

        transaction.exec_params(
            "INSERT INTO words (user_id, english, translation_ru, pronunciation_ru, "
            "transcription, topic, definition_ru, added_date) "
            "VALUES ($1, $2, $3, $4, $5, $6, $7, EXTRACT(EPOCH FROM NOW()))",
            user_id, english, translation, pronunciation, transcription, topic, definition);
        transaction.commit();
        LOG("Word added: " + english + " for user " + std::to_string(user_id));
        return true;
    } catch (const std::exception& error) {
        LOG_ERROR("Failed to add word: " + std::string(error.what()));
        return false;
    }
}

bool Database::mark_word_learned(long long user_id, const std::string& english,
                                 const std::string& filter) {
    if (!connected) {
        return false;
    }

    const auto course = filter.empty() ? get_active_course(user_id) : filter;
    try {
        pqxx::work transaction(*conn);
        transaction.exec_params("INSERT INTO users (user_id, name, last_active) VALUES ($1, '', "
                                "EXTRACT(EPOCH FROM NOW())) "
                                "ON CONFLICT (user_id) DO NOTHING",
                                user_id);
        const pqxx::result updated = transaction.exec_params(
            "UPDATE words SET is_learned = true, last_repetition = EXTRACT(EPOCH FROM NOW()) "
            "WHERE user_id = $1 AND rtrim(lower(trim(english)), '?.!') = rtrim(lower(trim($2)), "
            "'?.!') AND ($3 = 'all' OR topic = $3) "
            "RETURNING id",
            user_id, english, course);
        transaction.commit();
        if (updated.empty()) {
            LOG("Word not marked as learned because it was not found: " + english + " for user " +
                std::to_string(user_id));
            return false;
        }
        LOG("Word marked as learned: " + english + " for user " + std::to_string(user_id));
        return true;
    } catch (const std::exception& error) {
        LOG_ERROR("Failed to mark word learned: " + std::string(error.what()));
        return false;
    }
}

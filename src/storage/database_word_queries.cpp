#include "database.h"
#include "logger.h"

#include <utility>

std::vector<std::tuple<std::string, std::string, bool>>
Database::get_user_words(long long user_id, bool only_not_learned) {
    std::vector<std::tuple<std::string, std::string, bool>> words;
    if (!connected) {
        return words;
    }

    try {
        pqxx::work transaction(*conn);
        std::string query =
            "SELECT english, translation_ru, is_learned FROM words WHERE user_id = $1";
        if (only_not_learned) {
            query += " AND is_learned = false";
        }
        query += " ORDER BY added_date DESC LIMIT 50";

        const pqxx::result result = transaction.exec_params(query, user_id);
        transaction.commit();
        for (const auto& row : result) {
            words.emplace_back(row[0].as<std::string>(), row[1].as<std::string>(),
                               row[2].as<bool>());
        }
    } catch (const std::exception& error) {
        LOG_ERROR("Failed to get user words: " + std::string(error.what()));
    }
    return words;
}

bool Database::word_exists(long long user_id, const std::string& english) {
    if (!connected) {
        return false;
    }

    try {
        pqxx::work transaction(*conn);
        const pqxx::result result =
            transaction.exec_params("SELECT 1 FROM words WHERE user_id = $1 AND "
                                    "lower(trim(english)) = lower(trim($2)) LIMIT 1",
                                    user_id, english);
        transaction.commit();
        return !result.empty();
    } catch (const std::exception& error) {
        LOG_ERROR("word_exists failed: " + std::string(error.what()));
        return false;
    }
}

int Database::get_words_count(long long user_id, bool learned) {
    if (!connected) {
        return 0;
    }

    try {
        pqxx::work transaction(*conn);
        const pqxx::result result = transaction.exec_params(
            "SELECT COUNT(*) FROM words WHERE user_id = $1 AND is_learned = $2", user_id, learned);
        transaction.commit();
        if (!result.empty()) {
            return result[0][0].as<int>();
        }
    } catch (const std::exception& error) {
        LOG_ERROR("get_words_count failed: " + std::string(error.what()));
    }
    return 0;
}

std::vector<WordView> Database::get_user_words_full(long long user_id, bool only_not_learned) {
    std::vector<WordView> words;
    if (!connected) {
        return words;
    }

    try {
        pqxx::work transaction(*conn);
        std::string query = "SELECT english, translation_ru, is_learned, pronunciation_ru, "
                            "transcription, definition_ru FROM words WHERE user_id = $1";
        if (only_not_learned) {
            query += " AND is_learned = false";
        }
        query += " ORDER BY added_date DESC";

        const pqxx::result result = transaction.exec_params(query, user_id);
        transaction.commit();
        for (const auto& row : result) {
            std::string english = row[0].as<std::string>();
            std::string translation = row[1].as<std::string>();
            const bool learned = row[2].as<bool>();
            std::string pronunciation = row[3].is_null() ? "" : row[3].as<std::string>();
            std::string transcription = row[4].is_null() ? "" : row[4].as<std::string>();
            std::string definition = row[5].is_null() ? "" : row[5].as<std::string>();
            words.push_back(Word{std::move(english), std::move(translation), learned,
                                 std::move(pronunciation), std::move(transcription),
                                 std::move(definition)});
        }

        LOG("Found " + std::to_string(words.size()) + " words for user " + std::to_string(user_id));
    } catch (const std::exception& error) {
        LOG_ERROR("Failed to get user words full: " + std::string(error.what()));
    }
    return words;
}

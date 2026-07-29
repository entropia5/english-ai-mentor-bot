#include "database.h"
#include "logger.h"

std::vector<std::tuple<int, std::string, std::string>>
Database::get_words_missing_pronunciation(int limit) {
    std::vector<std::tuple<int, std::string, std::string>> words;
    if (!connected) {
        return words;
    }

    try {
        pqxx::work transaction(*conn);
        const pqxx::result result = transaction.exec_params(R"(
            SELECT id, english, COALESCE(translation_ru, '')
            FROM words
            WHERE NULLIF(trim(COALESCE(transcription, '')), '') IS NULL
               OR NULLIF(trim(COALESCE(pronunciation_ru, '')), '') IS NULL
            ORDER BY added_date ASC, id ASC
            LIMIT $1
        )",
                                                            limit);
        transaction.commit();
        for (const auto& row : result) {
            words.emplace_back(row[0].as<int>(), row[1].as<std::string>(),
                               row[2].as<std::string>());
        }
    } catch (const std::exception& error) {
        LOG_ERROR("get_words_missing_pronunciation failed: " + std::string(error.what()));
    }
    return words;
}

std::vector<std::tuple<int, std::string, std::string>>
Database::get_words_missing_definition(int limit) {
    std::vector<std::tuple<int, std::string, std::string>> words;
    if (!connected) {
        return words;
    }

    try {
        pqxx::work transaction(*conn);
        const pqxx::result result = transaction.exec_params(R"(
            SELECT id, english, COALESCE(translation_ru, '')
            FROM words
            WHERE NULLIF(trim(COALESCE(definition_ru, '')), '') IS NULL
            ORDER BY added_date ASC, id ASC
            LIMIT $1
        )",
                                                            limit);
        transaction.commit();
        for (const auto& row : result) {
            words.emplace_back(row[0].as<int>(), row[1].as<std::string>(),
                               row[2].as<std::string>());
        }
    } catch (const std::exception& error) {
        LOG_ERROR("get_words_missing_definition failed: " + std::string(error.what()));
    }
    return words;
}

bool Database::update_word_pronunciation(int word_id, const std::string& transcription,
                                         const std::string& pronunciation_ru) {
    if (!connected || (transcription.empty() && pronunciation_ru.empty())) {
        return false;
    }

    try {
        pqxx::work transaction(*conn);
        transaction.exec_params(R"(
            UPDATE words
            SET transcription = CASE
                    WHEN $2 <> '' THEN $2
                    ELSE transcription
                END,
                pronunciation_ru = CASE
                    WHEN $3 <> '' THEN $3
                    ELSE pronunciation_ru
                END
            WHERE id = $1
        )",
                                word_id, transcription, pronunciation_ru);
        transaction.commit();
        return true;
    } catch (const std::exception& error) {
        LOG_ERROR("update_word_pronunciation failed: " + std::string(error.what()));
        return false;
    }
}

bool Database::update_word_definition(int word_id, const std::string& definition_ru) {
    if (!connected || definition_ru.empty()) {
        return false;
    }

    try {
        pqxx::work transaction(*conn);
        transaction.exec_params("UPDATE words SET definition_ru = $2 WHERE id = $1", word_id,
                                definition_ru);
        transaction.commit();
        return true;
    } catch (const std::exception& error) {
        LOG_ERROR("update_word_definition failed: " + std::string(error.what()));
        return false;
    }
}

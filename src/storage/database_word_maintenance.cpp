#include "database.h"
#include "logger.h"

bool Database::cleanup_duplicate_words() {
    if (!connected) {
        LOG_ERROR("Cannot cleanup duplicates: not connected");
        return false;
    }

    try {
        pqxx::work transaction(*conn);
        const pqxx::result removed = transaction.exec(R"(
            WITH ranked AS (
                SELECT
                    id,
                    ROW_NUMBER() OVER (
                        PARTITION BY user_id, lower(trim(english))
                        ORDER BY is_learned DESC, added_date ASC, id ASC
                    ) AS rn
                FROM words
            )
            DELETE FROM words w
            USING ranked r
            WHERE w.id = r.id AND r.rn > 1
            RETURNING w.id
        )");
        transaction.exec(R"(
            CREATE UNIQUE INDEX IF NOT EXISTS idx_words_user_english_unique
            ON words(user_id, lower(trim(english)))
        )");
        transaction.commit();
        LOG("Duplicate word cleanup complete, removed " + std::to_string(removed.size()) + " rows");
        return true;
    } catch (const std::exception& error) {
        LOG_ERROR("cleanup_duplicate_words failed: " + std::string(error.what()));
        return false;
    }
}

std::vector<WordAuditRow>
Database::find_words_by_normalized_english(const std::vector<std::string>& english_words) {
    std::vector<WordAuditRow> rows;
    if (!connected || english_words.empty()) {
        return rows;
    }

    try {
        pqxx::work transaction(*conn);
        for (const std::string& english : english_words) {
            const pqxx::result result = transaction.exec_params(R"(
                SELECT id, user_id, english, COALESCE(translation_ru, ''), is_learned
                FROM words
                WHERE lower(trim(english)) = lower(trim($1))
                ORDER BY user_id, id
            )",
                                                                english);
            for (const auto& row : result) {
                rows.push_back({row[0].as<int>(), row[1].as<long long>(), row[2].as<std::string>(),
                                row[3].as<std::string>(), row[4].as<bool>()});
            }
        }
        transaction.commit();
    } catch (const std::exception& error) {
        LOG_ERROR("find_words_by_normalized_english failed: " + std::string(error.what()));
    }
    return rows;
}

int Database::delete_words_by_ids(const std::vector<int>& word_ids) {
    if (!connected || word_ids.empty()) {
        return 0;
    }

    try {
        pqxx::work transaction(*conn);
        int removed = 0;
        for (const int id : word_ids) {
            const pqxx::result result =
                transaction.exec_params("DELETE FROM words WHERE id = $1 RETURNING id", id);
            removed += static_cast<int>(result.size());
        }
        transaction.commit();
        LOG("Deleted suspicious words: " + std::to_string(removed));
        return removed;
    } catch (const std::exception& error) {
        LOG_ERROR("delete_words_by_ids failed: " + std::string(error.what()));
        return -1;
    }
}

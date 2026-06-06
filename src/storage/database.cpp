
#include "database.h"
#include "logger.h"
#include "config.h"
#include <sstream>

Database::Database() {
    std::string host = g_config.get("DB_HOST", "localhost");
    std::string port = g_config.get("DB_PORT", "5432");
    std::string dbname = g_config.get("DB_NAME", "english_mentor");
    std::string user = g_config.get("DB_USER", "n8n");
    std::string password = g_config.get("DB_PASSWORD", "");

    connection_string = "host=" + host + " port=" + port +
                        " dbname=" + dbname + " user=" + user;
    if (!password.empty()) {
        connection_string += " password=" + password;
    }
}

Database::~Database() {
    disconnect();
}

bool Database::connect() {
    try {
        conn = std::make_unique<pqxx::connection>(connection_string);
        if (conn->is_open()) {
            connected = true;
            LOG("Connected to PostgreSQL database: " + g_config.get("DB_NAME"));
            return true;
        } else {
            LOG_ERROR("Failed to open database connection");
            return false;
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Database connection error: " + std::string(e.what()));
        return false;
    }
}

void Database::disconnect() {
    if (conn && conn->is_open()) {
        conn->disconnect();
    }
    connected = false;
    conn.reset();
}

bool Database::test_connection() {
    if (!connected) return false;
    try {
        pqxx::work txn(*conn);
        pqxx::result res = txn.exec("SELECT 1");
        txn.commit();
        return !res.empty();
    } catch (const std::exception& e) {
        LOG_ERROR("Test connection failed: " + std::string(e.what()));
        return false;
    }
}

bool Database::init_tables() {
    if (!connected) {
        LOG_ERROR("Cannot init tables: not connected");
        return false;
    }

    try {
        pqxx::work txn(*conn);

        txn.exec(R"(
            CREATE TABLE IF NOT EXISTS users (
                user_id BIGINT PRIMARY KEY,
                name VARCHAR(100),
                level INTEGER DEFAULT 1,
                streak_days INTEGER DEFAULT 0,
                last_active BIGINT DEFAULT 0,
                last_daily_sent BIGINT DEFAULT 0,
                new_words_page INTEGER DEFAULT 0,
                learned_words_page INTEGER DEFAULT 0,
                last_viewed_dict VARCHAR(10) DEFAULT 'new',
                created_at BIGINT DEFAULT EXTRACT(EPOCH FROM NOW())
            )
        )");

        txn.exec(R"(
            CREATE TABLE IF NOT EXISTS words (
                id SERIAL PRIMARY KEY,
                user_id BIGINT REFERENCES users(user_id) ON DELETE CASCADE,
                english VARCHAR(255) NOT NULL,
                transcription VARCHAR(255),
                pronunciation_ru VARCHAR(255),
                translation_ru TEXT,
                repetition_count INTEGER DEFAULT 0,
                last_repetition BIGINT DEFAULT 0,
                next_review BIGINT DEFAULT 0,
                is_learned BOOLEAN DEFAULT FALSE,
                level INTEGER DEFAULT 1,
                topic VARCHAR(100) DEFAULT 'general',
                added_date BIGINT DEFAULT EXTRACT(EPOCH FROM NOW()),
                correct_in_row INTEGER DEFAULT 0
            )
        )");

        txn.exec(R"(
            CREATE TABLE IF NOT EXISTS conversations (
                id SERIAL PRIMARY KEY,
                user_id BIGINT REFERENCES users(user_id) ON DELETE CASCADE,
                role VARCHAR(10),
                content TEXT,
                timestamp BIGINT DEFAULT EXTRACT(EPOCH FROM NOW())
            )
        )");

        txn.exec("CREATE INDEX IF NOT EXISTS idx_words_user_learned ON words(user_id, is_learned)");
        txn.exec("CREATE INDEX IF NOT EXISTS idx_words_next_review ON words(next_review)");
        txn.exec("CREATE INDEX IF NOT EXISTS idx_words_user_english_lower ON words(user_id, lower(english))");
        txn.exec("CREATE INDEX IF NOT EXISTS idx_conversations_user_time ON conversations(user_id, timestamp)");

        txn.commit();
        cleanup_duplicate_words();
        LOG("Database tables initialized successfully");
        return true;

    } catch (const std::exception& e) {
        LOG_ERROR("Failed to init tables: " + std::string(e.what()));
        return false;
    }
}

bool Database::cleanup_duplicate_words() {
    if (!connected) {
        LOG_ERROR("Cannot cleanup duplicates: not connected");
        return false;
    }

    try {
        pqxx::work txn(*conn);

        pqxx::result removed = txn.exec(R"(
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

        txn.exec(R"(
            CREATE UNIQUE INDEX IF NOT EXISTS idx_words_user_english_unique
            ON words(user_id, lower(trim(english)))
        )");

        txn.commit();
        LOG("Duplicate word cleanup complete, removed " + std::to_string(removed.size()) + " rows");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("cleanup_duplicate_words failed: " + std::string(e.what()));
        return false;
    }
}

std::vector<std::tuple<int, std::string, std::string>> Database::get_words_missing_pronunciation(int limit) {
    std::vector<std::tuple<int, std::string, std::string>> words;
    if (!connected) return words;

    try {
        pqxx::work txn(*conn);
        pqxx::result res = txn.exec_params(R"(
            SELECT id, english, COALESCE(translation_ru, '')
            FROM words
            WHERE NULLIF(trim(COALESCE(transcription, '')), '') IS NULL
               OR NULLIF(trim(COALESCE(pronunciation_ru, '')), '') IS NULL
            ORDER BY added_date ASC, id ASC
            LIMIT $1
        )", limit);
        txn.commit();

        for (const auto& row : res) {
            words.push_back(std::make_tuple(
                row[0].as<int>(),
                row[1].as<std::string>(),
                row[2].as<std::string>()
            ));
        }
    } catch (const std::exception& e) {
        LOG_ERROR("get_words_missing_pronunciation failed: " + std::string(e.what()));
    }

    return words;
}

bool Database::update_word_pronunciation(int word_id, const std::string& transcription,
                                         const std::string& pronunciation_ru) {
    if (!connected) return false;
    if (transcription.empty() && pronunciation_ru.empty()) return false;

    try {
        pqxx::work txn(*conn);
        txn.exec_params(R"(
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
        )", word_id, transcription, pronunciation_ru);
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("update_word_pronunciation failed: " + std::string(e.what()));
        return false;
    }
}

bool Database::user_exists(long long user_id) {
    if (!connected) return false;

    try {
        pqxx::work txn(*conn);
        pqxx::result res = txn.exec_params(
            "SELECT 1 FROM users WHERE user_id = $1 LIMIT 1",
            user_id
        );
        txn.commit();
        return !res.empty();
    } catch (const std::exception& e) {
        LOG_ERROR("user_exists failed: " + std::string(e.what()));
        return false;
    }
}

bool Database::add_user(long long user_id, const std::string& name) {
    if (!connected) return false;

    try {
        pqxx::work txn(*conn);
        txn.exec_params(
            "INSERT INTO users (user_id, name, last_active) VALUES ($1, $2, EXTRACT(EPOCH FROM NOW())) "
            "ON CONFLICT (user_id) DO UPDATE SET name = $2",
            user_id, name
        );
        txn.commit();
        LOG("User added/updated: " + std::to_string(user_id) + " (" + name + ")");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("add_user failed: " + std::string(e.what()));
        return false;
    }
}

int Database::get_user_level(long long user_id) {
    if (!connected) return 1;

    try {
        pqxx::work txn(*conn);
        pqxx::result res = txn.exec_params(
            "SELECT level FROM users WHERE user_id = $1",
            user_id
        );
        txn.commit();
        if (!res.empty()) {
            return res[0][0].as<int>();
        }
    } catch (const std::exception& e) {
        LOG_ERROR("get_user_level failed: " + std::string(e.what()));
    }
    return 1;
}

void Database::update_user_level(long long user_id, int level) {
    if (!connected) return;

    try {
        pqxx::work txn(*conn);
        txn.exec_params(
            "UPDATE users SET level = $2 WHERE user_id = $1",
            user_id, level
        );
        txn.commit();
        LOG("User level updated: " + std::to_string(user_id) + " -> " + std::to_string(level));
    } catch (const std::exception& e) {
        LOG_ERROR("update_user_level failed: " + std::string(e.what()));
    }
}

bool Database::save_conversation(long long user_id, const std::string& role, const std::string& content) {
    if (!connected) {
        LOG_ERROR("Cannot save conversation: not connected");
        return false;
    }

    try {
        pqxx::work txn(*conn);
        txn.exec_params(
            "INSERT INTO users (user_id, name, last_active) VALUES ($1, '', EXTRACT(EPOCH FROM NOW())) "
            "ON CONFLICT (user_id) DO UPDATE SET last_active = EXTRACT(EPOCH FROM NOW())",
            user_id
        );

        txn.exec_params(
            "INSERT INTO conversations (user_id, role, content, timestamp) "
            "VALUES ($1, $2, $3, EXTRACT(EPOCH FROM NOW()))",
            user_id, role, content
        );
        txn.commit();
        LOG("Conversation saved for user " + std::to_string(user_id));
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to save conversation: " + std::string(e.what()));
        return false;
    }
}

std::vector<std::pair<std::string, std::string>> Database::get_conversation_history(long long user_id, int limit) {
    std::vector<std::pair<std::string, std::string>> history;

    if (!connected) {
        LOG_ERROR("Cannot get history: not connected");
        return history;
    }

    try {
        pqxx::work txn(*conn);
        pqxx::result res = txn.exec_params(
            "SELECT role, content FROM conversations "
            "WHERE user_id = $1 ORDER BY timestamp ASC LIMIT $2",
            user_id, limit
        );
        txn.commit();

        for (const auto& row : res) {
            std::string role = row[0].as<std::string>();
            std::string content = row[1].as<std::string>();
            history.push_back({role, content});
        }

        LOG("Loaded " + std::to_string(history.size()) + " history entries for user " + std::to_string(user_id));
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to get conversation history: " + std::string(e.what()));
    }

    return history;
}



bool Database::add_word(long long user_id, const std::string& english, const std::string& translation,
                        const std::string& pronunciation, const std::string& transcription,
                        const std::string& topic) {
    if (!connected) return false;

    try {
        pqxx::work txn(*conn);

        txn.exec_params(
            "INSERT INTO users (user_id, name, last_active) VALUES ($1, '', EXTRACT(EPOCH FROM NOW())) "
            "ON CONFLICT (user_id) DO NOTHING",
            user_id
        );

        pqxx::result existing = txn.exec_params(
            "SELECT 1 FROM words WHERE user_id = $1 AND lower(trim(english)) = lower(trim($2)) LIMIT 1",
            user_id, english
        );
        if (!existing.empty()) {
            txn.commit();
            LOG("Word skipped as duplicate: " + english + " for user " + std::to_string(user_id));
            return false;
        }

        txn.exec_params(
            "INSERT INTO words (user_id, english, translation_ru, pronunciation_ru, transcription, topic, added_date) "
            "VALUES ($1, $2, $3, $4, $5, $6, EXTRACT(EPOCH FROM NOW()))",
            user_id, english, translation, pronunciation, transcription, topic
        );
        txn.commit();
        LOG("Word added: " + english + " for user " + std::to_string(user_id));
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to add word: " + std::string(e.what()));
        return false;
    }
}

std::vector<std::tuple<std::string, std::string, bool>> Database::get_user_words(long long user_id, bool only_not_learned) {
    std::vector<std::tuple<std::string, std::string, bool>> words;
    if (!connected) return words;

    try {
        pqxx::work txn(*conn);
        std::string query = "SELECT english, translation_ru, is_learned FROM words WHERE user_id = $1";
        if (only_not_learned) {
            query += " AND is_learned = false";
        }
        query += " ORDER BY added_date DESC LIMIT 50";

        pqxx::result res = txn.exec_params(query, user_id);
        txn.commit();

        for (const auto& row : res) {
            std::string eng = row[0].as<std::string>();
            std::string trans = row[1].as<std::string>();
            bool learned = row[2].as<bool>();
            words.push_back(std::make_tuple(eng, trans, learned));
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to get user words: " + std::string(e.what()));
    }
    return words;
}

bool Database::mark_word_learned(long long user_id, const std::string& english) {
    if (!connected) return false;

    try {
        pqxx::work txn(*conn);
        txn.exec_params(
            "INSERT INTO users (user_id, name, last_active) VALUES ($1, '', EXTRACT(EPOCH FROM NOW())) "
            "ON CONFLICT (user_id) DO NOTHING",
            user_id
        );

        txn.exec_params(
            "UPDATE words SET is_learned = true, last_repetition = EXTRACT(EPOCH FROM NOW()) "
            "WHERE user_id = $1 AND lower(trim(english)) = lower(trim($2))",
            user_id, english
        );
        txn.commit();
        LOG("Word marked as learned: " + english + " for user " + std::to_string(user_id));
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to mark word learned: " + std::string(e.what()));
        return false;
    }
}

bool Database::word_exists(long long user_id, const std::string& english) {
    if (!connected) return false;

    try {
        pqxx::work txn(*conn);
        txn.exec_params(
            "INSERT INTO users (user_id, name, last_active) VALUES ($1, '', EXTRACT(EPOCH FROM NOW())) "
            "ON CONFLICT (user_id) DO NOTHING",
            user_id
        );
        txn.commit();

        pqxx::work txn2(*conn);
        pqxx::result res = txn2.exec_params(
            "SELECT 1 FROM words WHERE user_id = $1 AND lower(trim(english)) = lower(trim($2)) LIMIT 1",
            user_id, english
        );
        txn2.commit();
        return !res.empty();
    } catch (const std::exception& e) {
        LOG_ERROR("word_exists failed: " + std::string(e.what()));
        return false;
    }
}

int Database::get_words_count(long long user_id, bool learned) {
    if (!connected) return 0;

    try {
        pqxx::work txn(*conn);
        pqxx::result res = txn.exec_params(
            "SELECT COUNT(*) FROM words WHERE user_id = $1 AND is_learned = $2",
            user_id, learned
        );
        txn.commit();
        if (!res.empty()) {
            return res[0][0].as<int>();
        }
    } catch (const std::exception& e) {
        LOG_ERROR("get_words_count failed: " + std::string(e.what()));
    }
    return 0;
}

std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>> Database::get_user_words_full(long long user_id, bool only_not_learned) {
    std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>> words;
    if (!connected) return words;

    try {
        pqxx::work txn(*conn);
        std::string query = "SELECT english, translation_ru, is_learned, pronunciation_ru, transcription FROM words WHERE user_id = $1";
        if (only_not_learned) {
            query += " AND is_learned = false";
        }
        query += " ORDER BY added_date DESC";

        pqxx::result res = txn.exec_params(query, user_id);
        txn.commit();

        for (const auto& row : res) {
            std::string eng = row[0].as<std::string>();
            std::string trans = row[1].as<std::string>();
            bool learned = row[2].as<bool>();
            std::string pron = row[3].is_null() ? "" : row[3].as<std::string>();
            std::string transcr = row[4].is_null() ? "" : row[4].as<std::string>();

            words.push_back(std::make_tuple(eng, trans, learned, pron, transcr));
        }

        LOG("Found " + std::to_string(words.size()) + " words for user " + std::to_string(user_id));
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to get user words full: " + std::string(e.what()));
    }
    return words;
}

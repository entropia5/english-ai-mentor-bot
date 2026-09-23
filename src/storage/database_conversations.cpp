#include "database.h"
#include "logger.h"

bool Database::save_conversation(long long user_id, const std::string& role,
                                 const std::string& content) {
    if (!connected) {
        LOG_ERROR("Cannot save conversation: not connected");
        return false;
    }

    try {
        pqxx::work transaction(*conn);
        transaction.exec_params(
            "INSERT INTO users (user_id, name, last_active) VALUES ($1, '', EXTRACT(EPOCH FROM "
            "NOW())) "
            "ON CONFLICT (user_id) DO UPDATE SET last_active = EXTRACT(EPOCH FROM NOW())",
            user_id);
        transaction.exec_params("INSERT INTO conversations (user_id, role, content, timestamp) "
                                "VALUES ($1, $2, $3, EXTRACT(EPOCH FROM NOW()))",
                                user_id, role, content);
        transaction.commit();
        LOG("Conversation saved for user " + std::to_string(user_id));
        return true;
    } catch (const std::exception& error) {
        LOG_ERROR("Failed to save conversation: " + std::string(error.what()));
        return false;
    }
}

std::vector<std::pair<std::string, std::string>>
Database::get_conversation_history(long long user_id, int limit) {
    std::vector<std::pair<std::string, std::string>> history;
    if (!connected) {
        LOG_ERROR("Cannot get history: not connected");
        return history;
    }

    try {
        pqxx::work transaction(*conn);
        const pqxx::result result = transaction.exec_params(
            "SELECT role, content FROM (SELECT id, role, content FROM conversations "
            "WHERE user_id = $1 ORDER BY id DESC LIMIT $2) recent ORDER BY id ASC",
            user_id, limit);
        transaction.commit();
        for (const auto& row : result) {
            history.emplace_back(row[0].as<std::string>(), row[1].as<std::string>());
        }
        LOG("Loaded " + std::to_string(history.size()) + " history entries for user " +
            std::to_string(user_id));
    } catch (const std::exception& error) {
        LOG_ERROR("Failed to get conversation history: " + std::string(error.what()));
    }
    return history;
}

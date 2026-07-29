#include "database.h"
#include "logger.h"

bool Database::user_exists(long long user_id) {
    if (!connected) {
        return false;
    }

    try {
        pqxx::work transaction(*conn);
        const pqxx::result result =
            transaction.exec_params("SELECT 1 FROM users WHERE user_id = $1 LIMIT 1", user_id);
        transaction.commit();
        return !result.empty();
    } catch (const std::exception& error) {
        LOG_ERROR("user_exists failed: " + std::string(error.what()));
        return false;
    }
}

bool Database::add_user(long long user_id, const std::string& name) {
    if (!connected) {
        return false;
    }

    try {
        pqxx::work transaction(*conn);
        transaction.exec_params("INSERT INTO users (user_id, name, last_active) VALUES ($1, $2, "
                                "EXTRACT(EPOCH FROM NOW())) "
                                "ON CONFLICT (user_id) DO UPDATE SET name = $2",
                                user_id, name);
        transaction.commit();
        LOG("User added/updated: " + std::to_string(user_id) + " (" + name + ")");
        return true;
    } catch (const std::exception& error) {
        LOG_ERROR("add_user failed: " + std::string(error.what()));
        return false;
    }
}

int Database::get_user_level(long long user_id) {
    if (!connected) {
        return 1;
    }

    try {
        pqxx::work transaction(*conn);
        const pqxx::result result =
            transaction.exec_params("SELECT level FROM users WHERE user_id = $1", user_id);
        transaction.commit();
        if (!result.empty()) {
            return result[0][0].as<int>();
        }
    } catch (const std::exception& error) {
        LOG_ERROR("get_user_level failed: " + std::string(error.what()));
    }
    return 1;
}

void Database::update_user_level(long long user_id, int level) {
    if (!connected) {
        return;
    }

    try {
        pqxx::work transaction(*conn);
        transaction.exec_params("UPDATE users SET level = $2 WHERE user_id = $1", user_id, level);
        transaction.commit();
        LOG("User level updated: " + std::to_string(user_id) + " -> " + std::to_string(level));
    } catch (const std::exception& error) {
        LOG_ERROR("update_user_level failed: " + std::string(error.what()));
    }
}

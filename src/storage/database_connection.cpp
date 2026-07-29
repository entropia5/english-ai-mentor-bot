#include "config.h"
#include "database.h"
#include "logger.h"

Database::Database() {
    std::string host = g_config.get("DB_HOST", "localhost");
    std::string port = g_config.get("DB_PORT", "5432");
    std::string dbname = g_config.get("DB_NAME", "english_mentor");
    std::string user = g_config.get("DB_USER", "n8n");
    std::string password = g_config.get("DB_PASSWORD", "");

    connection_string = "host=" + host + " port=" + port + " dbname=" + dbname + " user=" + user;
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
    connected = false;
    conn.reset();
}

bool Database::test_connection() {
    if (!connected) {
        return false;
    }
    try {
        pqxx::work transaction(*conn);
        const pqxx::result result = transaction.exec("SELECT 1");
        transaction.commit();
        return !result.empty();
    } catch (const std::exception& error) {
        LOG_ERROR("Test connection failed: " + std::string(error.what()));
        return false;
    }
}

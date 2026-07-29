#include "config.h"
#include "database.h"
#include "logger.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace fs = std::filesystem;

namespace {

fs::path find_migrations_directory() {
    std::vector<fs::path> candidates;
    const std::string configured_directory = g_config.get("MIGRATIONS_DIR", "");
    if (!configured_directory.empty()) {
        candidates.emplace_back(configured_directory);
    }
    candidates.emplace_back("migrations");
    candidates.emplace_back("../migrations");

    for (const fs::path& candidate : candidates) {
        if (fs::is_directory(candidate)) {
            return candidate;
        }
    }
    return {};
}

std::string read_migration(const fs::path& path) {
    std::ifstream file(path);
    if (!file) {
        return {};
    }
    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}

} // namespace

bool Database::init_tables() {
    if (!connected) {
        LOG_ERROR("Cannot init tables: not connected");
        return false;
    }

    try {
        pqxx::work transaction(*conn);
        transaction.exec(R"(
            CREATE TABLE IF NOT EXISTS schema_migrations (
                version VARCHAR(255) PRIMARY KEY,
                applied_at TIMESTAMPTZ NOT NULL DEFAULT NOW()
            )
        )");

        const fs::path migrations_directory = find_migrations_directory();
        if (migrations_directory.empty()) {
            LOG_ERROR("Migrations directory not found");
            return false;
        }

        std::vector<fs::path> migrations;
        for (const auto& entry : fs::directory_iterator(migrations_directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".sql") {
                migrations.push_back(entry.path());
            }
        }
        std::sort(migrations.begin(), migrations.end());

        for (const fs::path& migration : migrations) {
            const std::string version = migration.filename().string();
            const pqxx::result already_applied = transaction.exec_params(
                "SELECT 1 FROM schema_migrations WHERE version = $1", version);
            if (!already_applied.empty()) {
                continue;
            }

            const std::string sql = read_migration(migration);
            if (sql.empty()) {
                throw std::runtime_error("Migration is empty or unreadable: " + version);
            }
            transaction.exec(sql);
            transaction.exec_params("INSERT INTO schema_migrations(version) VALUES ($1)", version);
            LOG("Applied database migration: " + version);
        }

        transaction.commit();
        cleanup_duplicate_words();
        LOG("Database tables initialized successfully");
        return true;
    } catch (const std::exception& error) {
        LOG_ERROR("Failed to init tables: " + std::string(error.what()));
        return false;
    }
}

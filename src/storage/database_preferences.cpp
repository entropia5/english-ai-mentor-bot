#include "database.h"
#include "logger.h"

#include <stdexcept>

bool valid_course_filter(const std::string& filter) {
    return filter == "all" || filter == "conversation" || filter == "medicine" || filter == "it";
}

std::string course_filter_title(const std::string& filter) {
    return filter == "all" ? "Все направления" : course_title(filter);
}

std::string Database::get_dictionary_filter(long long user_id) {
    if (!connected)
        throw std::runtime_error("Database is not connected");
    pqxx::work tx(*conn);
    const auto rows =
        tx.exec_params("SELECT dictionary_filter FROM users WHERE user_id=$1", user_id);
    tx.commit();
    return rows.empty() ? "all" : rows[0][0].as<std::string>();
}

bool Database::set_dictionary_filter(long long user_id, const std::string& filter) {
    if (!connected || !valid_course_filter(filter))
        return false;
    try {
        pqxx::work tx(*conn);
        tx.exec_params("INSERT INTO users(user_id, dictionary_filter) VALUES($1,$2) "
                       "ON CONFLICT(user_id) DO UPDATE SET dictionary_filter=$2, "
                       "new_words_page=0, learned_words_page=0",
                       user_id, filter);
        tx.commit();
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("set_dictionary_filter: " + std::string(e.what()));
        return false;
    }
}

ReminderSettings Database::get_reminder_settings(long long user_id) {
    // Fail closed: a failed settings read must not enable a disabled reminder.
    if (!connected)
        throw std::runtime_error("Database is not connected");
    pqxx::work tx(*conn);
    const auto rows = tx.exec_params("SELECT morning_enabled, evening_enabled, morning_course, "
                                     "evening_course, evening_add_new FROM users WHERE user_id=$1",
                                     user_id);
    tx.commit();
    if (rows.empty())
        return {};
    const auto& row = rows[0];
    return {row[0].as<bool>(), row[1].as<bool>(), row[2].as<std::string>(),
            row[3].as<std::string>(), row[4].as<bool>()};
}

bool Database::set_reminder_setting(long long user_id, const std::string& key,
                                    const std::string& value) {
    const bool boolean =
        key == "morning_enabled" || key == "evening_enabled" || key == "evening_add_new";
    const bool course = key == "morning_course" || key == "evening_course";
    if (!connected || (!boolean && !course) || (boolean && value != "0" && value != "1") ||
        (course && !valid_course_filter(value)))
        return false;
    try {
        pqxx::work tx(*conn);
        tx.exec_params("INSERT INTO users(user_id) VALUES($1) ON CONFLICT DO NOTHING", user_id);
        // Column names are restricted to the fixed allowlist above; values remain parameters.
        if (boolean)
            tx.exec_params("UPDATE users SET " + key + "=$2 WHERE user_id=$1", user_id,
                           value == "1");
        else
            tx.exec_params("UPDATE users SET " + key + "=$2 WHERE user_id=$1", user_id, value);
        tx.commit();
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("set_reminder_setting: " + std::string(e.what()));
        return false;
    }
}

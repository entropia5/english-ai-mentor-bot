#include "user_config.h"

#include "config.h"
#include "core/text.h"
#include "logger.h"

long long configured_user_id(const std::string& key) {
    const std::string value = trim(g_config.get(key, ""));
    if (value.empty()) {
        return 0;
    }

    try {
        return std::stoll(value);
    } catch (const std::exception& error) {
        LOG_ERROR("Invalid " + key + " value: " + value + "; " + error.what());
        return 0;
    }
}

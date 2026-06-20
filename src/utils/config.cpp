#include "config.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

void Config::trim(std::string& s) {
    size_t start = s.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) {
        s.clear();
        return;
    }

    size_t end = s.find_last_not_of(" \t\n\r\f\v");
    s = s.substr(start, end - start + 1);
}

bool Config::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            trim(key);
            trim(value);

            // Убираем кавычки если есть
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }

            values[key] = value;
        }
    }

    return true;
}

std::string Config::get(const std::string& key, const std::string& default_value) const {
    auto it = values.find(key);
    if (it != values.end()) {
        return it->second;
    }
    return default_value;
}

int Config::get_int(const std::string& key, int default_value) const {
    auto it = values.find(key);
    if (it != values.end()) {
        try {
            return std::stoi(it->second);
        } catch (...) {}
    }
    return default_value;
}

long long Config::get_long(const std::string& key, long long default_value) const {
    auto it = values.find(key);
    if (it != values.end()) {
        try {
            return std::stoll(it->second);
        } catch (...) {}
    }
    return default_value;
}

std::vector<long long> Config::get_long_list(const std::string& key, char delimiter) const {
    std::vector<long long> result;
    auto it = values.find(key);
    if (it != values.end()) {
        std::stringstream ss(it->second);
        std::string item;
        while (std::getline(ss, item, delimiter)) {
            // Убираем пробелы (создаем копию, так как item не должен меняться в const методе)
            std::string trimmed = item;
            const_cast<Config*>(this)->trim(trimmed);  // Временный костыль

            if (!trimmed.empty()) {
                try {
                    result.push_back(std::stoll(trimmed));
                } catch (...) {}
            }
        }
    }
    return result;
}

std::set<long long> Config::get_long_set(const std::string& key, char delimiter) const {
    std::set<long long> result;
    auto vec = get_long_list(key, delimiter);
    result.insert(vec.begin(), vec.end());
    return result;
}

bool Config::get_bool(const std::string& key, bool default_value) const {
    auto it = values.find(key);
    if (it != values.end()) {
        std::string val = it->second;
        std::transform(val.begin(), val.end(), val.begin(), ::tolower);
        if (val == "true" || val == "1" || val == "yes" || val == "on") {
            return true;
        }
        if (val == "false" || val == "0" || val == "no" || val == "off") {
            return false;
        }
    }
    return default_value;
}

// define global config instance
Config g_config;

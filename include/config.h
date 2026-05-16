#pragma once

#include <string>
#include <map>
#include <vector>
#include <set>

class Config {
private:
    std::map<std::string, std::string> values;

    void trim(std::string& s);

public:
    bool load(const std::string& filename = ".env");

    std::string get(const std::string& key, const std::string& default_value = "") const;
    int get_int(const std::string& key, int default_value = 0) const;
    long long get_long(const std::string& key, long long default_value = 0) const;
    std::vector<long long> get_long_list(const std::string& key, char delimiter = ',') const;
    std::set<long long> get_long_set(const std::string& key, char delimiter = ',') const;
    bool get_bool(const std::string& key, bool default_value = false) const;
};

// global config instance
extern Config g_config;

#pragma once
#include <string>

struct ReminderSettings {
    bool morning_enabled = true;
    bool evening_enabled = true;
    std::string morning_course = "all";
    std::string evening_course = "all";
    bool evening_add_new = true;
};

bool valid_course_filter(const std::string& filter);
std::string course_filter_title(const std::string& filter);

#pragma once

#include <set>
#include <string>
#include <vector>

struct CourseWord {
    std::string english;
    std::string translation;
    std::string example;
    std::string lesson;
    std::string transcription;
    std::string pronunciation;
};

std::string canonical_course(const std::string& name);
std::string course_title(const std::string& course);
std::vector<CourseWord> load_course_catalog(const std::string& course);
std::vector<CourseWord> select_course_words(const std::vector<CourseWord>& catalog,
                                            const std::set<std::string>& existing, int count);

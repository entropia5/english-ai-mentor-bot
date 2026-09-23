#include "services/course_catalog.h"

#include "config.h"
#include "core/text.h"
#include "file_utils.h"

#include <filesystem>
#include <nlohmann/json.hpp>
#include <stdexcept>

std::string canonical_course(const std::string& name) {
    if (name == "conversation" || name == "daily life" || name == "travel" || name == "food" ||
        name == "business" || name == "communication")
        return "conversation";
    if (name == "medicine")
        return "medicine";
    if (name == "it" || name == "IT programming")
        return "it";
    return {};
}

std::string course_title(const std::string& course) {
    if (course == "conversation")
        return "Разговорный английский";
    if (course == "medicine")
        return "Медицинский английский";
    if (course == "it")
        return "IT";
    return "Неизвестное направление";
}

std::vector<CourseWord> load_course_catalog(const std::string& course) {
    if (canonical_course(course) != course || course.empty()) {
        throw std::runtime_error("Unknown course: " + course);
    }
    namespace fs = std::filesystem;
    const std::string configured = g_config.get("COURSE_CATALOG_DIR", "");
    const std::vector<fs::path> paths =
        configured.empty() ? std::vector<fs::path>{"resources/courses", "../resources/courses"}
                           : std::vector<fs::path>{configured};
    for (const auto& directory : paths) {
        const auto path = directory / (course + ".json");
        if (!fs::exists(path))
            continue;
        const auto document = nlohmann::json::parse(read_text_file(path.string()));
        if (document.at("course") != course || document.at("version") != 1) {
            throw std::runtime_error("Invalid course header: " + path.string());
        }
        std::vector<CourseWord> result;
        std::set<std::string> unique;
        for (const auto& item : document.at("words")) {
            CourseWord word{item.at("english"),
                            item.at("translation"),
                            item.at("example"),
                            item.at("lesson"),
                            item.value("transcription", ""),
                            item.value("pronunciation", "")};
            const auto key = to_lower_ascii(trim(word.english));
            if (key.empty() || trim(word.translation).empty() || trim(word.example).empty() ||
                trim(word.lesson).empty() || trim(word.transcription).empty() ||
                trim(word.pronunciation).empty() || !unique.insert(key).second) {
                throw std::runtime_error("Incomplete or duplicate catalog entry: " + word.english);
            }
            result.push_back(std::move(word));
        }
        if (result.empty() || (course == "conversation" && result.size() != 2000)) {
            throw std::runtime_error("Invalid catalog size: " + course);
        }
        return result;
    }
    throw std::runtime_error("Course catalog not found: " + course);
}

std::vector<CourseWord> select_course_words(const std::vector<CourseWord>& catalog,
                                            const std::set<std::string>& existing, int count) {
    std::vector<CourseWord> result;
    if (count <= 0)
        return result;
    for (const auto& word : catalog) {
        if (existing.count(to_lower_ascii(trim(word.english))) != 0)
            continue;
        result.push_back(word);
        if (static_cast<int>(result.size()) == count)
            break;
    }
    return result;
}

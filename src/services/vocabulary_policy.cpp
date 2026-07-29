#include "services/vocabulary_service.h"
#include "vocabulary_internal.h"

#include <algorithm>
#include <set>
#include <vector>

std::string generation_attempt_hint(int attempt) {
    if (attempt == 1) {
        return "Use practical everyday verbs and concrete nouns that fit A2/B1 learners.";
    }
    if (attempt == 2) {
        return "Use a different lexical angle: errands, services, planning, tools, documents, "
               "places, and real-life actions.";
    }
    if (attempt == 3) {
        return "Use home, work, travel, food, communication, and city-life vocabulary that is "
               "useful but not too basic.";
    }
    if (attempt == 4) {
        return "Use common collocation-friendly nouns and verbs from everyday adult life.";
    }
    if (attempt == 5) {
        return "Use B1/B2 practical vocabulary from services, paperwork, repairs, schedules, and "
               "decisions.";
    }
    return "Use another fresh semantic field. Avoid generic words, rare jargon, brands, apps, and "
           "any previous candidate.";
}

bool is_ascii_english_word(const std::string& word) {
    if (word.size() < 3 || word.size() > 18) {
        return false;
    }
    return std::all_of(word.begin(), word.end(),
                       [](unsigned char c) { return c >= 'a' && c <= 'z'; });
}

const std::set<std::string>& blocked_generated_words() {
    static const std::set<std::string> blocked_words = {
        "swype",   "microwaver", "googling", "instagram", "whatsapp",   "facebook",
        "youtube", "tiktok",     "uber",     "airbnb",    "outpatient", "html",
        "css",     "api",        "sql",      "url",       "usb",        "hacker",
        "virus",   "malware",    "robot",    "mouse"};

    return blocked_words;
}

bool is_suspicious_generated_word(const std::string& normalized_word) {
    static const std::vector<std::string> blocked_suffixes = {"warez"};

    if (!is_ascii_english_word(normalized_word)) {
        return true;
    }
    if (blocked_generated_words().count(normalized_word) > 0) {
        return true;
    }
    for (const std::string& suffix : blocked_suffixes) {
        if (normalized_word.size() > suffix.size() &&
            normalized_word.compare(normalized_word.size() - suffix.size(), suffix.size(),
                                    suffix) == 0) {
            return true;
        }
    }

    return false;
}

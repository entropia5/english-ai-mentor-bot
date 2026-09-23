#include "config.h"
#include "database.h"
#include "groq_client.h"
#include "presentation/screen_components.h"
#include "services/broadcast_service.h"
#include "services/course_catalog.h"
#include "services/vocabulary_service.h"

#include <cstdlib>
#include <future>
#include <iostream>
#include <set>
#include <stdexcept>

namespace {
void expect(bool ok, const std::string& what) {
    if (!ok)
        throw std::runtime_error(what);
    std::cout << "[PASS] " << what << '\n';
}
} // namespace

int main() {
    const char* config = std::getenv("MENTOR_TEST_CONFIG");
    if (config == nullptr)
        return 77;
    try {
        expect(g_config.load(config), "isolated database config loaded");
        expect(g_config.get("DB_NAME").rfind("mentor_courses_test_", 0) == 0,
               "integration tests refuse a non-test database");
        Database db;
        expect(db.connect() && db.init_tables(), "migrations apply to isolated database");
        expect(db.init_tables(), "migrations can be run twice");
        const auto conversation = load_course_catalog("conversation");
        const auto medicine = load_course_catalog("medicine");
        const auto it = load_course_catalog("it");
        constexpr long long user = 1001;
        expect(db.set_active_course(user, "conversation"), "course selection persists");
        expect(db.add_course_words(user, "conversation", conversation, 10) == 10,
               "first batch added");
        expect(db.mark_word_learned(user, "i"), "case-insensitive learned marking");
        expect(db.add_course_words(user, "conversation", conversation, 10) == 10,
               "second batch added");
        expect(db.get_words_count(user, true) == 1 && db.get_words_count(user, false) == 19,
               "known words are not re-added");
        expect(db.set_active_course(user, "medicine") && db.get_user_words_full(user).empty(),
               "course dictionaries are isolated");
        expect(db.add_course_words(user, "medicine", medicine, 10) == 10,
               "medical selection works");
        const std::vector<CourseWord> overlap = {conversation.front()};
        expect(db.add_course_words(user, "medicine", overlap, 1) == 1,
               "same headword can have a different professional meaning");
        expect(db.word_exists(user, "I") && db.get_words_count(user, true) == 0,
               "learned state is not shared across courses");
        expect(!db.mark_word_learned(user, "have"), "marking cannot affect a hidden course");
        expect(db.cleanup_duplicate_words() && db.get_words_count(user, false) == 11,
               "maintenance preserves cross-course headwords");
        expect(db.set_active_course(user, "conversation"), "switching back restores the course");
        expect(db.get_words_count(user, true) == 1, "progress survived switching courses");
        expect(db.add_course_words(1002, "conversation", conversation, 10) == 10,
               "different user starts from the first lesson");
        expect(db.set_active_course(1002, "conversation"), "second user course selected");
        expect(db.get_user_words_full(1002).front().english == "I", "stable within-batch ordering");
        std::vector<std::future<int>> jobs;
        for (int i = 0; i < 3; ++i) {
            jobs.push_back(std::async(std::launch::async, [&conversation] {
                Database connection;
                if (!connection.connect())
                    return -1;
                return connection.add_course_words(1002, "conversation", conversation, 10);
            }));
        }
        for (auto& job : jobs)
            expect(job.get() == 10, "concurrent addition succeeds");
        expect(db.get_words_count(1002, false) == 40,
               "concurrent clicks neither duplicate nor lose words");
        expect(db.add_course_words(1003, "conversation", conversation, 1997) == 1997,
               "large test batch added");
        GroqClient ai;
        const auto last = add_generated_words_to_db(1003, db, ai, "conversation", 10);
        expect(last.added == 3 && last.exhausted && last.failed == 0,
               "partial final batch is reported correctly");
        const auto exhausted = add_generated_words_to_db(1003, db, ai, "conversation", 10);
        expect(exhausted.added == 0 && exhausted.exhausted && exhausted.failed == 0,
               "exhausted catalog adds no fallback or AI words");
        const auto invalid = add_generated_words_to_db(1003, db, ai, "not-a-course", 10);
        expect(invalid.failed == 1 && !invalid.exhausted,
               "invalid course is an error, not exhaustion");
        auto bad = it;
        bad.resize(2);
        bad[1].english = std::string(300, 'x');
        expect(db.add_course_words(1004, "it", bad, 2) == -1, "invalid batch fails");
        expect(db.get_words_count(1004, false, "it") == 0, "failed batch rolls back every insert");
        expect(db.set_active_course(1005, "medicine"), "phrase course selected");
        const std::vector<CourseWord> phrase = {{"May I examine you?", "Можно вас осмотреть?",
                                                 "May I examine you?", "Приём", "/meɪ/", "≈ мэй"}};
        expect(db.add_course_words(1005, "medicine", phrase, 1) == 1 &&
                   db.word_exists(1005, "may i examine you") &&
                   db.mark_word_learned(1005, "May I examine you"),
               "phrases can be marked without copying final punctuation");
        for (int i = 0; i < 8; ++i)
            db.save_conversation(user, "user", std::to_string(i));
        const auto history = db.get_conversation_history(user, 3);
        expect(history.size() == 3 && history.front().second == "5" && history.back().second == "7",
               "practice uses recent history in chronological order");
        expect(db.get_user_words_full(user, false, "all").size() == 31,
               "all dictionary retains words from both courses");
        expect(db.set_dictionary_filter(user, "medicine") &&
                   !db.set_dictionary_filter(user, "invalid"),
               "dictionary filter is validated");
        expect(db.set_reminder_setting(user, "morning_course", "conversation") &&
                   db.set_reminder_setting(user, "evening_course", "medicine") &&
                   db.set_reminder_setting(user, "evening_add_new", "0"),
               "morning and evening sources are independently configurable");
        expect(!db.set_reminder_setting(user, "active_course", "it") &&
                   !db.set_reminder_setting(user, "morning_enabled", "yes") &&
                   !db.set_reminder_setting(user, "evening_course", "invalid"),
               "invalid reminder updates are rejected");
        expect(db.set_active_course(user, "it"), "addition course can change independently");
        Database reopened;
        expect(reopened.connect() && reopened.get_dictionary_filter(user) == "medicine" &&
                   reopened.get_reminder_settings(user).morning_course == "conversation" &&
                   reopened.get_reminder_settings(user).evening_course == "medicine" &&
                   !reopened.get_reminder_settings(user).evening_add_new,
               "preferences survive reconnect and addition course changes");
        expect(db.get_dictionary_filter(1002) == "all", "preferences are isolated per user");
        const auto morning = prepare_morning_review(user, db);
        expect(morning.enabled && !morning.failed && morning.words.size() == 1 &&
                   morning.words.front().learned,
               "morning uses learned words from its own course");
        const auto evening = prepare_evening_review(user, db);
        expect(evening.enabled && !evening.failed && evening.words.size() == 11 &&
                   db.get_user_words_full(user, false, "all").size() == 31,
               "evening repeat mode uses its own course without adding words");
        expect(db.set_reminder_setting(user, "morning_enabled", "0") &&
                   !prepare_morning_review(user, db).enabled &&
                   prepare_evening_review(user, db).enabled,
               "disabling morning leaves evening enabled");
        expect(db.set_reminder_setting(user, "evening_enabled", "0") &&
                   db.set_reminder_setting(user, "evening_add_new", "1") &&
                   !prepare_evening_review(user, db).enabled &&
                   db.get_user_words_full(user, false, "all").size() == 31,
               "disabled evening cannot add new words");
        expect(db.set_reminder_setting(user, "evening_enabled", "1") &&
                   db.set_reminder_setting(user, "evening_course", "all"),
               "evening can be enabled for all courses");
        const auto mixed = prepare_evening_review(user, db);
        expect(!mixed.failed && mixed.words.size() == 35 &&
                   db.get_user_words_full(user, false, "all").size() == 36 &&
                   db.get_words_count(user, false, "it") == 1,
               "all-course evening adds five total distributed words");
        expect(db.get_dictionary_filter(user) == "medicine" && db.get_active_course(user) == "it",
               "scheduled additions preserve browsing and manual addition preferences");
        Database disconnected;
        expect(prepare_morning_review(user, disconnected).failed &&
                   prepare_evening_review(user, disconnected).failed,
               "database failure prevents reminder delivery");
        const auto keyboard = reminder_settings_keyboard(db.get_reminder_settings(user));
        bool morning_on = false, evening_off = false;
        for (const auto& row : keyboard) {
            for (const auto& button : row) {
                expect(button.second.size() <= 64, "reminder callback fits Telegram limit");
                morning_on |= button.second == "rem_morning_enabled_1";
                evening_off |= button.second == "rem_evening_enabled_0";
            }
        }
        expect(morning_on && evening_off, "toggle buttons carry explicit target states");
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "[FAIL] " << e.what() << '\n';
        return 1;
    }
}

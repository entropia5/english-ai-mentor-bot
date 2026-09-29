#include "core/text.h"
#include "database.h"
#include "logger.h"

#include <set>
#include <nlohmann/json.hpp>
#include <stdexcept>

std::string Database::get_active_course(long long user_id) {
    if (!connected)
        return "conversation";
    try {
        pqxx::work tx(*conn);
        const auto rows =
            tx.exec_params("SELECT active_course FROM users WHERE user_id = $1", user_id);
        tx.commit();
        if (!rows.empty())
            return rows[0][0].as<std::string>();
    } catch (const std::exception& e) {
        LOG_ERROR("get_active_course: " + std::string(e.what()));
    }
    return "conversation";
}

bool Database::set_active_course(long long user_id, const std::string& course) {
    if (!connected || course.empty() || canonical_course(course) != course)
        return false;
    try {
        pqxx::work tx(*conn);
        tx.exec_params("INSERT INTO users(user_id, active_course) VALUES ($1, $2) "
                       "ON CONFLICT (user_id) DO UPDATE SET active_course = $2, "
                       "new_words_page = 0, learned_words_page = 0",
                       user_id, course);
        tx.commit();
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("set_active_course: " + std::string(e.what()));
        return false;
    }
}

int Database::add_course_words(long long user_id, const std::string& course,
                               const std::vector<CourseWord>& catalog, int count) {
    if (!connected || course.empty() || canonical_course(course) != course || count <= 0)
        return -1;
    try {
        pqxx::work tx(*conn);
        // Serialize a user's manual and scheduled additions in one transaction.
        tx.exec_params("SELECT pg_advisory_xact_lock($1::bigint)", user_id);
        tx.exec_params("INSERT INTO users(user_id) VALUES ($1) ON CONFLICT DO NOTHING", user_id);
        const auto rows = tx.exec_params("SELECT lower(trim(english)) FROM words "
                                         "WHERE user_id = $1 AND topic = $2",
                                         user_id, course);
        std::set<std::string> existing;
        for (const auto& row : rows)
            existing.insert(row[0].as<std::string>());
        const auto next = select_course_words(catalog, existing, count);
        for (const auto& word : next) {
            tx.exec_params("INSERT INTO words(user_id, english, translation_ru, transcription, "
                           "pronunciation_ru, definition_ru, topic) VALUES ($1,$2,$3,$4,$5,$6,$7)",
                           user_id, word.english, word.translation, word.transcription,
                           word.pronunciation, format_course_word_definition(word), course);
        }
        tx.commit();
        return static_cast<int>(next.size());
    } catch (const std::exception& e) {
        LOG_ERROR("add_course_words: " + std::string(e.what()));
        return -1;
    }
}

std::vector<Word> Database::prepare_evening_batch(long long user_id, const std::string& filter,
                                                 const std::string& date) {
    if (!connected || !valid_course_filter(filter))
        throw std::runtime_error("Cannot prepare evening batch");
    pqxx::work tx(*conn);
    // Та же блокировка, что у ручного добавления слов.
    tx.exec_params("SELECT pg_advisory_xact_lock($1::bigint)", user_id);
    const auto saved = tx.exec_params(
        "SELECT words::text FROM evening_word_batches WHERE user_id=$1 AND batch_date=$2::date",
        user_id, date);
    if (!saved.empty()) {
        std::vector<Word> result;
        for (const auto& item : nlohmann::json::parse(saved[0][0].as<std::string>()))
            result.push_back({item.at("english"), item.at("translation"), false,
                              item.at("pronunciation"), item.at("transcription"),
                              item.at("definition")});
        tx.commit();
        return result;
    }
    tx.exec_params("INSERT INTO users(user_id) VALUES($1) ON CONFLICT DO NOTHING", user_id);
    const std::vector<std::string> courses = filter == "all"
        ? std::vector<std::string>{"conversation", "medicine", "it"}
        : std::vector<std::string>{filter};
    std::vector<std::vector<CourseWord>> candidates;
    for (const auto& course : courses) {
        std::set<std::string> existing;
        for (const auto& row : tx.exec_params(
                 "SELECT lower(trim(english)) FROM words WHERE user_id=$1 AND topic=$2",
                 user_id, course))
            existing.insert(row[0].as<std::string>());
        candidates.push_back(select_course_words(load_course_catalog(course), existing, 5));
    }
    std::vector<Word> result;
    auto snapshot = nlohmann::json::array();
    // По очереди берём слова из направлений, всего не больше пяти.
    for (std::size_t round = 0; round < 5 && result.size() < 5; ++round) {
        for (std::size_t i = 0; i < courses.size() && result.size() < 5; ++i) {
            if (round >= candidates[i].size())
                continue;
            const auto& word = candidates[i][round];
            const auto definition = format_course_word_definition(word);
            tx.exec_params(
                "INSERT INTO words(user_id,english,translation_ru,transcription,"
                "pronunciation_ru,definition_ru,topic) VALUES($1,$2,$3,$4,$5,$6,$7)",
                user_id, word.english, word.translation, word.transcription,
                word.pronunciation, definition, courses[i]);
            result.push_back({word.english, word.translation, false, word.pronunciation,
                              word.transcription, definition});
            snapshot.push_back({{"english", word.english}, {"translation", word.translation},
                                {"pronunciation", word.pronunciation},
                                {"transcription", word.transcription}, {"definition", definition}});
        }
    }
    tx.exec_params("INSERT INTO evening_word_batches(user_id,batch_date,words) "
                   "VALUES($1,$2::date,$3::jsonb)", user_id, date, snapshot.dump());
    tx.commit();
    return result;
}

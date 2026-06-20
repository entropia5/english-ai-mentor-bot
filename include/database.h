#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <memory>
#include <pqxx/pqxx>

struct WordAuditRow {
    int id = 0;
    long long user_id = 0;
    std::string english;
    std::string translation;
    bool is_learned = false;
};

class Database {
private:
    std::unique_ptr<pqxx::connection> conn;
    std::string connection_string;
    bool connected = false;

public:
    Database();
    ~Database();

    bool connect();
    bool is_connected() const { return connected; }
    void disconnect();

    // check connection
    bool test_connection();

    // init tables
    bool init_tables();

    // remove existing duplicate words and enforce uniqueness
    bool cleanup_duplicate_words();

    // find old words without IPA transcription or Russian pronunciation
    std::vector<std::tuple<int, std::string, std::string>> get_words_missing_pronunciation(int limit = 50);

    bool update_word_pronunciation(int word_id, const std::string& transcription,
                                   const std::string& pronunciation_ru);

    std::vector<WordAuditRow> find_words_by_normalized_english(const std::vector<std::string>& english_words);
    int delete_words_by_ids(const std::vector<int>& word_ids);

    // work with users
    bool user_exists(long long user_id);
    bool add_user(long long user_id, const std::string& name);

    // stats and levels
    int get_user_level(long long user_id);
    void update_user_level(long long user_id, int level);

    // work with conversations
    bool save_conversation(long long user_id, const std::string& role, const std::string& content);
    std::vector<std::pair<std::string, std::string>> get_conversation_history(long long user_id, int limit = 10);

    //work with words
    bool add_word(long long user_id, const std::string& english, const std::string& translation,
                  const std::string& pronunciation = "", const std::string& transcription = "",
                  const std::string& topic = "general");

    std::vector<std::tuple<std::string, std::string, bool>> get_user_words(long long user_id, bool only_not_learned = false);

    bool mark_word_learned(long long user_id, const std::string& english);

    bool word_exists(long long user_id, const std::string& english);

    int get_words_count(long long user_id, bool learned = false);

   // get full word info (translation, pronunciation, learned) for all words of user
    std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>> get_user_words_full(long long user_id, bool only_not_learned = false);


};

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <pqxx/pqxx>

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
                  const std::string& pronunciation = "", const std::string& topic = "general");

    std::vector<std::tuple<std::string, std::string, bool>> get_user_words(long long user_id, bool only_not_learned = false);

    bool mark_word_learned(long long user_id, const std::string& english);

    bool word_exists(long long user_id, const std::string& english);

    int get_words_count(long long user_id, bool learned = false);

   // get full word info (translation, pronunciation, learned) for all words of user
    std::vector<std::tuple<std::string, std::string, bool, std::string, std::string>> get_user_words_full(long long user_id, bool only_not_learned = false);


};

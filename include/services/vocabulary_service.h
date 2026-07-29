#pragma once

#include "domain/word.h"

#include <set>
#include <string>
#include <vector>

class Database;
class GroqClient;

struct WordGenerationResult {
    int added = 0;
    int duplicates = 0;
    int duplicate_in_response = 0;
    int rejected_quality = 0;
    int fallback_added = 0;
    int parsed_total = 0;
    int failed = 0;
};

const std::set<std::string>& blocked_generated_words();
bool is_suspicious_generated_word(const std::string& normalized_word);
std::vector<GeneratedWord> fallback_practical_words();
WordGenerationResult add_generated_words_to_db(long long chat_id, Database& database,
                                               GroqClient& ai, const std::string& topic_keyword,
                                               int target_count);
bool backfill_missing_pronunciations(Database& database, GroqClient& ai);
bool backfill_missing_definitions(Database& database, GroqClient& ai);

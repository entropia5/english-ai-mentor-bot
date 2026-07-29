#include "app/self_test.h"

#include "ai/word_response_parser.h"
#include "config.h"
#include "core/text.h"
#include "domain/word.h"
#include "file_utils.h"
#include "services/vocabulary_service.h"

#include <algorithm>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

bool run_self_tests() {
    int failed = 0;
    auto expect = [&](bool condition, const std::string& name) {
        if (condition) {
            std::cout << "[OK] " << name << '\n';
        } else {
            std::cout << "[FAIL] " << name << '\n';
            ++failed;
        }
    };

    const std::string generated = "WORD: receipt\n"
                                  "TRANS: /rɪˈsiːt/\n"
                                  "PRON: рисит\n"
                                  "MEAN: чек\n"
                                  "DEF: бумажное или электронное подтверждение оплаты\n"
                                  "---\n"
                                  "WORD: refund\n"
                                  "TRANS: /ˈriːfʌnd/\n"
                                  "PRON: рифанд\n"
                                  "MEAN: возврат денег\n"
                                  "DEF: деньги, которые возвращают после отмены покупки\n";
    const auto words = parse_generated_words(generated);
    expect(words.size() == 2 && words[0].english == "receipt" &&
               words[1].translation == "возврат денег" &&
               words[1].definition == "деньги, которые возвращают после отмены покупки",
           "parse_generated_words parses machine-readable AI output");

    const std::string definitions = "ID: 10\n"
                                    "DEF: короткое объяснение\n"
                                    "---\n";
    const auto definition_updates = parse_definition_updates(definitions);
    expect(definition_updates.size() == 1 && definition_updates[0].id == 10 &&
               definition_updates[0].definition == "короткое объяснение",
           "parse_definition_updates parses definition backfill output");

    expect(is_suspicious_generated_word("html"), "filter blocks acronym-like weak word");
    expect(is_suspicious_generated_word("hacker"), "filter blocks weak security word");
    expect(!is_suspicious_generated_word("receipt"), "filter accepts practical word");

    const auto split = split_words_input(" house, meeting , refund ");
    expect(split.size() == 3 && split[0] == "house" && split[2] == "refund",
           "split_words_input trims comma-separated words");

    const auto fallback_words = fallback_practical_words();
    const bool fallback_has_definitions =
        !fallback_words.empty() &&
        std::all_of(fallback_words.begin(), fallback_words.end(), [](const GeneratedWord& word) {
            return !trim(word.english).empty() && !trim(word.transcription).empty() &&
                   !trim(word.pronunciation).empty() && !trim(word.translation).empty() &&
                   !trim(word.definition).empty();
        });
    expect(fallback_words.size() >= 50 && fallback_has_definitions,
           "fallback_practical_words loads external dictionary with definitions");

    expect(format_ai_response_box("`int x = 1;`").find('`') != std::string::npos &&
               format_ai_response_box("`int x = 1;`").find("'''") == std::string::npos,
           "format_ai_response_box wraps answer in code block");

    const fs::path config_path =
        fs::temp_directory_path() /
        ("english_mentor_self_test_" + std::to_string(std::time(nullptr)) + ".env");
    const bool config_file_written =
        write_text_file(config_path.string(), "EMPTY=\nQUOTED=\"hello world\"\nNUMBER=42\n");
    Config config;
    const bool config_loaded = config_file_written && config.load(config_path.string());
    expect(config_loaded && config.get("EMPTY", "fallback").empty() &&
               config.get("QUOTED") == "hello world" && config.get_int("NUMBER", 0) == 42,
           "Config::load handles empty and quoted values");
    std::error_code error;
    fs::remove(config_path, error);

    std::cout << (failed == 0 ? "Self-tests passed"
                              : "Self-tests failed: " + std::to_string(failed))
              << '\n';
    return failed == 0;
}

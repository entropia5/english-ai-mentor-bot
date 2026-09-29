#include "ai/word_response_parser.h"
#include "app/command_line.h"
#include "config.h"
#include "core/text.h"
#include "domain/word.h"
#include "presentation/bot_presentation.h"
#include "presentation/bot_state.h"
#include "presentation/screen_components.h"
#include "rendering/bot_renderer.h"
#include "rendering/template_engine.h"
#include "services/course_catalog.h"
#include "services/vocabulary_service.h"
#include "storage/broadcast_run_state.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <string>

namespace {

class TestRunner {
  public:
    void expect(bool condition, const std::string& name) {
        if (condition) {
            std::cout << "[PASS] " << name << '\n';
            return;
        }
        std::cerr << "[FAIL] " << name << '\n';
        ++failed_;
    }

    int result() const {
        return failed_ == 0 ? 0 : 1;
    }

  private:
    int failed_ = 0;
};

void test_generated_words_parser(TestRunner& runner) {
    const std::string response = "WORD: receipt\n"
                                 "TRANS: /rɪˈsiːt/\n"
                                 "PRON: рисит\n"
                                 "MEAN: чек\n"
                                 "DEF: подтверждение оплаты\n"
                                 "---\n"
                                 "WORD: refund\n"
                                 "TRANS: /ˈriːfʌnd/\n"
                                 "PRON: рифанд\n"
                                 "MEAN: возврат денег\n"
                                 "DEF: деньги, возвращённые после отмены покупки\n";

    const auto words = parse_generated_words(response);
    runner.expect(words.size() == 2, "parser returns both generated words");
    runner.expect(words.size() == 2 && words[0].english == "receipt" &&
                      words[1].translation == "возврат денег",
                  "parser maps generated word fields");
}

void test_backfill_parsers(TestRunner& runner) {
    const auto pronunciation =
        parse_pronunciation_updates("ID: 42\nTRANS: /test/\nPRON: тест\n---\n");
    runner.expect(pronunciation.size() == 1 && pronunciation[0].id == 42 &&
                      pronunciation[0].pronunciation == "тест",
                  "pronunciation backfill parser");

    const auto definition = parse_definition_updates("ID: 10\nDEF: короткое объяснение\n---\n");
    runner.expect(definition.size() == 1 && definition[0].id == 10 &&
                      definition[0].definition == "короткое объяснение",
                  "definition backfill parser");
}

void test_text_utilities(TestRunner& runner) {
    const auto words = split_words_input(" house, meeting , refund ");
    runner.expect(words.size() == 3 && words.front() == "house" && words.back() == "refund",
                  "comma-separated word input");
    runner.expect(to_lower_ascii("Cpp-TEST") == "cpp-test", "ASCII normalization");
    runner.expect(trim(" \n value\t") == "value", "whitespace trimming");
    runner.expect(format_ai_response_box("`int value = 1;`") == "```cpp\n'int value = 1;'\n```",
                  "AI response formatting");
}

void test_word_model(TestRunner& runner) {
    Word word{"receipt", "чек", true, "рисит", "/rɪˈsiːt/", "подтверждение оплаты"};

    runner.expect(word.english == "receipt", "word exposes named English field");
    runner.expect(word.learned, "word exposes named learning state");
    runner.expect(word.transcription == "/rɪˈsiːt/" && word.definition == "подтверждение оплаты",
                  "word keeps pronunciation and definition fields");
}

void test_vocabulary_policy(TestRunner& runner) {
    runner.expect(!is_suspicious_generated_word("receipt") &&
                      !is_suspicious_generated_word("appointment"),
                  "vocabulary policy accepts practical dictionary words");
    runner.expect(is_suspicious_generated_word("tiktok") &&
                      is_suspicious_generated_word("two words") &&
                      is_suspicious_generated_word("x"),
                  "vocabulary policy rejects brands, phrases and short noise");
}

void test_fallback_dictionary(TestRunner& runner) {
    const std::vector<GeneratedWord> words = fallback_practical_words();
    runner.expect(!words.empty(), "fallback repository loads vocabulary");

    std::set<std::string> unique_words;
    bool complete = true;
    for (const GeneratedWord& word : words) {
        unique_words.insert(word.english);
        complete = complete && !word.english.empty() && !word.translation.empty() &&
                   !word.transcription.empty() && !word.pronunciation.empty() &&
                   !word.definition.empty();
    }
    runner.expect(unique_words.size() == words.size(), "fallback repository removes duplicates");
    runner.expect(complete, "fallback repository returns complete word records");
}

void test_course_catalogs(TestRunner& runner) {
    const auto conversation = load_course_catalog("conversation");
    const auto medicine = load_course_catalog("medicine");
    const auto it = load_course_catalog("it");
    runner.expect(conversation.size() == 2000, "conversation contains exactly 2000 entries");
    runner.expect(medicine.size() >= 300 && it.size() >= 250,
                  "professional catalogs contain substantial topic-specific material");
    for (const auto* catalog : {&conversation, &medicine, &it}) {
        bool translated = true;
        for (const auto& word : *catalog)
            translated = translated && !trim(word.example_translation).empty() &&
                         word.example_translation != word.example &&
                         !trim(word.example_pronunciation).empty();
        runner.expect(translated, "every example in the course has a separate translation");
    }
    const auto definition = format_course_word_definition(conversation.front());
    runner.expect(definition.find("Пример: I am ready. · ≈ ай эм рэ́ди.\nПеревод: Я готов.") != std::string::npos,
                  "English example and stressed reading hint share one line");
    std::set<std::string> keys;
    bool complete = true;
    for (const auto& word : conversation) {
        keys.insert(to_lower_ascii(word.english));
        complete = complete && !word.translation.empty() && !word.example.empty() &&
                   !word.example_translation.empty() && !word.lesson.empty() && !word.transcription.empty() &&
                   !word.pronunciation.empty();
    }
    runner.expect(keys.size() == 2000 && complete, "unique complete conversation cards");
    runner.expect(keys.count("i") && keys.count("be") && keys.count("go") && keys.count("do"),
                  "short essential words are not rejected by legacy AI filters");
    const auto first = select_course_words(conversation, {}, 10);
    runner.expect(first.size() == 10 && first.front().english == "I",
                  "curriculum begins with useful simple speech");
    std::set<std::string> existing;
    for (const auto& w : first)
        existing.insert(to_lower_ascii(w.english));
    const auto second = select_course_words(conversation, existing, 10);
    bool disjoint = true;
    for (const auto& w : second)
        disjoint = disjoint && !existing.count(to_lower_ascii(w.english));
    runner.expect(disjoint && second.size() == 10,
                  "next batch excludes all previously added words");
    runner.expect(select_course_words(conversation, keys, 10).empty(),
                  "exhausted catalog produces no invented fallback");
    keys.erase(to_lower_ascii(conversation.back().english));
    runner.expect(select_course_words(conversation, keys, 10).size() == 1,
                  "last partial batch contains only remaining entries");
    runner.expect(select_course_words(conversation, {}, 0).empty(), "zero count adds nothing");
    bool medical_term = false, medical_phrase = false, it_term = false;
    for (const auto& w : medicine) {
        medical_term = medical_term || w.english == "outpatient";
        medical_phrase = medical_phrase || w.english == "blood pressure";
    }
    for (const auto& w : it)
        it_term = it_term || w.english == "API";
    runner.expect(medical_term && medical_phrase && it_term,
                  "professional terms and expressions bypass obsolete bans");
    bool invalid_rejected = false;
    try {
        load_course_catalog("../../etc/passwd");
    } catch (const std::exception&) {
        invalid_rejected = true;
    }
    runner.expect(invalid_rejected && canonical_course("unknown").empty(),
                  "unknown courses fail closed");
    const auto keyboard = topic_keyboard();
    runner.expect(keyboard.size() == 4 && keyboard[0][0].second == "course_conversation" &&
                      keyboard[1][0].second == "course_medicine" &&
                      keyboard[2][0].second == "course_it",
                  "menu has exactly three courses and a back button");
}

void test_word_formatting(TestRunner& runner) {
    const std::string formatted =
        format_word("receipt", "чек", "/rɪˈsiːt/", "рисит", "подтверждение оплаты");
    runner.expect(formatted.find("receipt") != std::string::npos &&
                      formatted.find("/rɪˈsiːt/") != std::string::npos,
                  "word formatter includes English word and transcription");
    runner.expect(formatted.find("подтверждение оплаты") != std::string::npos,
                  "word formatter includes definition");
}

void test_rendering_templates(TestRunner& runner) {
    const RenderedTemplate status =
        render_template("status.html", {{"title", "<Готово>"},
                                        {"subtitle", "Шаблон {{value}} загружается"},
                                        {"note", "без перекомпиляции"}});
    runner.expect(static_cast<bool>(status), "runtime rendering template loads");
    runner.expect(status.html.find("&lt;Готово&gt;") != std::string::npos,
                  "template values are HTML-escaped");
    runner.expect(status.html.find("Шаблон {{value}} загружается") != std::string::npos,
                  "user text may contain template-like braces");
    runner.expect(status.html.find("--panel:") != std::string::npos,
                  "runtime theme is injected into template");

    const RenderedTemplate missing_value =
        render_template("status.html", {{"title", "Готово"}, {"subtitle", "Проверка"}});
    runner.expect(!missing_value, "template rejects a missing placeholder value");

    const RenderedTemplate path_traversal = render_template("../../fallback_words.json");
    runner.expect(!path_traversal, "template loader rejects paths outside templates directory");
}

void test_config(TestRunner& runner) {
    const auto path = std::filesystem::temp_directory_path() / "english_mentor_config_test.env";
    {
        std::ofstream file(path);
        file << "EMPTY=\nQUOTED=\"hello world\"\nIDS=10, 20,30\n";
    }

    Config config;
    runner.expect(config.load(path.string()), "config file loads");
    runner.expect(config.get("EMPTY", "fallback").empty(), "empty config value");
    runner.expect(config.get("QUOTED") == "hello world", "quoted config value");
    runner.expect(config.get_long_list("IDS") == std::vector<long long>({10, 20, 30}),
                  "config list parsing");

    std::error_code error;
    std::filesystem::remove(path, error);
}

void test_command_line(TestRunner& runner) {
    const auto maintenance =
        parse_command_line({"english_mentor", "--audit-words", "--render-preview",
                            "--refresh-doc-screenshots", "123456"});
    runner.expect(maintenance.ok() && maintenance.options.audit_words &&
                      maintenance.options.render_preview &&
                      maintenance.options.refresh_doc_screenshots &&
                      maintenance.options.doc_screenshots_chat_id == 123456,
                  "command line parser maps maintenance options");

    const auto invalid_id =
        parse_command_line({"english_mentor", "--send-evening-once", "not-a-number"});
    runner.expect(!invalid_id.ok(), "command line parser rejects invalid chat id");

    const auto unknown = parse_command_line({"english_mentor", "--unknown-option"});
    runner.expect(!unknown.ok(), "command line parser rejects unknown options");
}

void test_routing_helpers(TestRunner& runner) {
    const nlohmann::json photo_message = {
        {"photo", nlohmann::json::array({{{"file_id", "test"}}})}};
    const nlohmann::json text_message = {{"text", "hello"}};

    runner.expect(screen_message_type_from_callback(photo_message) == ScreenMessageType::Photo,
                  "callback router detects photo screen");
    runner.expect(screen_message_type_from_callback(text_message) == ScreenMessageType::Text,
                  "callback router detects text screen");
    runner.expect(is_persistable_screen_context("dictionary") &&
                      !is_persistable_screen_context("temporary-error"),
                  "screen state accepts only stable routing contexts");
}

void test_runtime_state_helpers(TestRunner& runner) {
    std::tm date{};
    date.tm_year = 126;
    date.tm_mon = 6;
    date.tm_mday = 29;
    runner.expect(local_date_key(date) == "2026-07-29",
                  "runtime state formats stable local date key");
}

} // namespace

int main() {
    TestRunner runner;
    test_generated_words_parser(runner);
    test_backfill_parsers(runner);
    test_text_utilities(runner);
    test_word_model(runner);
    test_vocabulary_policy(runner);
    test_fallback_dictionary(runner);
    test_course_catalogs(runner);
    test_word_formatting(runner);
    test_rendering_templates(runner);
    test_config(runner);
    test_command_line(runner);
    test_routing_helpers(runner);
    test_runtime_state_helpers(runner);
    return runner.result();
}

#include "services/vocabulary_service.h"

#include "ai/word_response_parser.h"
#include "core/text.h"
#include "database.h"
#include "groq_client.h"
#include "logger.h"
#include "vocabulary_internal.h"

#include <algorithm>
#include <set>

std::string build_existing_words_hint(const std::vector<WordView>& words) {
    std::string result;
    for (const auto& word : words) {
        if (!result.empty()) {
            result += ", ";
        }
        result += word.english;
    }
    return result.empty() ? "" : "\nУже есть в словаре: " + result + ".\n";
}

std::string build_seen_generated_words_hint(const std::set<std::string>& words) {
    if (words.empty()) {
        return "";
    }
    std::string result;
    for (const auto& word : words) {
        if (!result.empty()) {
            result += ", ";
        }
        result += word;
    }
    return "\nНе повторяй ранее предложенные варианты: " + result + ".\n";
}
WordGenerationResult add_generated_words_to_db(long long chat_id, Database& db, GroqClient& ai,
                                               const std::string& topic_keyword,
                                               int target_count = 10) {
    WordGenerationResult result;
    std::set<std::string> seen_generated_words;
    constexpr int max_attempts = 8;

    for (int attempt = 1; attempt <= max_attempts && result.added < target_count; attempt++) {
        auto existing_words = db.get_user_words_full(chat_id, false);
        int need = target_count - result.added;
        int requested_candidates = std::min(30, std::max(need * 4, 16));

        std::string prompt =
            "Generate " + std::to_string(requested_candidates) +
            " candidate English words. I will accept only " + std::to_string(need) +
            " of them. Topic: " + topic_keyword +
            ". Use practical dictionary headwords for language learners. Do not repeat words from "
            "the avoid list. "
            "Do not repeat words inside your own answer. "
            "Return only real common English dictionary words, lowercase, 3-18 letters, one word "
            "only. "
            "Do not invent words. Do not use acronyms, brand names, app names, typos, slang "
            "spellings, medical terms, "
            "security scare words, hardware objects, or obscure jargon. " +
            generation_attempt_hint(attempt) +
            " "
            "For every word, TRANS must be the original English IPA transcription with slashes, "
            "PRON must be a Russian Cyrillic pronunciation hint, "
            "and DEF must be a short Russian explanation of the word meaning, not just a synonym." +
            build_existing_words_hint(existing_words) +
            build_seen_generated_words_hint(seen_generated_words) +
            "\nReturn ONLY blocks in this exact format, no numbering and no extra text:\n"
            "WORD: word\n"
            "TRANS: /IPA transcription/\n"
            "PRON: русское произношение кириллицей\n"
            "MEAN: russian translation\n"
            "DEF: краткое объяснение смысла на русском\n"
            "---\n";

        std::string response = ai.ask(prompt);
        LOG("AI Response length: " + std::to_string(response.length()) +
            ", attempt: " + std::to_string(attempt));

        auto generated_words = parse_generated_words(response);
        result.parsed_total += static_cast<int>(generated_words.size());
        LOG("Parsed generated words: " + std::to_string(generated_words.size()));

        if (generated_words.empty()) {
            result.failed++;
            continue;
        }

        for (const auto& word : generated_words) {
            if (result.added >= target_count)
                break;

            std::string normalized_word = to_lower_ascii(trim(word.english));
            if (normalized_word.empty()) {
                result.failed++;
                continue;
            }
            std::string translation = trim(word.translation);
            std::string pronunciation = trim(word.pronunciation);
            std::string transcription = trim(word.transcription);
            std::string definition = trim(word.definition);
            if (translation.empty() || pronunciation.empty() || transcription.empty() ||
                definition.empty()) {
                result.rejected_quality++;
                LOG("Generated incomplete word rejected: " + word.english);
                continue;
            }
            if (is_suspicious_generated_word(normalized_word)) {
                result.rejected_quality++;
                LOG("Generated low-quality word rejected: " + word.english);
                continue;
            }
            if (seen_generated_words.count(normalized_word) > 0) {
                result.duplicate_in_response++;
                LOG("Generated duplicate inside response skipped: " + word.english);
                continue;
            }
            seen_generated_words.insert(normalized_word);

            if (db.word_exists(chat_id, normalized_word)) {
                result.duplicates++;
                LOG("Generated duplicate skipped before insert: " + word.english);
                continue;
            }

            LOG("Adding word: " + normalized_word + " -> " + translation);
            if (db.add_word(chat_id, normalized_word, translation, pronunciation, transcription,
                            topic_keyword, definition)) {
                result.added++;
            } else {
                result.failed++;
            }
        }
    }

    if (result.added < target_count) {
        for (const GeneratedWord& word : fallback_practical_words()) {
            if (result.added >= target_count)
                break;

            std::string normalized_word = to_lower_ascii(trim(word.english));
            std::string translation = trim(word.translation);
            std::string pronunciation = trim(word.pronunciation);
            std::string transcription = trim(word.transcription);
            std::string definition = trim(word.definition);
            if (definition.empty() && !translation.empty()) {
                definition = "Означает: " + translation + ".";
            }
            if (normalized_word.empty() || seen_generated_words.count(normalized_word) > 0 ||
                translation.empty() || pronunciation.empty() || transcription.empty() ||
                definition.empty() || is_suspicious_generated_word(normalized_word)) {
                continue;
            }
            seen_generated_words.insert(normalized_word);

            if (db.word_exists(chat_id, normalized_word)) {
                result.duplicates++;
                continue;
            }

            LOG("Adding fallback word: " + normalized_word + " -> " + translation);
            if (db.add_word(chat_id, normalized_word, translation, pronunciation, transcription,
                            topic_keyword, definition)) {
                result.added++;
                result.fallback_added++;
            } else {
                result.failed++;
            }
        }
    }

    return result;
}

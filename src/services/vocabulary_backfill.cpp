#include "ai/word_response_parser.h"
#include "core/text.h"
#include "database.h"
#include "groq_client.h"
#include "logger.h"
#include "services/vocabulary_service.h"

#include <chrono>
#include <sstream>
#include <thread>

bool backfill_missing_pronunciations(Database& db, GroqClient& ai) {
    if (!ai.is_available()) {
        LOG_ERROR("Cannot backfill transcriptions: GROQ_API_KEY is not configured");
        return false;
    }

    int total_updated = 0;
    constexpr int batch_size = 15;
    constexpr int max_batches = 100;

    for (int batch = 1; batch <= max_batches; batch++) {
        auto missing_words = db.get_words_missing_pronunciation(batch_size);
        if (missing_words.empty()) {
            LOG("Transcription backfill complete, updated " + std::to_string(total_updated) +
                " words");
            return true;
        }

        std::stringstream prompt;
        prompt << "Fill missing pronunciation data for these English words. "
               << "Return ONLY blocks in this exact format, keep IDs exactly, no numbering and no "
                  "extra text:\n"
               << "ID: 123\n"
               << "TRANS: /IPA transcription/\n"
               << "PRON: русское произношение кириллицей\n"
               << "---\n\n"
               << "Words:\n";

        for (const auto& [id, english, translation] : missing_words) {
            prompt << "ID: " << id << "\n"
                   << "WORD: " << english << "\n"
                   << "MEAN: " << translation << "\n"
                   << "---\n";
        }

        std::string response = ai.ask(
            prompt.str(),
            "You return machine-readable English pronunciation data only. "
            "TRANS is IPA with slashes. PRON is Russian Cyrillic approximate pronunciation.");
        auto updates = parse_pronunciation_updates(response);
        LOG("Backfill batch " + std::to_string(batch) + ": parsed " +
            std::to_string(updates.size()) + " pronunciation updates");

        int batch_updated = 0;
        for (const auto& update : updates) {
            if (db.update_word_pronunciation(update.id, update.transcription,
                                             update.pronunciation)) {
                batch_updated++;
            }
        }

        total_updated += batch_updated;
        if (batch_updated == 0) {
            LOG_ERROR("Transcription backfill stopped: AI returned no usable updates");
            return false;
        }
    }

    LOG_WARNING("Transcription backfill reached batch limit, updated " +
                std::to_string(total_updated) + " words");
    return true;
}

bool backfill_missing_definitions(Database& db, GroqClient& ai) {
    if (!ai.is_available()) {
        LOG_ERROR("Cannot backfill definitions: GROQ_API_KEY is not configured");
        return false;
    }

    int total_updated = 0;
    constexpr int batch_size = 20;
    constexpr int max_batches = 100;
    int empty_update_retries = 0;

    for (int batch = 1; batch <= max_batches; batch++) {
        auto missing_words = db.get_words_missing_definition(batch_size);
        if (missing_words.empty()) {
            LOG("Definition backfill complete, updated " + std::to_string(total_updated) +
                " words");
            return true;
        }

        std::stringstream prompt;
        prompt << "Write short Russian meaning explanations for these English words. "
               << "Each DEF must explain the sense in simple Russian, not just repeat the "
                  "translation. "
               << "Keep DEF concise: 1 short sentence, max 90 characters. "
               << "Return ONLY blocks in this exact format, keep IDs exactly, no numbering and no "
                  "extra text:\n"
               << "ID: 123\n"
               << "DEF: краткое объяснение смысла слова\n"
               << "---\n\n"
               << "Words:\n";

        for (const auto& [id, english, translation] : missing_words) {
            prompt << "ID: " << id << "\n"
                   << "WORD: " << english << "\n"
                   << "TRANSLATION: " << translation << "\n"
                   << "---\n";
        }

        std::string response = ai.ask(
            prompt.str(),
            "You return machine-readable Russian word definitions only. "
            "Each DEF is a concise Russian explanation of meaning, not an example sentence.");
        auto updates = parse_definition_updates(response);
        LOG("Definition backfill batch " + std::to_string(batch) + ": parsed " +
            std::to_string(updates.size()) + " updates");

        int batch_updated = 0;
        for (const auto& update : updates) {
            if (db.update_word_definition(update.id, trim(update.definition))) {
                batch_updated++;
            }
        }

        total_updated += batch_updated;
        if (batch_updated == 0) {
            if (empty_update_retries < 3) {
                empty_update_retries++;
                LOG_WARNING("Definition backfill batch returned no usable updates; retry " +
                            std::to_string(empty_update_retries) + "/3 after delay");
                std::this_thread::sleep_for(std::chrono::seconds(5));
                batch--;
                continue;
            }
            LOG_ERROR("Definition backfill stopped: AI returned no usable updates");
            return false;
        }
        empty_update_retries = 0;
    }

    LOG_WARNING("Definition backfill reached batch limit, updated " +
                std::to_string(total_updated) + " words");
    return true;
}

#include "ai/word_response_parser.h"
#include "response_fields.h"

std::vector<GeneratedWord> parse_generated_words(const std::string& response) {
    std::vector<GeneratedWord> words;
    GeneratedWord current;
    for (const auto& field : ai::detail::parse_response_fields(response)) {
        if (field.label == "word") {
            if (!current.english.empty() && !current.translation.empty()) {
                words.push_back(current);
                current = {};
            }
            current.english = field.value;
        } else if (field.label == "trans" || field.label == "transcription") {
            current.transcription = field.value;
        } else if (field.label == "pron" || field.label == "pronunciation") {
            current.pronunciation = field.value;
        } else if (field.label == "mean" || field.label == "meaning" ||
                   field.label == "translation") {
            current.translation = field.value;
        } else if (field.label == "def" || field.label == "definition" ||
                   field.label == "explanation") {
            current.definition = field.value;
        }
    }

    if (!current.english.empty() && !current.translation.empty()) {
        words.push_back(current);
    }
    return words;
}

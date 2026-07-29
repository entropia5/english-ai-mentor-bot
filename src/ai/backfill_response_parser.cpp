#include "ai/word_response_parser.h"
#include "response_fields.h"

#include <exception>

namespace {

int parse_id(const std::string& value) {
    try {
        return std::stoi(value);
    } catch (const std::exception&) {
        return 0;
    }
}

void append_complete(std::vector<PronunciationUpdate>& updates, PronunciationUpdate& current) {
    if (current.id > 0 && (!current.transcription.empty() || !current.pronunciation.empty())) {
        updates.push_back(current);
    }
    current = {};
}

void append_complete(std::vector<DefinitionUpdate>& updates, DefinitionUpdate& current) {
    if (current.id > 0 && !current.definition.empty()) {
        updates.push_back(current);
    }
    current = {};
}

} // namespace

std::vector<PronunciationUpdate> parse_pronunciation_updates(const std::string& response) {
    std::vector<PronunciationUpdate> updates;
    PronunciationUpdate current;
    for (const auto& field : ai::detail::parse_response_fields(response)) {
        if (field.label == "id") {
            append_complete(updates, current);
            current.id = parse_id(field.value);
        } else if (field.label == "trans" || field.label == "transcription") {
            current.transcription = field.value;
        } else if (field.label == "pron" || field.label == "pronunciation") {
            current.pronunciation = field.value;
        }
    }
    append_complete(updates, current);
    return updates;
}

std::vector<DefinitionUpdate> parse_definition_updates(const std::string& response) {
    std::vector<DefinitionUpdate> updates;
    DefinitionUpdate current;
    for (const auto& field : ai::detail::parse_response_fields(response)) {
        if (field.label == "id") {
            append_complete(updates, current);
            current.id = parse_id(field.value);
        } else if (field.label == "def" || field.label == "definition" ||
                   field.label == "explanation") {
            current.definition = field.value;
        }
    }
    append_complete(updates, current);
    return updates;
}

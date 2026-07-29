#pragma once

#include <string>
#include <utility>

struct Word {
    std::string english;
    std::string translation;
    bool learned = false;
    std::string pronunciation;
    std::string transcription;
    std::string definition;
};

struct GeneratedWord {
    std::string english;
    std::string transcription;
    std::string pronunciation;
    std::string translation;
    std::string definition;

    GeneratedWord() = default;

    GeneratedWord(std::string english_value, std::string transcription_value,
                  std::string pronunciation_value, std::string translation_value,
                  std::string definition_value = {})
        : english(std::move(english_value)), transcription(std::move(transcription_value)),
          pronunciation(std::move(pronunciation_value)), translation(std::move(translation_value)),
          definition(std::move(definition_value)) {}
};

struct PronunciationUpdate {
    int id = 0;
    std::string transcription;
    std::string pronunciation;
};

struct DefinitionUpdate {
    int id = 0;
    std::string definition;
};

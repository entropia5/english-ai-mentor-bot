#pragma once

#include "domain/word.h"

#include <string>
#include <vector>

std::vector<GeneratedWord> parse_generated_words(const std::string& response);
std::vector<PronunciationUpdate> parse_pronunciation_updates(const std::string& response);
std::vector<DefinitionUpdate> parse_definition_updates(const std::string& response);

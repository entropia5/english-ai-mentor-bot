#include "domain/progress.h"

std::string english_level_from_learned(int learned_words) {
    if (learned_words < 100) {
        return "A1";
    }
    if (learned_words < 300) {
        return "A2";
    }
    if (learned_words < 700) {
        return "B1";
    }
    if (learned_words < 1500) {
        return "B2";
    }
    return "C1";
}

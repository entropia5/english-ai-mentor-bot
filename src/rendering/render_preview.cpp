#include "rendering/render_preview.h"

#include "rendering/bot_renderer.h"
#include "services/course_catalog.h"

#include <iostream>
#include <string>
#include <vector>

bool render_preview_screens() {
    std::vector<WordView> words;
    const auto catalog = load_course_catalog("conversation");
    for (std::size_t i = 0; i < 5; ++i) {
        const auto& w = catalog[i];
        words.push_back({w.english, w.translation, false, w.pronunciation, w.transcription,
                         format_course_word_definition(w)});
    }
    const std::vector<std::string> images = {
        render_main_menu_image("17"),
        render_ai_prompt_image(),
        render_topic_menu_image(),
        render_reminder_settings_image(),
        render_status_image("preview", "Готово", "Дизайн загружен из runtime-шаблона",
                            "Измените CSS или HTML и запустите preview снова."),
        render_stats_image(0, 42, 17, "Разговорный английский", "", 2000, 0),
        render_dictionary_words_image(0, words, 0, 1, 0, static_cast<int>(words.size())),
        render_learned_words_image(0, words, 0, 1, 0, static_cast<int>(words.size())),
        render_daily_review_image(0, words, 0, 1, 0, static_cast<int>(words.size())),
        render_evening_words_image(0, words, 0, 1, 0, static_cast<int>(words.size())),
    };

    bool success = true;
    for (const std::string& image : images) {
        if (image.empty()) {
            success = false;
        } else {
            std::cout << image << '\n';
        }
    }
    return success;
}

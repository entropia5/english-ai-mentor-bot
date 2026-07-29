#include "rendering/render_preview.h"

#include "rendering/bot_renderer.h"

#include <iostream>
#include <string>
#include <vector>

bool render_preview_screens() {
    const std::vector<WordView> words = {
        {"receipt", "чек", false, "рисит", "/rɪˈsiːt/", "подтверждение оплаты"},
        {"refund", "возврат денег", true, "рифанд", "/ˈriːfʌnd/",
         "деньги, возвращённые после отмены покупки"},
    };
    const std::vector<std::string> images = {
        render_main_menu_image("B1"),
        render_ai_prompt_image(),
        render_topic_menu_image(),
        render_status_image("preview", "Готово", "Дизайн загружен из runtime-шаблона",
                            "Измените CSS или HTML и запустите preview снова."),
        render_stats_image(0, 42, 17, "B1", "B2", 58, 40),
        render_dictionary_words_image(0, words, 0, 1, 0, static_cast<int>(words.size())),
        render_learned_words_image(0, words, 0, 1, 0, static_cast<int>(words.size())),
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

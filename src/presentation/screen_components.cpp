#include "presentation/screen_components.h"

#include <algorithm>

InlineKeyboard column_keyboard(std::initializer_list<std::pair<std::string, std::string>> buttons) {
    InlineKeyboard keyboard;
    for (const auto& button : buttons) {
        keyboard.push_back({button});
    }
    return keyboard;
}

InlineKeyboard main_menu_keyboard() {
    return column_keyboard({{"Слова, которые я ещё учу", "menu_dictionary"},
                            {"Словарь выученных слов", "menu_learned"},
                            {"Добавить новые слова", "menu_new_words"},
                            {"Спросить AI", "menu_ai"},
                            {"Статистика", "menu_stats"},
                            {"Напоминания", "menu_reminders"}});
}

InlineKeyboard topic_keyboard() {
    return column_keyboard({{"Разговорный английский · 2000 слов", "course_conversation"},
                            {"Медицинский английский", "course_medicine"},
                            {"IT", "course_it"},
                            {"Главное меню", "menu_main"}});
}

InlineKeyboard page_keyboard(const std::string& callback_prefix, int page, int total,
                             const std::string& menu_callback) {
    InlineKeyboard buttons;
    const int previous_page = std::max(0, page - 1);
    const int next_page = std::min(total - 1, page + 1);
    buttons.push_back({{"<", callback_prefix + "prev_" + std::to_string(previous_page)},
                       {std::to_string(page + 1) + "/" + std::to_string(total),
                        callback_prefix + "info_" + std::to_string(page)},
                       {">", callback_prefix + "next_" + std::to_string(next_page)}});
    buttons.push_back({{"Главное меню", menu_callback}});
    return buttons;
}

std::string screen_caption(const std::string& title, int page, int total, const std::string& hint) {
    std::string caption = "Сейчас открыто: " + title;
    if (page >= 0 && total > 0) {
        caption += " · страница " + std::to_string(page + 1) + "/" + std::to_string(total);
    }
    caption += ".";
    if (!hint.empty()) {
        caption += "\n" + hint;
    }
    return caption;
}

#include "presentation/bot_presentation.h"
#include "presentation/bot_state.h"
#include "presentation/screen_components.h"
#include "presentation/screen_transport.h"
#include "rendering/bot_renderer.h"

InlineKeyboard course_filter_keyboard(const std::string& prefix, const std::string& selected) {
    InlineKeyboard keyboard;
    const auto label = [&](const std::string& id, const std::string& text) {
        return (selected == id ? "✓ " : "") + text;
    };
    keyboard.push_back({{label("all", "Все"), prefix + "all"},
                        {label("conversation", "Разговорный"), prefix + "conversation"}});
    keyboard.push_back({{label("medicine", "Медицинский"), prefix + "medicine"},
                        {label("it", "IT"), prefix + "it"}});
    return keyboard;
}

InlineKeyboard reminder_settings_keyboard(const ReminderSettings& settings) {
    InlineKeyboard buttons = column_keyboard(
        {{settings.morning_enabled ? "Выключить утро" : "Включить утро",
          std::string("rem_morning_enabled_") + (settings.morning_enabled ? "0" : "1")}});
    auto morning = course_filter_keyboard("rem_morning_course_", settings.morning_course);
    morning.front().front().first =
        std::string(settings.morning_course == "all" ? "✓ " : "") + "Повторять все слова";
    // Длинную подпись показываем отдельной строкой.
    const auto all_morning = morning.front().front();
    morning.front().erase(morning.front().begin());
    buttons.push_back({all_morning});
    buttons.insert(buttons.end(), morning.begin(), morning.end());
    buttons.push_back(
        {{settings.evening_enabled ? "Выключить вечер" : "Включить вечер",
          std::string("rem_evening_enabled_") + (settings.evening_enabled ? "0" : "1")}});
    buttons.push_back({{std::string(settings.evening_add_new ? "" : "✓ ") + "Показывать ещё не выученные слова",
                        "rem_evening_add_new_0"}});
    buttons.push_back({{std::string(settings.evening_add_new ? "✓ " : "") + "Добавлять ежедневно 5 новых слов",
                        "rem_evening_add_new_1"}});
    buttons.push_back({{"Главное меню", "menu_main"}});
    return buttons;
}

void show_reminder_settings(long long chat_id, TelegramClient& bot, Database& database,
                            int message_id) {
    const auto settings = database.get_reminder_settings(chat_id);
    remember_screen_context(chat_id, "reminders");
    const std::string text =
        "*Утро · 09:00* — " + std::string(settings.morning_enabled ? "включено" : "выключено") +
        "\nПовторение выученных слов.\nНаправление: " +
        course_filter_title(settings.morning_course) + "\n\n*Вечер · 21:00* — " +
        std::string(settings.evening_enabled ? "включено" : "выключено") + "\n" +
        (settings.evening_add_new
             ? "Ежедневно добавлять до 5 новых слов."
             : "Показывать ещё не выученные слова. Новые не добавляются.") +
        "\nНаправление из добавления слов: " + course_filter_title(settings.evening_course);
    const auto buttons = reminder_settings_keyboard(settings);
    const auto image = render_reminder_settings_image();
    if (image.empty()) {
        upsert_screen(chat_id, bot, "*Напоминания*\n\n" + text, buttons, message_id);
        return;
    }
    upsert_photo_screen(chat_id, bot, image, buttons, message_id, text, "Markdown");
}

void show_user_dictionary(long long chat_id, TelegramClient& bot, Database& database, bool learned,
                          int page, int message_id, bool is_new) {
    const auto filter = database.get_dictionary_filter(chat_id);
    auto words = database.get_user_words_full(chat_id, !learned, filter);
    if (learned) {
        std::vector<WordView> filtered;
        for (const auto& word : words)
            if (word.learned)
                filtered.push_back(word);
        words = std::move(filtered);
    }
    int current_page = page;
    std::string action;
    if (learned)
        show_learned_page(chat_id, bot, words, page, current_page, action, message_id, is_new,
                          filter);
    else
        show_dictionary_page(chat_id, bot, words, page, current_page, action, message_id, is_new,
                             filter);
}

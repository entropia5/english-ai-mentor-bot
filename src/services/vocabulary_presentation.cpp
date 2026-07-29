#include "services/vocabulary_presentation.h"

#include "presentation/bot_presentation.h"
#include "presentation/bot_state.h"
#include "presentation/screen_components.h"
#include "services/vocabulary_service.h"

void generate_words(long long chat_id, TelegramClient& bot, Database& database, GroqClient& ai,
                    const std::string& topic_keyword, const std::string& topic_name,
                    int message_id) {
    remember_screen_context(chat_id, "generation");
    show_status_screen(
        chat_id, bot, "generate_" + topic_keyword, "Генерирую слова", "Тема: " + topic_name,
        "AI подбирает реальные словарные слова, проверяет дубли и готовит произношение.",
        column_keyboard({{"Главное меню", "menu_main"}}), message_id);

    const WordGenerationResult generation =
        add_generated_words_to_db(chat_id, database, ai, topic_keyword, 10);
    if (generation.added > 0) {
        std::string note = "Добавлено: " + std::to_string(generation.added) + ".";
        if (generation.fallback_added > 0) {
            note += " Добрано из резервного словаря: " + std::to_string(generation.fallback_added) +
                    ".";
        }
        if (generation.added < 10) {
            note += " Не хватило уникальных слов даже после дополнительных попыток и резервного "
                    "словаря.";
            if (generation.duplicates > 0) {
                note += " Дубли: " + std::to_string(generation.duplicates) + ".";
            }
            if (generation.rejected_quality > 0) {
                note += " Отсеяно: " + std::to_string(generation.rejected_quality) + ".";
            }
        }
        show_status_screen(chat_id, bot, "generate_done_" + topic_keyword, "Слова добавлены",
                           "Новая подборка готова.", note,
                           column_keyboard({{"Словарь для изучения", "menu_dictionary"},
                                            {"Добавить слова", "menu_new_words"},
                                            {"Главное меню", "menu_main"}}));
        return;
    }

    std::string note = "Попробуй другую тему.";
    if (generation.duplicates > 0) {
        note = "AI предложил только слова, которые уже есть в словаре. " + note;
    } else if (generation.parsed_total == 0) {
        note = "AI вернул ответ в неожиданном формате. " + note;
    } else if (generation.rejected_quality > 0) {
        note = "Все новые варианты не прошли фильтр качества. " + note;
    }
    show_status_screen(
        chat_id, bot, "generate_failed_" + topic_keyword, "Не удалось добавить",
        "Новых слов сейчас нет.", note,
        column_keyboard({{"Добавить слова", "menu_new_words"}, {"Главное меню", "menu_main"}}));
}

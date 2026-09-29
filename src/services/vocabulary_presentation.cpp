#include "services/vocabulary_presentation.h"

#include "database.h"
#include "presentation/bot_presentation.h"
#include "presentation/bot_state.h"
#include "presentation/screen_components.h"
#include "services/course_catalog.h"
#include "services/vocabulary_service.h"

void generate_words(long long chat_id, TelegramClient& bot, Database& database, GroqClient& ai,
                    const std::string& topic_keyword, const std::string&, int message_id) {
    const auto course = canonical_course(topic_keyword);
    if (course.empty() || !database.set_active_course(chat_id, course)) {
        show_status_screen(chat_id, bot, "course_error", "Не удалось выбрать курс",
                           "Попробуйте ещё раз.", "Слова не добавлены.",
                           column_keyboard({{"Направления", "menu_new_words"}}), message_id);
        return;
    }
    remember_screen_context(chat_id, "generation");
    const auto result = add_generated_words_to_db(chat_id, database, ai, course, 5);
    const std::string title =
        result.failed > 0
            ? "Не удалось добавить слова"
            : (result.added == 0 ? "Все слова курса уже добавлены" : "Слова добавлены");
    std::string note = result.failed > 0
                           ? "Не удалось прочитать каталог или сохранить слова. Попробуйте позже."
                           : "Добавлено: " + std::to_string(result.added) +
                                 ". Прочитайте примеры вслух и составьте свои предложения. Уже "
                                 "знакомые слова можно сразу отметить выученными.";
    if (result.exhausted)
        note += " Каталог исчерпан: продолжайте повторение и разговорную практику.";
    show_status_screen(chat_id, bot, "course_result_" + course, title, course_title(course), note,
                       column_keyboard({{"Слова, которые я ещё учу", "menu_dictionary"},
                                        {"Практика с AI", "practice_course"},
                                        {"Ещё 5 слов", "add_" + course},
                                        {"Другие направления", "menu_new_words"},
                                        {"Главное меню", "menu_main"}}),
                       message_id);
}

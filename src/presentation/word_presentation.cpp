#include "pagination.h"
#include "presentation/bot_presentation.h"
#include "presentation/bot_state.h"
#include "presentation/screen_components.h"
#include "presentation/screen_transport.h"
#include "rendering/bot_renderer.h"

void show_dictionary_page(long long chat_id, TelegramClient& bot,
                          const std::vector<WordView>& words, int requested_page, int& current_page,
                          std::string& last_action, int& message_id, bool is_new,
                          const std::string& filter) {
    const auto page = presentation::detail::paginate(words.size(), requested_page);
    current_page = page.page;
    last_action = "dictionary";
    remember_screen_context(chat_id, "dictionary");
    auto buttons = course_filter_keyboard("dict_filter_", filter);
    const auto navigation =
        words.empty()
            ? column_keyboard({{"Добавить новые слова", "menu_new_words"}, {"Главное меню", "menu_main"}})
            : page_keyboard("dict_", page.page, page.total_pages);
    buttons.insert(buttons.end(), navigation.begin(), navigation.end());

    if (words.empty()) {
        const std::string message = "📚 *" + course_filter_title(filter) +
                                    "*\n\nВ этом фильтре нет слов для изучения. Выберите другое "
                                    "направление или добавьте слова.";
        upsert_screen(chat_id, bot, message, buttons, is_new ? 0 : message_id);
        return;
    }

    const std::string image_path = render_dictionary_words_image(
        chat_id, words, page.page, page.total_pages, page.start, page.end, filter);
    if (image_path.empty()) {
        std::string message = "📚 *Слова, которые я ещё учу · " + course_filter_title(filter) + "*\n";
        message += "▫️ Страница " + std::to_string(page.page + 1) + " из " +
                   std::to_string(page.total_pages) + "\n";
        message += "▫️ Чтобы отметить слово или фразу — напишите их в "
                   "чат\n\n";
        for (int index = page.start; index < page.end; ++index) {
            const auto& [english, translation, learned, pronunciation, transcription, definition] =
                words[static_cast<std::size_t>(index)];
            message += format_word(english, translation, transcription, pronunciation, definition) +
                       "\n\n";
        }
        upsert_screen(chat_id, bot, message, buttons, is_new ? 0 : message_id);
        return;
    }

    upsert_photo_screen(chat_id, bot, image_path, buttons, is_new ? 0 : message_id,
                        screen_caption("Слова, которые я ещё учу · " + course_filter_title(filter),
                                       page.page, page.total_pages));
}

void show_learned_page(long long chat_id, TelegramClient& bot, const std::vector<WordView>& words,
                       int requested_page, int& current_page, std::string& last_action,
                       int& message_id, bool is_new, const std::string& filter) {
    const auto page = presentation::detail::paginate(words.size(), requested_page);
    current_page = page.page;
    last_action = "learned";
    remember_screen_context(chat_id, "learned");
    auto buttons = course_filter_keyboard("learn_filter_", filter);
    const auto navigation = words.empty()
                                ? column_keyboard({{"Слова, которые я ещё учу", "menu_dictionary"},
                                                   {"Главное меню", "menu_main"}})
                                : page_keyboard("learn_", page.page, page.total_pages);
    buttons.insert(buttons.end(), navigation.begin(), navigation.end());

    if (words.empty()) {
        const std::string message =
            "✅ *" + course_filter_title(filter) + "*\n\nВыученных слов в этом фильтре пока нет.";
        upsert_screen(chat_id, bot, message, buttons, is_new ? 0 : message_id);
        return;
    }

    const std::string image_path = render_learned_words_image(
        chat_id, words, page.page, page.total_pages, page.start, page.end, filter);
    if (image_path.empty()) {
        const std::string message =
            "*Словарь выученных слов*\n\nНе удалось собрать картинку. Попробуйте открыть раздел еще раз.";
        upsert_screen(chat_id, bot, message, buttons, is_new ? 0 : message_id);
        return;
    }

    upsert_photo_screen(
        chat_id, bot, image_path, buttons, is_new ? 0 : message_id,
        screen_caption("Словарь выученных слов · " + course_filter_title(filter), page.page, page.total_pages));
}

bool show_daily_review_page(long long chat_id, TelegramClient& bot,
                            const std::vector<WordView>& words, int requested_page, int message_id,
                            bool is_new, std::string* last_action, const std::string& filter) {
    const auto page = presentation::detail::paginate(words.size(), requested_page);
    if (last_action != nullptr) {
        *last_action = "daily";
    }
    remember_screen_context(chat_id, "daily");

    if (words.empty()) {
        return show_status_screen(
            chat_id, bot, "daily_empty", "Доброе утро", "Пока нет выученных слов для повторения.",
            "Отметьте несколько слов как выученные, и завтра утренний экран покажет их для практики.",
            column_keyboard(
                {{"Словарь выученных слов", "menu_learned"}, {"Главное меню", "menu_main"}}),
            is_new ? 0 : message_id);
    }

    const std::string image_path = render_daily_review_image(
        chat_id, words, page.page, page.total_pages, page.start, page.end, filter);
    auto buttons = page_keyboard("daily_", page.page, page.total_pages);
    buttons.insert(buttons.begin(), InlineKeyboard::value_type{{"Добавить новые слова", "menu_new_words"}});
    buttons.push_back({{"Настроить напоминания", "menu_reminders"}});
    const std::string caption = "Доброе утро! Давайте повторим то, что уже выучено.";
    if (image_path.empty()) {
        std::string message = caption + "\n\n";
        for (int i = page.start; i < page.end; ++i) {
            const auto& word = words[static_cast<std::size_t>(i)];
            message += format_word(word.english, word.translation, word.transcription,
                                   word.pronunciation, word.definition) + "\n\n";
        }
        return upsert_screen(chat_id, bot, message, buttons, message_id, is_new);
    }

    return upsert_photo_screen(
        chat_id, bot, image_path, buttons, is_new ? 0 : message_id,
        caption, "", is_new);
}

bool show_evening_words_page(long long chat_id, TelegramClient& bot,
                             const std::vector<WordView>& words, int requested_page, int message_id,
                             bool is_new, std::string* last_action, const std::string& filter,
                             bool new_words) {
    const auto page = presentation::detail::paginate(words.size(), requested_page);
    if (last_action != nullptr) {
        *last_action = "evening";
    }
    remember_screen_context(chat_id, "evening");

    if (words.empty()) {
        return show_status_screen(
            chat_id, bot, "evening_empty", "Вечернее занятие", "Сейчас нет слов для изучения.",
            "Выберите тему вручную, чтобы добавить новую подборку.",
            column_keyboard({{"Добавить новые слова", "menu_new_words"}, {"Главное меню", "menu_main"}}),
            is_new ? 0 : message_id);
    }

    const std::string image_path = render_evening_words_image(
        chat_id, words, page.page, page.total_pages, page.start, page.end, filter);
    auto buttons = new_words
        ? column_keyboard({{"Слова, которые я ещё учу", "menu_dictionary"}, {"Главное меню", "menu_main"}})
        : page_keyboard("evening_", page.page, page.total_pages);
    buttons.push_back({{"Настроить напоминания", "menu_reminders"}});
    std::string caption = new_words
        ? "Добрый вечер! Давайте познакомимся с новыми словами.\nСлов на сегодня: " +
              std::to_string(words.size()) + ". Прочитайте примеры вслух."
        : "Добрый вечер! Давайте повторим слова, которые сейчас изучаем. Прочитайте примеры вслух.";
    caption += "\n\nВыучили слово или фразу? Отправьте его на английском прямо в этот чат — "
               "я отмечу его как выученное и добавлю в словарь выученных слов.";
    if (image_path.empty()) {
        std::string message = caption + "\n\n";
        for (int i = page.start; i < page.end; ++i) {
            const auto& word = words[static_cast<std::size_t>(i)];
            message += format_word(word.english, word.translation, word.transcription,
                                   word.pronunciation, word.definition) + "\n\n";
        }
        return upsert_screen(chat_id, bot, message, buttons, message_id, is_new);
    }

    return upsert_photo_screen(chat_id, bot, image_path, buttons, is_new ? 0 : message_id,
                               caption, "", is_new);
}

#include "pagination.h"
#include "presentation/bot_presentation.h"
#include "presentation/bot_state.h"
#include "presentation/screen_components.h"
#include "presentation/screen_transport.h"
#include "rendering/bot_renderer.h"

void show_dictionary_page(long long chat_id, TelegramClient& bot,
                          const std::vector<WordView>& words, int requested_page, int& current_page,
                          std::string& last_action, int& message_id, bool is_new) {
    const auto page = presentation::detail::paginate(words.size(), requested_page);
    current_page = page.page;
    last_action = "dictionary";
    remember_screen_context(chat_id, "dictionary");

    if (words.empty()) {
        const std::string message =
            "📚 *Словарь пуст*\n\nНажми «Добавить слова», чтобы пополнить словарь.";
        upsert_screen(
            chat_id, bot, message,
            column_keyboard({{"Добавить слова", "menu_new_words"}, {"Главное меню", "menu_main"}}),
            is_new ? 0 : message_id);
        return;
    }

    const InlineKeyboard buttons = page_keyboard("dict_", page.page, page.total_pages);
    const std::string image_path = render_dictionary_words_image(
        chat_id, words, page.page, page.total_pages, page.start, page.end);
    if (image_path.empty()) {
        std::string message = "📚 *Словарь для изучения*\n";
        message += "▫️ Страница " + std::to_string(page.page + 1) + " из " +
                   std::to_string(page.total_pages) + "\n";
        message +=
            "▫️ Чтобы отметить слово - просто напиши его\n\n";
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
                        screen_caption("Словарь для изучения", page.page, page.total_pages));
}

void show_learned_page(long long chat_id, TelegramClient& bot, const std::vector<WordView>& words,
                       int requested_page, int& current_page, std::string& last_action,
                       int& message_id, bool is_new) {
    const auto page = presentation::detail::paginate(words.size(), requested_page);
    current_page = page.page;
    last_action = "learned";
    remember_screen_context(chat_id, "learned");

    if (words.empty()) {
        const std::string message = "✅ *Выученных слов пока нет*\n\n📚 Учи слова из словаря!";
        upsert_screen(chat_id, bot, message,
                      column_keyboard({{"Словарь для изучения", "menu_dictionary"},
                                       {"Главное меню", "menu_main"}}),
                      is_new ? 0 : message_id);
        return;
    }

    const std::string image_path = render_learned_words_image(
        chat_id, words, page.page, page.total_pages, page.start, page.end);
    const InlineKeyboard buttons = page_keyboard("learn_", page.page, page.total_pages);
    if (image_path.empty()) {
        const std::string message =
            "*Выученные слова*\n\nНе удалось собрать картинку. Попробуй открыть раздел еще раз.";
        upsert_screen(chat_id, bot, message, buttons, is_new ? 0 : message_id);
        return;
    }

    upsert_photo_screen(chat_id, bot, image_path, buttons, is_new ? 0 : message_id,
                        screen_caption("Словарь для повторения", page.page, page.total_pages));
}

bool show_daily_review_page(long long chat_id, TelegramClient& bot,
                            const std::vector<WordView>& words, int requested_page, int message_id,
                            bool is_new, std::string* last_action) {
    const auto page = presentation::detail::paginate(words.size(), requested_page);
    if (last_action != nullptr) {
        *last_action = "daily";
    }
    remember_screen_context(chat_id, "daily");

    if (words.empty()) {
        return show_status_screen(
            chat_id, bot, "daily_empty", "Доброе утро", "Пока нет выученных слов для повторения.",
            "Отметь несколько слов как выученные, и завтра утренний экран покажет их для практики.",
            column_keyboard(
                {{"Словарь для повторения", "menu_learned"}, {"Главное меню", "menu_main"}}),
            is_new ? 0 : message_id);
    }

    const std::string image_path = render_daily_review_image(
        chat_id, words, page.page, page.total_pages, page.start, page.end);
    const InlineKeyboard buttons = page_keyboard("daily_", page.page, page.total_pages);
    if (image_path.empty()) {
        const std::string message =
            "*Доброе утро*\n\nНе удалось собрать картинку повторения. Попробуй открыть раздел еще "
            "раз.";
        return upsert_screen(chat_id, bot, message, buttons, is_new ? 0 : message_id);
    }

    return upsert_photo_screen(chat_id, bot, image_path, buttons, is_new ? 0 : message_id,
                               screen_caption("Утреннее повторение", page.page, page.total_pages,
                                              "Прочитай слова и проговори их вслух."));
}

bool show_evening_words_page(long long chat_id, TelegramClient& bot,
                             const std::vector<WordView>& words, int requested_page, int message_id,
                             bool is_new, std::string* last_action) {
    const auto page = presentation::detail::paginate(words.size(), requested_page);
    if (last_action != nullptr) {
        *last_action = "evening";
    }
    remember_screen_context(chat_id, "evening");

    if (words.empty()) {
        return show_status_screen(
            chat_id, bot, "evening_empty", "Новые слова", "Сейчас нет слов для изучения.",
            "Выбери тему вручную, чтобы добавить новую подборку.",
            column_keyboard({{"Добавить слова", "menu_new_words"}, {"Главное меню", "menu_main"}}),
            is_new ? 0 : message_id);
    }

    const std::string image_path = render_evening_words_image(
        chat_id, words, page.page, page.total_pages, page.start, page.end);
    const InlineKeyboard buttons = page_keyboard("evening_", page.page, page.total_pages);
    if (image_path.empty()) {
        const std::string message =
            "*Новые слова*\n\nНе удалось собрать картинку. Попробуй открыть раздел еще раз.";
        return upsert_screen(chat_id, bot, message, buttons, is_new ? 0 : message_id);
    }

    return upsert_photo_screen(chat_id, bot, image_path, buttons, is_new ? 0 : message_id,
                               screen_caption("Вечерняя подборка", page.page, page.total_pages,
                                              "Выучил слово — отправь его боту текстом."));
}

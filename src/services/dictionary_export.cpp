#include "services/dictionary_export.h"
#include "database.h"
#include "telegram_client.h"
#include "logger.h"
#include "presentation/bot_state.h"
#include "presentation/screen_components.h"
#include "presentation/screen_transport.h"
#include "rendering/bot_renderer.h"
#include "services/course_catalog.h"
#include <cerrno>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <unistd.h>
#include <sys/wait.h>

namespace {
std::string export_filename(DictionaryExportKind kind) {
    return kind == DictionaryExportKind::Conversation ? "conversation-2000.pdf" : "learned-dictionary.pdf";
}
std::string export_title(DictionaryExportKind kind) {
    return kind == DictionaryExportKind::Conversation ? "Разговорный английский · 2000 слов" : "Мой выученный словарь";
}
std::string escape(const std::string& value) {
    std::string result;
    for (const unsigned char c : value) {
        switch (c) {
        case '&': result += "&amp;"; break;
        case '<': result += "&lt;"; break;
        case '>': result += "&gt;"; break;
        case '"': result += "&quot;"; break;
        default: if (c >= 32 || c == '\n' || c == '\t') result += static_cast<char>(c);
        }
    }
    return result;
}
struct TemporaryDirectory {
    std::filesystem::path path;
    TemporaryDirectory() {
        auto pattern = (std::filesystem::temp_directory_path() / "mentor-export-XXXXXX").string();
        if (!mkdtemp(pattern.data())) throw std::runtime_error("Cannot create export directory");
        path = pattern;
    }
    ~TemporaryDirectory() {
        std::error_code error;
        std::filesystem::remove_all(path, error);
    }
};
}

std::string dictionary_export_html(const std::vector<Word>& words, DictionaryExportKind kind) {
    std::string html = R"(<!doctype html><html lang="ru"><head><meta charset="utf-8">
<style>body{font-family:'DejaVu Sans',sans-serif;font-size:11pt;color:#111;line-height:1.45}
h1{font-size:22pt}article{page-break-inside:avoid;border-bottom:1px solid #bbb;padding:10px 0}
h2{font-size:13pt;margin:0 0 4px}p{margin:4px 0;white-space:pre-wrap;word-wrap:break-word}
.meta{font-size:10pt;color:#444}</style></head><body><h1>)";
    html += escape(export_title(kind)) + "</h1><p>" +
            (kind == DictionaryExportKind::Learned ? "Все направления · " : "Полный каталог · ") + "Слов: ";
    html += std::to_string(words.size()) + "</p>";
    std::size_t number = 0;
    for (const auto& word : words) {
        html += "<article><h2>" + std::to_string(++number) + ". " + escape(word.english) +
                " — " + escape(word.translation) + "</h2>";
        if (!word.transcription.empty() || !word.pronunciation.empty())
            html += "<p class=\"meta\">" + escape(word.transcription) + " · " +
                    escape(word.pronunciation) + "</p>";
        if (!word.definition.empty()) html += "<p>" + escape(word.definition) + "</p>";
        html += "</article>";
    }
    return html + "</body></html>";
}

void render_dictionary_pdf(const std::vector<Word>& words, const std::filesystem::path& directory,
                           DictionaryExportKind kind) {
    const auto source = (directory / "dictionary.html").string();
    const auto output = (directory / export_filename(kind)).string();
    std::ofstream file(source);
    file << dictionary_export_html(words, kind);
    file.close();
    if (!file) throw std::runtime_error("Cannot write export HTML");
    const pid_t child = fork();
    if (child == -1) throw std::runtime_error("Cannot start PDF renderer");
    if (child == 0) {
        execlp("timeout", "timeout", "45s", "wkhtmltopdf", "--quiet", "--encoding", "UTF-8",
               "--disable-javascript", "--disable-local-file-access", "--page-size", "A4",
               "--margin-top", "15mm", "--margin-bottom", "15mm", "--margin-left", "15mm",
               "--margin-right", "15mm", source.c_str(), output.c_str(), static_cast<char*>(nullptr));
        _exit(127);
    }
    int status = 0;
    pid_t waited;
    do { waited = waitpid(child, &status, 0); } while (waited == -1 && errno == EINTR);
    if (waited != child || !WIFEXITED(status) || WEXITSTATUS(status) != 0)
        throw std::runtime_error("PDF renderer failed");
    const auto size = std::filesystem::file_size(output);
    if (size < 5 || size > 49 * 1024 * 1024) throw std::runtime_error("Invalid PDF size");
    std::ifstream pdf(output, std::ios::binary);
    char signature[5]{};
    pdf.read(signature, 5);
    if (std::string(signature, 5) != "%PDF-") throw std::runtime_error("Invalid PDF output");
}

std::vector<Word> conversation_words_for_export() {
    const auto catalog = load_course_catalog("conversation");
    std::vector<Word> words;
    words.reserve(catalog.size());
    for (const auto& word : catalog)
        words.push_back({word.english, word.translation, false, word.pronunciation,
                         word.transcription, format_course_word_definition(word)});
    return words;
}

void show_dictionary_export_menu(long long chat_id, TelegramClient& bot, int message_id) {
    remember_screen_context(chat_id, "export");
    const auto buttons = column_keyboard({{"Все 2000 разговорных слов", "export_conversation_pdf"},
                                          {"Только выученные слова", "export_learned_pdf"},
                                          {"Главное меню", "menu_main"}});
    const auto image = render_dictionary_export_image();
    if (!image.empty()) {
        upsert_photo_screen(chat_id, bot, image, buttons, message_id,
                            screen_caption("Скачать словарь в PDF"));
        return;
    }
    upsert_screen(chat_id, bot,
        "*Скачать словарь в PDF*\n\n"
        "*Все 2000 разговорных слов* — полный разговорный курс по порядку.\n\n"
        "*Только выученные слова* — Ваши выученные слова из всех направлений, по алфавиту.\n\n"
        "Переводы, произношение и примеры в документе для печати на A4.",
        buttons, message_id);
}

void send_dictionary_pdf(long long chat_id, TelegramClient& bot, Database& database,
                         DictionaryExportKind kind) {
    try {
        const auto words = kind == DictionaryExportKind::Conversation
                               ? conversation_words_for_export()
                               : database.get_learned_words_for_export(chat_id);
        if (words.empty()) {
            int hint_id = 0;
            bot.send_message(chat_id, "Пока нет выученных слов. Отметьте слова как выученные и скачайте словарь снова.", "", &hint_id);
            remember_export_message(chat_id, hint_id);
            return;
        }
        TemporaryDirectory directory;
        render_dictionary_pdf(words, directory.path, kind);
        int message_id = 0;
        if (!bot.send_document(chat_id, (directory.path / export_filename(kind)).string(),
                export_title(kind) + " · Слов: " + std::to_string(words.size()) +
                "\nСохраните PDF для печати на A4.",
                &message_id))
            throw std::runtime_error("Document upload failed");
        remember_export_message(chat_id, message_id);
    } catch (const std::exception& error) {
        LOG_ERROR("Dictionary export failed: " + std::string(error.what()));
        int hint_id = 0;
        bot.send_message(chat_id, "Не удалось подготовить или отправить словарь. Попробуйте ещё раз позже.", "", &hint_id);
        remember_export_message(chat_id, hint_id);
    }
}

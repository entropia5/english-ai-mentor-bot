#include "services/dictionary_export.h"
#include "presentation/screen_components.h"
#include "database.h"
#include "services/course_catalog.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <stdexcept>
#include <unistd.h>

int main() {
    const auto directory = std::filesystem::temp_directory_path() /
                           ("mentor-pdf-test-" + std::to_string(getpid()));
    try {
        std::vector<Word> words;
        for (int i = 0; i < 160; ++i)
            words.push_back({"word " + std::to_string(i), "Перевод", true, "уо́рд", "/wɜːd/",
                "IT\nПример: A <script> & example · ≈ эгза́мпл\nПеревод: Пример предложения."});
        const auto html = dictionary_export_html(words);
        if (html.find("<script>") != std::string::npos ||
            html.find("&lt;script&gt; &amp;") == std::string::npos ||
            html.find("word 159") == std::string::npos)
            throw std::runtime_error("Incomplete or unsafe export");
        bool found = false;
        for (const auto& row : main_menu_keyboard())
            for (const auto& button : row) found |= button.second == "menu_export_pdf";
        if (!found) throw std::runtime_error("Missing export button");
        Database disconnected;
        bool failed = false;
        try { disconnected.get_learned_words_for_export(1); }
        catch (const std::exception&) { failed = true; }
        if (!failed) throw std::runtime_error("DB failure disguised as empty dictionary");
        std::filesystem::create_directories(directory);
        render_dictionary_pdf(words, directory);
        std::ifstream pdf(directory / "learned-dictionary.pdf", std::ios::binary);
        const std::string bytes((std::istreambuf_iterator<char>(pdf)), std::istreambuf_iterator<char>());
        const std::regex page(R"(/Type\s*/Page\b)");
        const auto pages = std::distance(std::sregex_iterator(bytes.begin(), bytes.end(), page),
                                         std::sregex_iterator());
        if (pages < 2) throw std::runtime_error("Long dictionary did not span multiple pages");
        std::cout << "Pages: " << pages << '\n';
        std::cout << "PDF with 160 entries: " << directory / "learned-dictionary.pdf" << '\n';
        const auto catalog = load_course_catalog("conversation");
        const auto conversation = conversation_words_for_export();
        if (conversation.size() != 2000) throw std::runtime_error("Incomplete conversation export");
        for (std::size_t i = 0; i < catalog.size(); ++i) {
            if (conversation[i].english != catalog[i].english ||
                conversation[i].definition != format_course_word_definition(catalog[i]) ||
                conversation[i].learned)
                throw std::runtime_error("Conversation entry changed or missing");
        }
        const auto full_html = dictionary_export_html(conversation, DictionaryExportKind::Conversation);
        if (full_html.find("Мой выученный словарь") != std::string::npos ||
            full_html.find("2000") == std::string::npos)
            throw std::runtime_error("Incorrect conversation title");
        render_dictionary_pdf(conversation, directory, DictionaryExportKind::Conversation);
        if (!std::filesystem::exists(directory / "conversation-2000.pdf"))
            throw std::runtime_error("Full catalog PDF missing");
        std::cout << "Full 2000-word PDF generated successfully\n";
        std::filesystem::remove_all(directory);
        return 0;
    } catch (const std::exception& error) {
        std::filesystem::remove_all(directory);
        std::cerr << error.what() << '\n';
        return 1;
    }
}

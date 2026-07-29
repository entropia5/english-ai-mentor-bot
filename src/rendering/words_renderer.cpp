#include "bot_renderer_internal.h"
#include "core/text.h"
#include "file_utils.h"
#include "rendering/bot_renderer.h"

#include <cstdint>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

namespace {

std::uint64_t fnv1a_update(std::uint64_t hash, const std::string& text) {
    constexpr std::uint64_t prime = 1099511628211ULL;
    for (const unsigned char character : text) {
        hash ^= character;
        hash *= prime;
    }
    return hash;
}

std::string words_page_fingerprint(const std::vector<WordView>& words, int page, int total,
                                   int start, int end) {
    std::uint64_t hash = 1469598103934665603ULL;
    hash = fnv1a_update(hash, std::to_string(page));
    hash = fnv1a_update(hash, std::to_string(total));
    hash = fnv1a_update(hash, std::to_string(words.size()));
    for (int index = start; index < end; ++index) {
        const WordView& word = words[static_cast<std::size_t>(index)];
        hash = fnv1a_update(hash, word.english);
        hash = fnv1a_update(hash, word.translation);
        hash = fnv1a_update(hash, word.pronunciation);
        hash = fnv1a_update(hash, word.transcription);
        hash = fnv1a_update(hash, word.definition);
    }

    std::ostringstream result;
    result << std::hex << hash;
    return result.str();
}

std::string render_phonetics(const std::string& transcription, const std::string& pronunciation) {
    std::string ipa = trim(transcription);
    if (!ipa.empty() && ipa.front() != '/' && ipa.front() != '[') {
        ipa = "/" + ipa + "/";
    }
    const std::string pronunciation_ru = trim(pronunciation);
    if (ipa.empty() && pronunciation_ru.empty()) {
        return {};
    }

    std::string html = "<span class=\"phonetics\">";
    if (!ipa.empty()) {
        html += "<span class=\"ipa\">" + html_escape(ipa) + "</span>";
    }
    if (!pronunciation_ru.empty()) {
        html += "<span class=\"pron\">" + html_escape(pronunciation_ru) + "</span>";
    }
    return html + "</span>";
}

std::string render_word_items(const std::vector<WordView>& words, int start, int end,
                              bool checked_marker) {
    std::string items;
    for (int index = start; index < end; ++index) {
        const WordView& word = words[static_cast<std::size_t>(index)];
        const std::string definition = trim(word.definition);
        const TemplateValues values = {
            {"marker_class", checked_marker ? "" : " study"},
            {"english", word.english},
            {"phonetics", render_phonetics(word.transcription, word.pronunciation)},
            {"translation", word.translation},
            {"definition", definition.empty()
                               ? ""
                               : "<div class=\"definition\">" + html_escape(definition) + "</div>"},
        };
        const RenderedTemplate item = render_template("word_item.html", values);
        if (!item) {
            return {};
        }
        items += item.html;
    }
    return items;
}

std::string render_words_card_image(long long chat_id, const std::vector<WordView>& words, int page,
                                    int total, int start, int end, const std::string& folder,
                                    const std::string& title, const std::string& subtitle,
                                    const std::string& footer_left, bool checked_marker = true) {
    const fs::path render_dir =
        fs::path(project_data_dir()) / "rendered" / folder / std::to_string(chat_id);
    rendering::detail::cleanup_stale_page_artifacts(render_dir, total);

    const std::string items = render_word_items(words, start, end, checked_marker);
    if (items.empty() && start < end) {
        return {};
    }

    const TemplateValues values = {
        {"title", title},
        {"subtitle", subtitle},
        {"page_badge", std::to_string(page + 1) + " / " + std::to_string(total)},
        {"word_items", items},
        {"footer_left", footer_left},
        {"word_count", std::to_string(words.size())},
    };
    const std::string data_fingerprint = folder + "-" + (checked_marker ? "checked-" : "study-") +
                                         words_page_fingerprint(words, page, total, start, end);
    return rendering::detail::render_screen(render_dir / ("page_" + std::to_string(page + 1)),
                                            "words_card.html", values, data_fingerprint,
                                            folder + " words", 1440);
}

} // namespace

std::string render_dictionary_words_image(long long chat_id, const std::vector<WordView>& words,
                                          int page, int total, int start, int end) {
    return render_words_card_image(
        chat_id, words, page, total, start, end, "dictionary", "Словарь для изучения",
        "Напиши слово в чат, чтобы отметить его выученным", "by entropia5", false);
}

std::string render_learned_words_image(long long chat_id, const std::vector<WordView>& words,
                                       int page, int total, int start, int end) {
    return render_words_card_image(
        chat_id, words, page, total, start, end, "learned", "Выученные слова",
        "Словарь для повторения и закрепления выученных слов", "by entropia5");
}

std::string render_daily_review_image(long long chat_id, const std::vector<WordView>& words,
                                      int page, int total, int start, int end) {
    return render_words_card_image(chat_id, words, page, total, start, end, "daily", "Доброе утро",
                                   "Повторение выученных слов", "by entropia5");
}

std::string render_evening_words_image(long long chat_id, const std::vector<WordView>& words,
                                       int page, int total, int start, int end) {
    return render_words_card_image(chat_id, words, page, total, start, end, "evening",
                                   "Новые слова", "Вечерняя подборка для изучения", "by entropia5");
}

std::string format_word(const std::string& english, const std::string& translation,
                        const std::string& transcription, const std::string& pronunciation_ru,
                        const std::string& definition) {
    std::string result = "🇺🇸 *" + english + "*\n";
    if (!transcription.empty()) {
        result += "🏳️ " + transcription + "\n";
    }
    if (!pronunciation_ru.empty()) {
        result += "🏴 " + pronunciation_ru + "\n";
    }
    result += "🇷🇺 " + translation;
    if (!trim(definition).empty()) {
        result += "\nСмысл: " + trim(definition);
    }
    return result;
}

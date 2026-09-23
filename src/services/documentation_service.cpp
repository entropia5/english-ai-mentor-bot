#include "services/documentation_service.h"

#include "database.h"
#include "logger.h"
#include "rendering/bot_renderer.h"
#include "services/dictionary_service.h"

#include <algorithm>
#include <filesystem>
#include <string>
#include <tuple>
#include <vector>

namespace fs = std::filesystem;

bool refresh_doc_screenshots(Database& database, long long chat_id) {
    if (chat_id <= 0) {
        LOG_ERROR("--refresh-doc-screenshots requires a chat id or USER_1_ID in .env");
        return false;
    }

    const fs::path docs_dir = "docs/ui";
    std::error_code error;
    fs::create_directories(docs_dir, error);
    if (error) {
        LOG_ERROR("Failed to create docs/ui directory: " + error.message());
        return false;
    }

    auto copy_png = [&](const std::string& source, const std::string& name) {
        if (source.empty() || !fs::exists(source)) {
            LOG_ERROR("Cannot refresh docs screenshot " + name + ": source image is missing");
            return false;
        }
        fs::copy_file(source, docs_dir / name, fs::copy_options::overwrite_existing, error);
        if (error) {
            LOG_ERROR("Failed to copy docs screenshot " + name + ": " + error.message());
            error.clear();
            return false;
        }
        return true;
    };

    bool ok = true;
    const int learned_count = database.get_words_count(chat_id, true);
    ok = copy_png(render_main_menu_image(std::to_string(learned_count)), "menu.png") && ok;
    ok = copy_png(render_topic_menu_image(), "new_words.png") && ok;

    const auto learned = get_learned_words_for_review(chat_id, database);
    const auto not_learned = database.get_user_words_full(chat_id, true);

    auto first_page = [](const std::vector<WordView>& words, int per_page) {
        const int total = std::max(1, (static_cast<int>(words.size()) + per_page - 1) / per_page);
        return std::make_tuple(total, 0, std::min(per_page, static_cast<int>(words.size())));
    };

    if (!learned.empty()) {
        const auto [total, start, end] = first_page(learned, 5);
        ok = copy_png(render_learned_words_image(chat_id, learned, 0, total, start, end),
                      "words2.png") &&
             ok;
    }
    if (!not_learned.empty()) {
        const auto [total, start, end] = first_page(not_learned, 5);
        ok = copy_png(render_dictionary_words_image(chat_id, not_learned, 0, total, start, end),
                      "words.png") &&
             ok;
        ok = copy_png(render_evening_words_image(chat_id, not_learned, 0, total, start, end),
                      "evening.png") &&
             ok;
    }
    if (!learned.empty()) {
        const auto [total, start, end] = first_page(learned, 5);
        ok = copy_png(render_daily_review_image(chat_id, learned, 0, total, start, end),
                      "morning.png") &&
             ok;
    }

    LOG(ok ? "Docs screenshots refreshed" : "Docs screenshot refresh finished with errors");
    return ok;
}

#include "maintenance_commands.h"

#include "app/command_line.h"
#include "app/self_test.h"
#include "database.h"
#include "groq_client.h"
#include "logger.h"
#include "rendering/bot_renderer.h"
#include "rendering/render_preview.h"
#include "services/documentation_service.h"
#include "services/maintenance_service.h"
#include "services/vocabulary_service.h"
#include "user_config.h"

namespace {

bool has_database_maintenance_command(const AppOptions& options) {
    return options.cleanup_database || options.backfill_transcriptions ||
           options.backfill_definitions || options.audit_words || options.cleanup_bad_words ||
           options.refresh_doc_screenshots;
}

} // namespace

namespace app::detail {

CommandResult run_pre_config_command(const AppOptions& options) {
    if (options.self_test) {
        return {true, run_self_tests() ? 0 : 1};
    }
    if (options.cleanup_render_cache) {
        return {true, cleanup_render_cache() ? 0 : 1};
    }
    if (options.render_preview) {
        return {true, render_preview_screens() ? 0 : 1};
    }
    return {};
}

CommandResult run_database_maintenance(const AppOptions& options) {
    if (!has_database_maintenance_command(options)) {
        return {};
    }

    Database database;
    if (!database.connect() || !database.init_tables()) {
        return {true, 1};
    }
    if (options.backfill_transcriptions) {
        GroqClient ai;
        return {true, backfill_missing_pronunciations(database, ai) ? 0 : 1};
    }
    if (options.backfill_definitions) {
        GroqClient ai;
        return {true, backfill_missing_definitions(database, ai) ? 0 : 1};
    }
    if (options.audit_words) {
        return {true, audit_bad_words(database, false) ? 0 : 1};
    }
    if (options.cleanup_bad_words) {
        return {true, audit_bad_words(database, true) ? 0 : 1};
    }
    if (options.refresh_doc_screenshots) {
        const long long chat_id = options.doc_screenshots_chat_id > 0
                                      ? options.doc_screenshots_chat_id
                                      : configured_user_id("USER_1_ID");
        return {true, refresh_doc_screenshots(database, chat_id) ? 0 : 1};
    }

    LOG("Database cleanup finished");
    return {true, 0};
}

} // namespace app::detail

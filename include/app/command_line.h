#pragma once

#include <string>
#include <vector>

struct AppOptions {
    bool cleanup_database = false;
    bool cleanup_render_cache = false;
    bool backfill_transcriptions = false;
    bool backfill_definitions = false;
    bool audit_words = false;
    bool cleanup_bad_words = false;
    bool self_test = false;
    bool render_preview = false;
    bool send_evening_once = false;
    long long evening_chat_id = 0;
    bool refresh_doc_screenshots = false;
    long long doc_screenshots_chat_id = 0;
};

struct CommandLineParseResult {
    AppOptions options;
    std::string error;

    bool ok() const {
        return error.empty();
    }
};

CommandLineParseResult parse_command_line(const std::vector<std::string>& arguments);
CommandLineParseResult parse_command_line(int argc, char* argv[]);

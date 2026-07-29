#include "app/command_line.h"

#include <exception>

namespace {

bool has_value(const std::vector<std::string>& arguments, std::size_t index) {
    return index + 1 < arguments.size() && !arguments[index + 1].empty() &&
           arguments[index + 1].front() != '-';
}

bool parse_optional_chat_id(const std::vector<std::string>& arguments, std::size_t& index,
                            long long& chat_id, std::string& error) {
    if (!has_value(arguments, index)) {
        return true;
    }

    const std::string& value = arguments[++index];
    try {
        std::size_t parsed_characters = 0;
        chat_id = std::stoll(value, &parsed_characters);
        if (parsed_characters != value.size() || chat_id <= 0) {
            error = "Invalid chat id: " + value;
            return false;
        }
    } catch (const std::exception&) {
        error = "Invalid chat id: " + value;
        return false;
    }
    return true;
}

} // namespace

CommandLineParseResult parse_command_line(const std::vector<std::string>& arguments) {
    CommandLineParseResult result;

    for (std::size_t index = 1; index < arguments.size(); ++index) {
        const std::string& argument = arguments[index];
        if (argument == "--cleanup-db") {
            result.options.cleanup_database = true;
        } else if (argument == "--cleanup-render-cache") {
            result.options.cleanup_render_cache = true;
        } else if (argument == "--backfill-transcriptions") {
            result.options.backfill_transcriptions = true;
        } else if (argument == "--backfill-definitions") {
            result.options.backfill_definitions = true;
        } else if (argument == "--audit-words") {
            result.options.audit_words = true;
        } else if (argument == "--cleanup-bad-words") {
            result.options.cleanup_bad_words = true;
        } else if (argument == "--self-test") {
            result.options.self_test = true;
        } else if (argument == "--render-preview") {
            result.options.render_preview = true;
        } else if (argument == "--send-evening-once") {
            result.options.send_evening_once = true;
            if (!parse_optional_chat_id(arguments, index, result.options.evening_chat_id,
                                        result.error)) {
                return result;
            }
        } else if (argument == "--refresh-doc-screenshots") {
            result.options.refresh_doc_screenshots = true;
            if (!parse_optional_chat_id(arguments, index, result.options.doc_screenshots_chat_id,
                                        result.error)) {
                return result;
            }
        } else {
            result.error = "Unknown command-line option: " + argument;
            return result;
        }
    }
    return result;
}

CommandLineParseResult parse_command_line(int argc, char* argv[]) {
    std::vector<std::string> arguments;
    arguments.reserve(static_cast<std::size_t>(argc));
    for (int index = 0; index < argc; ++index) {
        arguments.emplace_back(argv[index]);
    }
    return parse_command_line(arguments);
}

#include "rendering/template_engine.h"

#include "logger.h"
#include "template_resources.h"

#include <cstdint>
#include <filesystem>
#include <iomanip>
#include <sstream>

namespace fs = std::filesystem;

namespace {

void replace_all(std::string& text, const std::string& needle, const std::string& replacement) {
    if (needle.empty()) {
        return;
    }

    std::size_t position = 0;
    while ((position = text.find(needle, position)) != std::string::npos) {
        text.replace(position, needle.size(), replacement);
        position += replacement.size();
    }
}

std::string fingerprint(const std::string& text) {
    constexpr std::uint64_t offset = 1469598103934665603ULL;
    constexpr std::uint64_t prime = 1099511628211ULL;
    std::uint64_t hash = offset;
    for (const unsigned char byte : text) {
        hash ^= byte;
        hash *= prime;
    }

    std::ostringstream result;
    result << std::hex << hash;
    return result.str();
}

bool placeholders_are_known(const std::string& source, const TemplateValues& values) {
    std::size_t position = 0;
    while ((position = source.find("{{", position)) != std::string::npos) {
        const bool raw = source.compare(position, 3, "{{{") == 0;
        const std::size_t name_start = position + (raw ? 3U : 2U);
        const std::string closing = raw ? "}}}" : "}}";
        const std::size_t end = source.find(closing, name_start);
        if (end == std::string::npos ||
            values.find(source.substr(name_start, end - name_start)) == values.end()) {
            return false;
        }
        position = end + closing.size();
    }
    return true;
}

bool is_safe_template_name(const std::string& template_name) {
    const fs::path path(template_name);
    return !template_name.empty() && path == path.filename() && path.extension() == ".html";
}

} // namespace

std::string html_escape(const std::string& text) {
    std::string result;
    result.reserve(text.size());
    for (const char character : text) {
        switch (character) {
        case '&':
            result += "&amp;";
            break;
        case '<':
            result += "&lt;";
            break;
        case '>':
            result += "&gt;";
            break;
        case '"':
            result += "&quot;";
            break;
        case '\'':
            result += "&#39;";
            break;
        default:
            result += character;
            break;
        }
    }
    return result;
}

std::string rendering_resources_dir() {
    return rendering::resources::locate().string();
}

RenderedTemplate render_template(const std::string& template_name, const TemplateValues& values) {
    if (!is_safe_template_name(template_name)) {
        LOG_ERROR("Invalid rendering template name: " + template_name);
        return {};
    }

    const fs::path resources = rendering::resources::locate();
    const fs::path template_path = resources / "templates" / template_name;
    std::string source = rendering::resources::read_file(template_path);
    const std::string theme = rendering::resources::load_styles(resources);
    if (source.empty() || theme.empty()) {
        LOG_ERROR("Rendering resources are missing: " + template_path.string() + " or " +
                  (resources / "styles").string());
        return {};
    }

    replace_all(source, "{{{theme_css}}}", theme);
    if (!placeholders_are_known(source, values)) {
        LOG_ERROR("Unknown or malformed placeholder in rendering template: " +
                  template_path.string());
        return {};
    }
    for (const auto& [name, value] : values) {
        replace_all(source, "{{{" + name + "}}}", value);
        replace_all(source, "{{" + name + "}}", html_escape(value));
    }

    return {source, fingerprint(source)};
}

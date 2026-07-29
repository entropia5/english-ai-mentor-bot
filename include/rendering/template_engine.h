#pragma once

#include <map>
#include <string>

using TemplateValues = std::map<std::string, std::string>;

struct RenderedTemplate {
    std::string html;
    std::string fingerprint;

    explicit operator bool() const {
        return !html.empty();
    }
};

std::string html_escape(const std::string& text);
RenderedTemplate render_template(const std::string& template_name,
                                 const TemplateValues& values = {});
std::string rendering_resources_dir();

#pragma once

#include <string>

class GroqClient {
private:
    std::string api_key;
    std::string model;
    std::string api_url;
    
public:
    GroqClient();
    std::string ask(const std::string& prompt, const std::string& system_prompt = "");
    bool is_available() const { return !api_key.empty(); }
};

#pragma once

#include <string>

class Logger {
private:
    std::string log_file;
    bool console_output = true;
    bool file_output = true;

public:
    Logger(const std::string& filepath = "bot.log");

    void log(const std::string& message);
    void error(const std::string& message);
    void warning(const std::string& message);

    void set_console_output(bool enable) { console_output = enable; }
    void set_file_output(bool enable) { file_output = enable; }
};

// global logger instance
extern Logger g_logger;

// macros for easier logging
#define LOG(msg) g_logger.log(msg)
#define LOG_ERROR(msg) g_logger.error(msg)
#define LOG_WARNING(msg) g_logger.warning(msg)

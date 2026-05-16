#include "logger.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <ctime>

Logger::Logger(const std::string& filepath) : log_file(filepath) {}

void Logger::log(const std::string& message) {
    // get current time
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_tm = std::localtime(&now_time_t);

    std::stringstream ss;
    ss << "[" << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S") << "] " << message;

    std::string formatted = ss.str();

    // terminal output
    if (console_output) {
        std::cout << formatted << std::endl;
    }

    // in file output
    if (file_output && !log_file.empty()) {
        std::ofstream file(log_file, std::ios::app);
        if (file.is_open()) {
            file << formatted << std::endl;
            file.close();
        }
    }
}

void Logger::error(const std::string& message) {
    log("[ERROR] " + message);
}

void Logger::warning(const std::string& message) {
    log("[WARNING] " + message);
}

// define global logger instance
Logger g_logger("bot.log");

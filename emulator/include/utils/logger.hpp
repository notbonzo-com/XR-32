#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <string_view>
#include <mutex>

enum class LogLevel {
    DEBUG,
    WARN,
    ERROR,
    FATAL,
    LOG
};

class Logger {
public:
    Logger(std::ostream& os = std::cout) : outStream(os) {}

    void log(LogLevel level, std::string_view msg) {
        std::lock_guard<std::mutex> lock(logMutex);
        switch (level) {
            case LogLevel::DEBUG:
                outStream << ANSI_BOLD_BLUE << "[DEBUG] " << ANSI_RESET;
                break;
            case LogLevel::WARN:
                outStream << ANSI_BOLD_DIM_YELLOW << "[WARN] " << ANSI_RESET;
                break;
            case LogLevel::ERROR:
                outStream << ANSI_RED << "[ERROR] " << ANSI_RESET;
                break;
            case LogLevel::FATAL:
                outStream << ANSI_BOLD_RED << "[FATAL] " << ANSI_RESET;
                break;
            case LogLevel::LOG:
            default:
                outStream << "[LOG] ";
                break;
        }
        outStream << msg << std::endl;

        if (level == LogLevel::FATAL) {
            std::abort();
        }
    }

    void debug(std::string_view msg) { log(LogLevel::DEBUG, msg); }
    void warn(std::string_view msg) { log(LogLevel::WARN, msg); }
    void error(std::string_view msg) { log(LogLevel::ERROR, msg); }
    void fatal(std::string_view msg) { log(LogLevel::FATAL, msg); }
    void log(std::string_view msg) { log(LogLevel::LOG, msg); }

private:
    std::ostream& outStream;
    std::mutex logMutex;

    static constexpr std::string_view ANSI_RESET = "\033[0m";
    static constexpr std::string_view ANSI_BOLD = "\033[1m";
    static constexpr std::string_view ANSI_DIM = "\033[2m";
    static constexpr std::string_view ANSI_ITALIC = "\033[3m";
    static constexpr std::string_view ANSI_UNDERLINE = "\033[4m";
    static constexpr std::string_view ANSI_BLINK = "\033[5m";
    static constexpr std::string_view ANSI_REVERSE = "\033[7m";
    
    static constexpr std::string_view ANSI_RED = "\033[31m";
    static constexpr std::string_view ANSI_GREEN = "\033[32m";
    static constexpr std::string_view ANSI_YELLOW = "\033[33m";
    static constexpr std::string_view ANSI_BLUE = "\033[34m";
    static constexpr std::string_view ANSI_MAGENTA = "\033[35m";
    
    static constexpr std::string_view ANSI_BOLD_RED = "\033[1;31m";
    static constexpr std::string_view ANSI_BOLD_GREEN = "\033[1;32m";
    static constexpr std::string_view ANSI_BOLD_YELLOW = "\033[1;33m";
    static constexpr std::string_view ANSI_BOLD_BLUE = "\033[1;34m";
    static constexpr std::string_view ANSI_BOLD_MAGENTA = "\033[1;35m";

    static constexpr std::string_view ANSI_BOLD_DIM_RED = "\033[2;31m";
    static constexpr std::string_view ANSI_BOLD_DIM_GREEN = "\033[2;32m";
    static constexpr std::string_view ANSI_BOLD_DIM_YELLOW = "\033[2;33m";
    static constexpr std::string_view ANSI_BOLD_DIM_BLUE = "\033[2;34m";
    static constexpr std::string_view ANSI_BOLD_DIM_MAGENTA = "\033[2;35m";
};

#endif // LOGGER_HPP

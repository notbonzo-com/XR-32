#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string_view>
#include <iostream>

namespace log {

    constexpr std::string_view ANSI_RESET = "\033[0m";
    constexpr std::string_view ANSI_BOLD = "\033[1m";
    constexpr std::string_view ANSI_DIM = "\033[2m";
    constexpr std::string_view ANSI_ITALIC = "\033[3m";
    constexpr std::string_view ANSI_UNDERLINE = "\033[4m";
    constexpr std::string_view ANSI_BLINK = "\033[5m";
    constexpr std::string_view ANSI_REVERSE = "\033[7m";

    constexpr std::string_view ANSI_RED = "\033[31m";
    constexpr std::string_view ANSI_GREEN = "\033[32m";
    constexpr std::string_view ANSI_YELLOW = "\033[33m";
    constexpr std::string_view ANSI_BLUE = "\033[34m";
    constexpr std::string_view ANSI_MAGENTA = "\033[35m";

    constexpr std::string_view ANSI_BOLD_RED = "\033[1;31m";
    constexpr std::string_view ANSI_BOLD_GREEN = "\033[1;32m";
    constexpr std::string_view ANSI_BOLD_YELLOW = "\033[1;33m";
    constexpr std::string_view ANSI_BOLD_BLUE = "\033[1;34m";
    constexpr std::string_view ANSI_BOLD_MAGENTA = "\033[1;35m";

    constexpr std::string_view ANSI_BOLD_DIM_RED = "\033[2;31m";
    constexpr std::string_view ANSI_BOLD_DIM_GREEN = "\033[2;32m";
    constexpr std::string_view ANSI_BOLD_DIM_YELLOW = "\033[2;33m";
    constexpr std::string_view ANSI_BOLD_DIM_BLUE = "\033[2;34m";
    constexpr std::string_view ANSI_BOLD_DIM_MAGENTA = "\033[2;35m";

    static inline void debug(std::string_view msg) {
        std::cout << ANSI_BOLD_BLUE << "[DEBUG] " << ANSI_RESET << msg << std::endl;
    }
    static inline void warn(std::string_view msg) {
        std::cout << ANSI_BOLD_DIM_YELLOW << "[WARN] " << ANSI_RESET << msg << std::endl;
    }
    static inline void error(std::string_view msg) {
        std::cout << ANSI_RED << "[ERROR] " << ANSI_RESET << msg << std::endl;
    }
    static inline void fatal(std::string_view msg) {
        std::cout << ANSI_BOLD_RED << "[FATAL] " << ANSI_RESET << msg << std::endl;
        std::abort();
    }
    static inline void log(std::string_view msg) {
        std::cout << "[LOG] " << msg << std::endl;
    }
}

#endif // LOGGER_HPP

#ifndef ERROR_HPP
#define ERROR_HPP

#include <stdexcept>
#include <string>
#include <format>

/**
 * @brief Base class for all assembler-related exceptions.
 */
class AssemblerException : public std::runtime_error {
public:
    explicit AssemblerException(const std::string& message)
        : std::runtime_error(message) {}
};

/**
 * @brief Exception thrown when there is a parsing error.
 */
class ParseException : public AssemblerException {
public:
    explicit ParseException(const std::string& message)
        : AssemblerException("Parse Error: " + message) {}
};

/**
 * @brief Exception thrown when there is a symbol-related error.
 */
class SymbolException : public AssemblerException {
public:
    explicit SymbolException(const std::string& message)
        : AssemblerException("Symbol Error: " + message) {}
};

/**
 * @brief Exception thrown for general assembler errors.
 */
class GeneralAssemblerException : public AssemblerException {
public:
    explicit GeneralAssemblerException(const std::string& message)
        : AssemblerException("Assembler Error: " + message) {}
};

/**
 * @brief Exception thrown for logical errors in the assembler.
 */
class LogicalAssemblerException : public AssemblerException {
public:
    explicit LogicalAssemblerException(const std::string& message)
        : AssemblerException("Logical Error: " + message) {}
};

/**
 * @brief Utility function to throw an exception with formatted message.
 *
 * This overload allows for throwing an exception with a formatted message using std::format.
 *
 * @tparam ExceptionType The type of exception to throw.
 * @param formatStr The format string.
 * @param args The arguments for the format string.
 */
template <typename ExceptionType, typename... Args>
[[noreturn]] void throwFormatted(const std::string& formatStr, Args&&... args) {
    throw ExceptionType(std::format(formatStr, std::forward<Args>(args)...));
}

/**
 * @brief Utility function to throw a general assembler exception.
 *
 * This overload allows for throwing a GeneralAssemblerException with a formatted message.
 *
 * @param formatStr The format string.
 * @param args The arguments for the format string.
 */
template <typename... Args>
[[noreturn]] void throwGeneralError(const std::string& formatStr, Args&&... args) {
    throwFormatted<GeneralAssemblerException>(formatStr, std::forward<Args>(args)...);
}

/**
 * @brief Utility function to throw a parsing exception.
 *
 * This overload allows for throwing a ParseException with a formatted message.
 *
 * @param formatStr The format string.
 * @param args The arguments for the format string.
 */
template <typename... Args>
[[noreturn]] void throwParseError(const std::string& formatStr, Args&&... args) {
    throwFormatted<ParseException>(formatStr, std::forward<Args>(args)...);
}

/**
 * @brief Utility function to throw a symbol-related exception.
 *
 * This overload allows for throwing a SymbolException with a formatted message.
 *
 * @param formatStr The format string.
 * @param args The arguments for the format string.
 */
template <typename... Args>
[[noreturn]] void throwSymbolError(const std::string& formatStr, Args&&... args) {
    throwFormatted<SymbolException>(formatStr, std::forward<Args>(args)...);
}

/**
 * @brief Utility function to throw a logical assembler exception.
 *
 * This overload allows for throwing a LogicalAssemblerException with a formatted message.
 *
 * @param formatStr The format string.
 * @param args The arguments for the format string.
 */
template <typename... Args>
[[noreturn]] void throwLogicalError(const std::string& formatStr, Args&&... args) {
    throwFormatted<LogicalAssemblerException>(formatStr, std::forward<Args>(args)...);
}

#endif // ERROR_HPP

#ifndef XR32_LEXER_H
#define XR32_LEXER_H

#include <string>
#include <vector>
#include <ranges>
#include <optional>
#include <coroutine>

namespace Assembler {

/**
 * @brief Enum representing different token types in the XR-32 assembly language.
 */
enum class TokenType {
    KEYWORD,         ///< Keyword (e.g., 'mov', 'add', 'jmp')
    IDENTIFIER,      ///< Identifier, such as labels and variable names
    REGISTER,        ///< Register name (e.g., 'r0', 'r1', 's0')
    IMMEDIATE,       ///< Immediate value (e.g., 42, 0xFF)
    SYMBOL,          ///< Symbols like ':', '[', ']', ','
    DIRECTIVE,       ///< Assembler directives (e.g., '.org', 'db', 'dw')
    COMMENT,         ///< Comments (starting with ';')
    END_OF_LINE,     ///< End of a line of code
    END_OF_FILE,     ///< End of the assembly file
    UNKNOWN          ///< Unrecognized token type
};

/**
 * @brief Struct representing a token in the XR-32 assembly language.
 */
struct Token {
    TokenType type;        ///< The type of the token
    std::string value;     ///< The string value of the token
    int line;              ///< Line number where the token was found
    int column;            ///< Column number where the token was found
};

/**
 * @brief Coroutine-based generator class for tokens.
 * 
 * Provides a way to iterate through tokens lazily.
 */
class TokenGenerator {
public:
    struct promise_type {
        Token current_value;

        TokenGenerator get_return_object() {
            return TokenGenerator{
                std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(Token value) {
            current_value = std::move(value);
            return {};
        }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    using handle_type = std::coroutine_handle<promise_type>;

    explicit TokenGenerator(handle_type h) : handle_(h) {}
    TokenGenerator(const TokenGenerator&) = delete;
    TokenGenerator(TokenGenerator&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }
    ~TokenGenerator() {
        if (handle_) handle_.destroy();
    }

    TokenGenerator& operator=(const TokenGenerator&) = delete;
    TokenGenerator& operator=(TokenGenerator&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }

    bool move_next() {
        handle_.resume();
        return !handle_.done();
    }

    Token current() const {
        return handle_.promise().current_value;
    }

private:
    handle_type handle_;
};

/**
 * @brief Lexer class for tokenizing XR-32 assembly language source code.
 */
class XR32Lexer {
public:
    /**
     * @brief Constructs a lexer with the given source code.
     * 
     * @param source The source code to tokenize.
     */
    explicit XR32Lexer(const std::string& source);

    /**
     * @brief Tokenizes the entire input and returns a lazy token generator.
     * 
     * @return A coroutine-based generator for tokens.
     */
    TokenGenerator tokenize();

    /**
     * @brief Resets the lexer to the beginning of the input.
     */
    void reset();

private:
    /**
     * @brief Lexes a keyword or identifier.
     * 
     * @return The next token representing a keyword or identifier.
     */
    Token lexKeywordOrIdentifier();

    /**
     * @brief Lexes a register.
     * 
     * @return The next token representing a register.
     */
    Token lexRegister();

    /**
     * @brief Lexes an immediate value.
     * 
     * @return The next token representing an immediate value.
     */
    Token lexImmediate();

    /**
     * @brief Lexes a symbol.
     * 
     * @return The next token representing a symbol.
     */
    Token lexSymbol();

    /**
     * @brief Lexes a directive.
     * 
     * @return The next token representing a directive.
     */
    Token lexDirective();

    /**
     * @brief Lexes a comment.
     * 
     * @return The next token representing a comment.
     */
    Token lexComment();

    /**
     * @brief Lexes the end of a line.
     * 
     * @return The next token representing the end of a line.
     */
    Token lexEndOfLine();

    /**
     * @brief Enum representing the set of supported keywords.
     */
    enum class Keyword {
        MOV, ADD, SUB, JMP, HLT, LDR, STR, CALL, BEQ, AND, CMP, JNE,
        Count // Total number of keywords
    };

    /**
     * @brief Array mapping keywords to their string representations.
     */
    static constexpr std::array<std::string_view, static_cast<size_t>(Keyword::Count)> keywordToStringArray = {
        "mov", "add", "sub", "jmp", "hlt", "ldr", "str", "call", "beq", "and", "cmp", "jne"
    };

    /**
     * @brief Checks if a given string is a keyword.
     *
     * @param str The string to check.
     * @return True if the string is a keyword, otherwise false.
     */
    constexpr bool isKeyword(const std::string_view str) {
        return std::ranges::find(keywordToStringArray, str) != keywordToStringArray.end();
    }


    /**
     * @brief Utility function to check if a string is a known directive.
     * 
     * @param str The string to check.
     * @return True if the string is a directive, false otherwise.
     */
    bool isDirective(const std::string& str) const;

    // Current position in the source code
    std::string source_; ///< The source code being tokenized.
    size_t position_;    ///< The current position in the source.
    int line_;           ///< The current line number.
    int column_;         ///< The current column number.
};

} // namespace Assembler

#endif // XR32_LEXER_H
#include <Lexer.hpp>
#include <Error.hpp>
#include <Register.hpp>
#include <cctype>
#include <stdexcept>
#include <format>
#include <ranges>

namespace Assembler {

XR32Lexer::XR32Lexer(const std::string& source)
    : source_(source), position_(0), line_(1), column_(1) {}

TokenGenerator XR32Lexer::tokenize() {
    while (position_ < source_.size()) {
        if (std::isspace(source_[position_])) {
            if (source_[position_] == '\n') {
                co_yield lexEndOfLine();
            }
            ++position_;
            ++column_;
            continue;
        }

        if (std::isalpha(source_[position_]) || source_[position_] == '_') {
            co_yield lexKeywordOrIdentifier();
        } else if (source_[position_] == '.') {
            co_yield lexDirective();
        } else if (std::isdigit(source_[position_]) || (source_[position_] == '0' && position_ + 1 < source_.size() && source_[position_ + 1] == 'x')) {
            co_yield lexImmediate();
        } else if (source_[position_] == ';') {
            co_yield lexComment();
        } else if (std::ispunct(source_[position_])) {
            co_yield lexSymbol();
        } else if (source_[position_] == '\0') {
            break;
        } else {
            throwGeneralError("Unknown character '{}' at line {}, column {}", source_[position_], line_, column_);
        }
    }
    co_yield Token{TokenType::END_OF_FILE, "", line_, column_};
}

void XR32Lexer::reset() {
    position_ = 0;
    line_ = 1;
    column_ = 1;
}

Token XR32Lexer::lexKeywordOrIdentifier() {
    size_t start = position_;
    while (position_ < source_.size() && (std::isalnum(source_[position_]) || source_[position_] == '_')) {
        ++position_;
        ++column_;
    }
    std::string value = source_.substr(start, position_ - start);
    if (Register::stringToRegister(value)) {
        return Token{TokenType::REGISTER, value, line_, column_};
    }
    TokenType type = isKeyword(value) ? TokenType::KEYWORD : TokenType::IDENTIFIER;
    return Token{type, value, line_, column_};
}

Token XR32Lexer::lexImmediate() {
    size_t start = position_;
    if (source_[position_] == '0' && position_ + 1 < source_.size() && source_[position_ + 1] == 'x') {
        position_ += 2;
        column_ += 2;
        while (position_ < source_.size() && std::isxdigit(source_[position_])) {
            ++position_;
            ++column_;
        }
    } else {
        while (position_ < source_.size() && std::isdigit(source_[position_])) {
            ++position_;
            ++column_;
        }
    }
    std::string value = source_.substr(start, position_ - start);
    return Token{TokenType::IMMEDIATE, value, line_, column_};
}

Token XR32Lexer::lexSymbol() {
    char symbol = source_[position_++];
    ++column_;
    return Token{TokenType::SYMBOL, std::string(1, symbol), line_, column_};
}

Token XR32Lexer::lexDirective() {
    size_t start = position_;
    ++position_; // Skip the initial '.'
    ++column_;
    while (position_ < source_.size() && std::isalnum(source_[position_])) {
        ++position_;
        ++column_;
    }
    std::string value = source_.substr(start, position_ - start);
    if (!isDirective(value)) {
        throwParseError("Unknown directive '{}'", value);
    }
    return Token{TokenType::DIRECTIVE, value, line_, column_};
}

Token XR32Lexer::lexComment() {
    size_t start = position_;
    while (position_ < source_.size() && source_[position_] != '\n') {
        ++position_;
        ++column_;
    }
    std::string value = source_.substr(start, position_ - start);
    return Token{TokenType::COMMENT, value, line_, column_};
}

Token XR32Lexer::lexEndOfLine() {
    ++position_;
    ++line_;
    column_ = 1;
    return Token{TokenType::END_OF_LINE, "\\n", line_ - 1, column_};
}

bool XR32Lexer::isDirective(const std::string& str) const {
    static const std::vector<std::string> directives = { ".org", "db", "dw", "dd" };
    return std::ranges::find(directives, str) != directives.end();
}

} // namespace Assembler
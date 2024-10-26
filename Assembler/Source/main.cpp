#include <iostream>
#include <Lexer.hpp>
#include <Error.hpp>
#include <Register.hpp>

int main() {
    const std::string assemblyCode = R"(
        ; Sample XR-32 Assembly
        .org 0x1000
        start:    mov r1, 42             ; Load immediate value 42 into r1
                  add r2, r1, r3         ; Add r1 and r3, store result in r2
                  jmp end                ; Jump to end label
        end:      hlt                    ; Halt execution
    )";

    Assembler::XR32Lexer lexer(assemblyCode);

    try {
        auto tokenGenerator = lexer.tokenize();
        while (tokenGenerator.move_next()) {
            auto token = tokenGenerator.current();

            std::cout << "Token: ";
            switch (token.type) {
                case Assembler::TokenType::KEYWORD:
                    std::cout << "KEYWORD";
                    break;
                case Assembler::TokenType::IDENTIFIER:
                    std::cout << "IDENTIFIER";
                    break;
                case Assembler::TokenType::REGISTER:
                    std::cout << "REGISTER";
                    break;
                case Assembler::TokenType::IMMEDIATE:
                    std::cout << "IMMEDIATE";
                    break;
                case Assembler::TokenType::SYMBOL:
                    std::cout << "SYMBOL";
                    break;
                case Assembler::TokenType::DIRECTIVE:
                    std::cout << "DIRECTIVE";
                    break;
                case Assembler::TokenType::COMMENT:
                    std::cout << "COMMENT";
                    break;
                case Assembler::TokenType::END_OF_LINE:
                    std::cout << "END_OF_LINE";
                    break;
                case Assembler::TokenType::END_OF_FILE:
                    std::cout << "END_OF_FILE";
                    break;
                default:
                    std::cout << "UNKNOWN";
                    break;
            }

            std::cout << " | Value: " << token.value
                      << " | Line: " << token.line
                      << " | Column: " << token.column << '\n';
        }
    } catch (const Assembler::AssemblerException& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}

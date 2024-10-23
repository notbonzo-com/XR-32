#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <components/cpu.hpp>

/**
 * @brief Assembler class for XR-32 architecture.
 * 
 * This class provides methods to assemble XR-32 assembly code into binary machine code.
 */
class Assembler {
public:
    /**
     * @brief Constructs an Assembler object.
     */
    Assembler() = default;

    /**
     * @brief Assembles a given XR-32 assembly source file into a binary file.
     * @param sourceFile The file containing XR-32 assembly code.
     * @param outputFile The binary file to write the assembled machine code.
     * @return true if assembly was successful, false otherwise.
     */
    bool assemble(const std::string& sourceFile, const std::string& outputFile);

    /**
     * @brief Parses a single line of XR-32 assembly code and converts it into binary.
     * @param line The line of XR-32 assembly code to parse.
     * @return The corresponding 64-bit machine code for the given instruction.
     */
    uint64_t parseAssemblyLine(const std::string& line);

private:
    /**
     * @brief Assembles an R-Type instruction from tokens.
     * @param tokens The tokens representing the instruction.
     * @return The assembled 64-bit instruction.
     */
    uint64_t assembleRType(const std::vector<std::string>& tokens);

    /**
     * @brief Assembles an I-Type instruction from tokens.
     * @param tokens The tokens representing the instruction.
     * @return The assembled 64-bit instruction.
     */
    uint64_t assembleIType(const std::vector<std::string>& tokens);

    /**
     * @brief Assembles a J-Type instruction from tokens.
     * @param tokens The tokens representing the instruction.
     * @return The assembled 64-bit instruction.
     */
    uint64_t assembleJType(const std::vector<std::string>& tokens);

    /**
     * @brief Utility to split a line of assembly code into tokens.
     * @param line The line of code to tokenize.
     * @return A vector of tokens.
     */
    std::vector<std::string> tokenizeLine(const std::string& line);

    /**
     * @brief Utility to parse an immediate value from a string.
     * @param immStr The string to parse.
     * @return The parsed immediate value.
     */
    int32_t parseImmediate(const std::string& immStr);
};

#endif // ASSEMBLER_HPP

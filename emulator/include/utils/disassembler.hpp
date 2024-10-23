#ifndef DISASSEMBLER_HPP
#define DISASSEMBLER_HPP

#include <string>
#include <cstdint>
#include <vector>

/**
 * @brief The Disassembler class converts machine code instructions into human-readable assembly code.
 *
 * This class provides methods for disassembling various types of instructions (R-Type, I-Type, J-Type, etc.)
 * and assembling the output into a readable format. The Disassembler is initialized with a vector of 32-bit
 * instructions and processes them to generate the corresponding assembly code.
 */
class Disassembler {
public:
    /**
     * @brief Constructs a Disassembler with a list of instructions.
     * 
     * @param instructions A vector of 32-bit instructions to be disassembled.
     */
    Disassembler(std::vector<uint32_t> instructions);

    /**
     * @brief Disassembles the provided instructions and returns the assembly code as a string.
     * 
     * This method processes all instructions provided to the Disassembler and generates the corresponding
     * assembly code for each instruction.
     * 
     * @return The complete disassembly as a string.
     */
    [[nodiscard]] std::string disassemble();

    /**
     * @brief Retrieves the current disassembly output.
     * 
     * @return The disassembly output as a string.
     */
    [[nodiscard]] std::string getOutput() const { return output; }

private:
    /**
     * @brief Disassembles an R-Type instruction into its assembly equivalent.
     * 
     * @param instruction The 32-bit R-Type instruction to disassemble.
     * @return The assembly code as a string.
     */
    [[nodiscard]] static std::string disassembleRType(uint32_t instruction);

    /**
     * @brief Disassembles an I-Type instruction into its assembly equivalent.
     * 
     * @param instruction The 32-bit I-Type instruction to disassemble.
     * @return The assembly code as a string.
     */
    [[nodiscard]] static std::string disassembleIType(uint32_t instruction);

    /**
     * @brief Disassembles a J-Type instruction into its assembly equivalent.
     * 
     * @param instruction The 32-bit J-Type instruction to disassemble.
     * @return The assembly code as a string.
     */
    [[nodiscard]] static std::string disassembleJType(uint32_t instruction);

    /**
     * @brief Disassembles a branch instruction into its assembly equivalent.
     * 
     * @param instruction The 32-bit branch instruction to disassemble.
     * @return The assembly code as a string.
     */
    [[nodiscard]] static std::string disassembleBranchType(uint32_t instruction);

    /**
     * @brief Disassembles a load instruction into its assembly equivalent.
     * 
     * @param instruction The 32-bit load instruction to disassemble.
     * @return The assembly code as a string.
     */
    [[nodiscard]] static std::string disassembleLoadType(uint32_t instruction);

    /**
     * @brief Disassembles a store instruction into its assembly equivalent.
     * 
     * @param instruction The 32-bit store instruction to disassemble.
     * @return The assembly code as a string.
     */
    [[nodiscard]] static std::string disassembleStoreType(uint32_t instruction);

    /**
     * @brief Disassembles a compare instruction into its assembly equivalent.
     * 
     * @param instruction The 32-bit compare instruction to disassemble.
     * @return The assembly code as a string.
     */
    [[nodiscard]] static std::string disassembleCmpType(uint32_t instruction);

    /**
     * @brief Disassembles an immediate instruction into its assembly equivalent.
     * 
     * @param instruction The 32-bit immediate instruction to disassemble.
     * @return The assembly code as a string.
     */
    [[nodiscard]] static std::string disassembleImmType(uint32_t instruction);

    std::string output; ///< The complete disassembly output as a string.
    std::vector<uint32_t> instructions; ///< The list of instructions to be disassembled.
    size_t index; ///< The current index in the instructions vector.
};

#endif // DISASSEMBLER_HPP

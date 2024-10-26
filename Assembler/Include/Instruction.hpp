#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>
#include <format>
#include <optional>
#include <Error.hpp>

namespace Assembler {

/**
 * @brief Enum representing the set of supported instructions.
 */
enum class InstructionName {
    ADD, SUB, MUL, DIV, AND, OR, XOR, LSL, LSR, CMP, MOV, ZEXT, MFS, MTS, LDR, STR,
    JMP, JAR, BEQ, BNE, BZ, BNZ, BG, BL, CALL, KCALL, PUSH, POP, KPUSH, KPOP,
    SWI, INC, DEC, RET, KRET, IRET, NOP, HLT, IN, OUT,
    Count // Total number of instructions
};

/**
 * @brief Array mapping instruction names to their opcode values.
 */
constexpr std::array<std::optional<uint8_t>, static_cast<size_t>(InstructionName::Count)> instructionToOpcodeArray = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
    0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14,
    0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E,
    0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28
};

/**
 * @brief Array mapping instruction names to their string representations.
 */
constexpr std::array<std::string_view, static_cast<size_t>(InstructionName::Count)> instructionToStringArray = {
    "ADD", "SUB", "MUL", "DIV", "AND", "OR", "XOR", "LSL", "LSR", "CMP",
    "MOV", "ZEXT", "MFS", "MTS", "LDR", "STR", "JMP", "JAR", "BEQ", "BNE",
    "BZ", "BNZ", "BG", "BL", "CALL", "KCALL", "PUSH", "POP", "KPUSH", "KPOP",
    "SWI", "INC", "DEC", "RET", "KRET", "IRET", "NOP", "HLT", "IN", "OUT"
};

/**
 * @brief Converts an InstructionName to its opcode value.
 *
 * @param name The instruction name to convert.
 * @return The corresponding opcode value.
 */
constexpr std::optional<uint8_t> instructionToOpcode(InstructionName name) {
    return instructionToOpcodeArray[static_cast<size_t>(name)];
}

/**
 * @brief Converts an opcode to an InstructionName.
 *
 * @param opcode The opcode value.
 * @return The corresponding InstructionName, or std::nullopt if not found.
 */
constexpr std::optional<InstructionName> opcodeToInstruction(uint8_t opcode) {
    auto it = std::find(instructionToOpcodeArray.begin(), instructionToOpcodeArray.end(), opcode);
    if (it != instructionToOpcodeArray.end()) {
        return static_cast<InstructionName>(std::distance(instructionToOpcodeArray.begin(), it));
    }
    return std::nullopt;
}

/**
 * @brief Converts an InstructionName to its string representation.
 *
 * @param name The instruction name to convert.
 * @return The string representation of the instruction.
 */
constexpr std::string_view instructionToString(InstructionName name) {
    return instructionToStringArray[static_cast<size_t>(name)];
}

/**
 * @brief Converts a string representation to an InstructionName.
 *
 * @param str The string representation.
 * @return The corresponding InstructionName, or std::nullopt if not found.
 */
constexpr std::optional<InstructionName> stringToInstruction(std::string_view str) {
    auto it = std::find(instructionToStringArray.begin(), instructionToStringArray.end(), str);
    if (it != instructionToStringArray.end()) {
        return static_cast<InstructionName>(std::distance(instructionToStringArray.begin(), it));
    }
    return std::nullopt;
}

/**
 * @brief Enum representing different addressing modes.
 */
enum class AddressingMode {
    Immediate = 0,          // Operand is an immediate value
    Register = 1,           // Operand is a register
    Address = 2,            // Operand is a memory address
    Offset = 3              // Operand is a memory address with offset
};

/**
 * @brief Enum representing the instruction type.
 */
enum class InstructionType {
    RType,                  // Register/Immediate Type
    NType,                  // No Operand Type
    RRType                  // Register/Register/Immediate Type
};

/**
 * @brief Class representing an XR-32 instruction.
 */
class Instruction {
public:
    /**
     * @brief Constructor for an R/I-Type instruction.
     *
     * @param opcode The opcode of the instruction (6 bits).
     * @param addressingMode The addressing mode (2 bits).
     * @param operand The main operand (register or immediate).
     * @param offset Optional register offset (only used for Address mode with offset).
     */
    Instruction(uint8_t opcode, AddressingMode addressingMode, uint32_t operand, std::optional<uint8_t> offset = std::nullopt)
        : opcode(opcode), addressingMode(addressingMode), operand1(operand), offset(offset), type(InstructionType::RType) {}

    /**
     * @brief Constructor for an N/A-Type instruction.
     *
     * @param opcode The opcode of the instruction (6 bits).
     */
    Instruction(uint8_t opcode)
        : opcode(opcode), addressingMode(AddressingMode::Immediate), operand1(0), rd(0), type(InstructionType::NType) {}

    /**
     * @brief Constructor for an RR/I-Type instruction.
     *
     * @param opcode The opcode of the instruction (6 bits).
     * @param addressingMode The addressing mode (2 bits).
     * @param destination The destination register.
     * @param operand The main operand (register or immediate).
     */
    Instruction(uint8_t opcode, AddressingMode addressingMode, uint8_t destination, uint32_t operand)
        : opcode(opcode), addressingMode(addressingMode), operand1(operand), rd(destination), type(InstructionType::RRType) {}

    /**
     * @brief Sets the opcode for the instruction.
     *
     * @param newOpcode The new opcode value.
     */
    void setOpcode(uint8_t newOpcode) {
        opcode = newOpcode;
    }

    /**
     * @brief Gets the opcode of the instruction.
     *
     * @return The opcode value.
     */
    uint8_t getOpcode() const {
        return opcode;
    }

    /**
     * @brief Sets the addressing mode for the instruction.
     *
     * @param mode The new addressing mode.
     */
    void setAddressingMode(AddressingMode mode) {
        addressingMode = mode;
    }

    /**
     * @brief Gets the addressing mode of the instruction.
     *
     * @return The addressing mode.
     */
    AddressingMode getAddressingMode() const {
        return addressingMode;
    }

    /**
     * @brief Sets the first operand (for R/I and RR/I types).
     *
     * @param operand The new operand value.
     */
    void setOperand1(uint32_t operand) {
        operand1 = operand;
    }

    /**
     * @brief Gets the first operand.
     *
     * @return The operand value.
     */
    uint32_t getOperand1() const {
        return operand1;
    }

    /**
     * @brief Sets the destination register (for RR/I type).
     *
     * @param destination The destination register.
     */
    void setDestination(uint8_t destination) {
        rd = destination;
    }

    /**
     * @brief Gets the destination register.
     *
     * @return The destination register.
     */
    uint8_t getDestination() const {
        if (type != InstructionType::RRType) {
            throwLogicalError("Destination register is only available for RR/I-Type instructions.");
        }
        return rd;
    }

    /**
     * @brief Sets the offset register (for R/I type with offset).
     *
     * @param newOffset The new offset value.
     */
    void setOffset(uint8_t newOffset) {
        offset = newOffset;
    }

    /**
     * @brief Gets the offset value, if available.
     *
     * @return The offset value, or std::nullopt if not used.
     */
    std::optional<uint8_t> getOffset() const {
        return offset;
    }

    /**
     * @brief Encodes the instruction into a 64-bit binary representation.
     *
     * @return The encoded instruction as a 64-bit integer.
     */
    uint64_t encode() const {
        uint64_t encoded = 0;
        encoded |= static_cast<uint64_t>(opcode) << 58; // Opcode (6 bits)

        switch (type) {
            case InstructionType::RType:
                encoded |= static_cast<uint64_t>(addressingMode) << 56; // Addressing Mode (2 bits)
                encoded |= static_cast<uint64_t>(operand1) << 24;       // Operand (32 bits)
                if (offset.has_value()) {
                    encoded |= static_cast<uint64_t>(offset.value()) << 15; // Offset (8 bits)
                }
                break;

            case InstructionType::NType:
                // No additional encoding needed for N/A-Type instructions.
                break;

            case InstructionType::RRType:
                encoded |= static_cast<uint64_t>(addressingMode) << 56; // Addressing Mode (2 bits)
                encoded |= static_cast<uint64_t>(rd) << 48;             // Destination Register (8 bits)
                encoded |= static_cast<uint64_t>(operand1) << 15;       // Operand (32 bits)
                break;
        }

        return encoded;
    }

private:
    uint8_t opcode;                                 // 6-bit opcode
    AddressingMode addressingMode;                  // 2-bit addressing mode
    uint32_t operand1;                              // Main operand
    uint8_t rd;                                     // Destination register (for RR/I-Type)
    std::optional<uint8_t> offset;                  // Optional offset (for R/I-Type)
    InstructionType type;                           // Instruction type (R/I, N/A, RR/I)
};

} // namespace Assembler

#endif // INSTRUCTION_HPP

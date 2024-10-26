#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>
#include <format>
#include <optional>
#include "Error.hpp"

namespace Assembler {

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

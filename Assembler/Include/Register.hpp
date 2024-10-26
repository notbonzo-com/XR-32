#ifndef REGISTER_HPP
#define REGISTER_HPP

#include <cstdint>
#include <string_view>
#include <optional>
#include <array>
#include <algorithm>

/**
 * @brief Enum representing all general-purpose, special-purpose, and other registers.
 */
enum class Register {
    // General-purpose registers
    R0, R1, R2, R3, R4, R5, R6, R7,
    R8, R9, R10, R11, R12, R13, R14, R15,
    R16, R17, R18, R19, R20, R21, R22, R23,
    R24, R25, R26, R27, R28, R29, R30, R31,

    // Special-purpose registers
    I0,    // Instruction Pointer
    S0,    // Stack Pointer 0
    S1,    // Stack Pointer 1 (Kernel stack)
    FR,    // Flags Register
    IVTR,  // Interrupt Vector Table Register
    IE0,   // Interrupt Error Code
    IE1,   // Instruction Pointer of Interrupted Instruction
    IE2,   // Stack Pointer of Current Task
    IE3,   // Flags Register of Interrupted Instruction
    IE4,   // Mode Status Register of Interrupted State
    TPDR,  // Top Page Directory Pointer
    TSP,   // Task State Pointer
    PRR,   // Processor Revision Register
    MSR,   // Mode Status Register
    Count  // Total number of registers
};

/**
 * @brief Array for converting registers to hexadecimal values.
 */
constexpr std::array<std::optional<uint8_t>, static_cast<size_t>(Register::Count)> registerToHexArray = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2A, 0x2B, 0x2C
};

/**
 * @brief Array for converting registers to string representations.
 */
constexpr std::array<std::string_view, static_cast<size_t>(Register::Count)> registerToStringArray = {
    "R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7",
    "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15",
    "R16", "R17", "R18", "R19", "R20", "R21", "R22", "R23",
    "R24", "R25", "R26", "R27", "R28", "R29", "R30", "R31",
    "I0", "S0", "S1", "FR", "IVTR", "IE0", "IE1", "IE2",
    "IE3", "IE4", "TPDR", "TSP", "PRR", "MSR"
};

/**
 * @brief Converts a register to its hexadecimal representation.
 *
 * @param reg The register to convert.
 * @return The hexadecimal value associated with the register.
 */
constexpr std::optional<uint8_t> registerToHex(Register reg) {
    return registerToHexArray[static_cast<size_t>(reg)];
}

/**
 * @brief Converts a hexadecimal representation to a register.
 *
 * @param hex The hexadecimal value.
 * @return The corresponding register, or std::nullopt if not found.
 */
constexpr std::optional<Register> hexToRegister(uint8_t hex) {
    auto it = std::find(registerToHexArray.begin(), registerToHexArray.end(), hex);
    if (it != registerToHexArray.end()) {
        return static_cast<Register>(std::distance(registerToHexArray.begin(), it));
    }
    return std::nullopt;
}

/**
 * @brief Converts a register to its string representation.
 *
 * @param reg The register to convert.
 * @return The string representation of the register.
 */
constexpr std::string_view registerToString(Register reg) {
    return registerToStringArray[static_cast<size_t>(reg)];
}

/**
 * @brief Converts a string representation to a register.
 *
 * @param str The string representation.
 * @return The corresponding register, or std::nullopt if not found.
 */
constexpr std::optional<Register> stringToRegister(std::string_view str) {
    auto it = std::find(registerToStringArray.begin(), registerToStringArray.end(), str);
    if (it != registerToStringArray.end()) {
        return static_cast<Register>(std::distance(registerToStringArray.begin(), it));
    }
    return std::nullopt;
}

#endif // REGISTER_HPP
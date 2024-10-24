#ifndef CPU_HPP
#define CPU_HPP

#include <array>
#include <cstdint>
#include <concepts>
#include <variant>
#include <string_view>
#include <unordered_map>
#include <components/memory.hpp>
#include <components/io.hpp>
#include <components/interrupts.hpp>
#include <components/isa.hpp>

constexpr std::array<std::pair<uint8_t, std::string_view>, 45> Hex2Register {{
    {0x0, "R0"}, {0x1, "R1"}, {0x2, "R2"}, {0x3, "R3"},
    {0x4, "R4"}, {0x5, "R5"}, {0x6, "R6"}, {0x7, "R7"},
    {0x8, "R8"}, {0x9, "R9"}, {0xA, "R10"}, {0xB, "R11"},
    {0xC, "R12"}, {0xD, "R13"}, {0xE, "R14"}, {0xF, "R15"},
    {0x10, "R16"}, {0x11, "R17"}, {0x12, "R18"}, {0x13, "R19"},
    {0x14, "R20"}, {0x15, "R21"}, {0x16, "R22"}, {0x17, "R23"},
    {0x18, "R24"}, {0x19, "R25"}, {0x1A, "R26"}, {0x1B, "R27"},
    {0x1C, "R28"}, {0x1D, "R29"}, {0x1E, "R30"}, {0x1F, "R31"},
    {0x20, "I0"}, {0x21, "S0"}, {0x22, "S1"}, {0x23, "FR"},
    {0x24, "IVTR"}, {0x25, "IE0"}, {0x26, "IE1"}, {0x27, "IE2"},
    {0x28, "IE3"}, {0x29, "TPDR"}, {0x2A, "TSP"}, {0x2B, "PRR"},
    {0x2C, "MSR"}
}};

constexpr std::array<std::pair<std::string_view, uint64_t>, 38> Instruction2Hex {{
    {"ADD", 0x01}, {"SUB", 0x02}, {"AND", 0x03}, {"OR", 0x04}, {"XOR", 0x05},
    {"LSL", 0x06}, {"LSR", 0x07}, {"LDR", 0x08}, {"STR", 0x09}, {"JMP", 0x0A},
    {"JAL", 0x0B}, {"BEQ", 0x0C}, {"BNE", 0x0D}, {"MOV", 0x0E}, {"CMP", 0x0F},
    {"PUSH", 0x10}, {"POP", 0x11}, {"CALL", 0x12}, {"RET", 0x13}, {"IRET", 0x14},
    {"NOP", 0x15}, {"HLT", 0x16}, {"MUL", 0x17}, {"DIV", 0x18}, {"MOD", 0x19},
    {"NOT", 0x1A}, {"NEG", 0x1B}, {"INC", 0x1C}, {"DEC", 0x1D}, {"ASL", 0x1E},
    {"ASR", 0x1F}, {"SWI", 0x20}, {"SEXT", 0x21}, {"ZEXT", 0x22},
    {"MFS", 0x23}, {"MTS", 0x24}, {"OUT", 0x25}, {"IN", 0x26}
}};

/**
 * @brief CPU class simulates the XR-32 architecture CPU.
 *
 * Manages the execution of instructions, interacting with memory, IO, and handling interrupts.
 * Provides methods to execute instructions, reset the CPU state, and manage registers.
 */
class CPU {
public:
    /**
     * @brief Constructs an CPU object with the specified components.
     */
    CPU(size_t memorySize) noexcept;

    void executeNextInstruction();
    void reset() noexcept;

    /**
     * @brief Structure to represent CPU registers.
     */
    struct Registers {
        std::array<uint32_t, 32> R{}; ///< General-purpose registers R0-R31
        uint32_t I0{0};               ///< Instruction Pointer
        uint32_t S0{0};               ///< Stack Pointer 0
        uint32_t S1{0};               ///< Stack Pointer 1 (Kernel)
        uint8_t FR{0};                ///< Flags Register
        uint32_t IVTR{0};             ///< Interrupt Vector Table Register
        uint8_t IE0{0};               ///< Interrupt type register
        uint32_t IE1{0};              ///< Interrupt type register
        uint32_t IE2{0};              ///< Interrupt type register
        uint8_t IE3{0};               ///< Interrupt type register
        uint32_t IE4{0};              ///< Interrupt type register
        uint32_t TPDR{0};             ///< Top Page Directory Register
        uint32_t TSP{0};              ///< Task State Pointer
        uint8_t PRR{0};               ///< Processor Revision Register (read-only)
        uint32_t MSR{0};              ///< Mode Status Register
    } registers;

    Memory memory;                    ///< memory component
    IO io;                            ///< IO component
    Interrupts interrupts;            ///< interrupt handler
    InstructionSet isa;               ///< ISA component

private:
    [[nodiscard]] uint64_t fetchInstruction() const; ///< Fetches the instruction code at the current I0
    
public:
    constexpr static std::string_view findRegister(uint8_t hex) {
        for (const auto& [key, value] : Hex2Register) {
            if (key == hex) {
                return value;
            }
        }
        return "Unknown";
    }
    constexpr static uint8_t findRegister(std::string_view register_name) {
        for (const auto& [key, value] : Hex2Register) {
            if (value == register_name) {
                return key;
            }
        }
        return 0xFF;
    }
    constexpr static uint64_t findInstruction(std::string_view instruction) {
        for (const auto& [key, value] : Instruction2Hex) {
            if (key == instruction) {
                return value;
            }
        }
        return 0xFFFFFFFF;
    }
};

#endif // CPU_HPP

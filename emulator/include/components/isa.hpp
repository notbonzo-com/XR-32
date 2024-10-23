#ifndef ISA_HPP
#define ISA_HPP

#include <components/memory.hpp>
#include <components/io.hpp>
#include <cstdint>
#include <variant>

class CPU; // Forward declaration

/**
 * @brief Struct representing a decoded R-Type instruction.
 */
struct RTypeInstruction {
    uint64_t reserved : 29;  ///< Reserved space (29 bits)
    uint64_t func     : 8;   ///< Function code for extended operations (8 bits)
    uint64_t shamt    : 6;   ///< Shift amount (6 bits)
    uint64_t rs2      : 5;   ///< Second source register (5 bits)
    uint64_t rs1      : 5;   ///< First source register (5 bits)
    uint64_t rd       : 5;   ///< Destination register (5 bits)
    uint64_t opcode   : 6;   ///< Operation code (6 bits)
};

/**
 * @brief Struct representing a decoded I-Type instruction.
 */
struct ITypeInstruction {
    uint64_t reserved  : 16;   ///< Reserved space (16 bits)
    uint64_t immediate : 32;   ///< Immediate value (32 bits)
    uint64_t rs1       : 5;    ///< Source register (5 bits)
    uint64_t rd        : 5;    ///< Destination register (5 bits)
    uint64_t opcode    : 6;    ///< Operation code (6 bits)
};

/**
 * @brief Struct representing a decoded J-Type instruction.
 */
struct JTypeInstruction {
    uint64_t reserved : 26;    ///< Reserved space (26 bits)
    uint64_t address  : 32;    ///< Jump target address (32 bits)
    uint64_t opcode   : 6;     ///< Operation code (6 bits)
};

/**
 * @brief InstructionSet class for XR-32 architecture.
 * 
 * Handles the decoding and execution of instructions.
 */
class InstructionSet {
public:
    /**
     * @brief Constructs an InstructionSet object with a reference to the CPU.
     */
    explicit InstructionSet(CPU& cpuRef) : cpu(cpuRef) {}

    /**
     * @brief Decodes a fetched instruction into its appropriate type.
     * @param instruction The raw 32-bit instruction code.
     * @return Decoded instruction as one of the defined instruction structs.
     */
    static std::variant<RTypeInstruction, ITypeInstruction, JTypeInstruction> decodeInstruction(uint64_t instruction);

    /**
     * @brief Executes a decoded instruction.
     * @param instruction The decoded instruction to execute.
     */
    void execute(const std::variant<RTypeInstruction, ITypeInstruction, JTypeInstruction>& instruction);

private:
    CPU& cpu;  ///< Reference to the CPU object to interact with the CPU state, memory, and interrupts.

    // Helper functions for instruction execution
    void executeRType(const RTypeInstruction& instr);
    void executeIType(const ITypeInstruction& instr);
    void executeJType(const JTypeInstruction& instr);

    /**
     * @brief Sets the flags in the CPU based on the result of an operation.
     * @param result The result of the operation.
     * @param carry Whether the operation caused a carry.
     * @param overflow Whether the operation caused an overflow.
     */
    void setFlags(uint32_t result, bool carry, bool overflow);

    friend class CPU;
};

#endif // ISA_HPP

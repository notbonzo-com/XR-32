#include <components/isa.hpp>
#include <components/cpu.hpp>
#include <components/memory.hpp>
#include <components/io.hpp>
#include <stdexcept>
#include <variant>
#include <cstring>

std::variant<RTypeInstruction, ITypeInstruction, JTypeInstruction>
InstructionSet::decodeInstruction(uint64_t instruction) {
    uint64_t opcode = instruction >> 58; // Extract the opcode (6 bits: 63–58)

    if (opcode == 0x01 || opcode == 0x02 || opcode == 0x03 || opcode == 0x04 || opcode == 0x05 ||
        opcode == 0x06 || opcode == 0x07 || opcode == 0x17 || opcode == 0x18 || opcode == 0x19) {
        // R-Type instruction
        RTypeInstruction rInstr{
            .reserved = 0,
            .func = static_cast<uint8_t>((instruction >> 29) & 0xFF),    // func: bits 36–29
            .shamt = static_cast<uint8_t>((instruction >> 37) & 0x3F),   // shamt: bits 42–37
            .rs2 = static_cast<uint8_t>((instruction >> 43) & 0x1F),     // rs2: bits 47–43
            .rs1 = static_cast<uint8_t>((instruction >> 48) & 0x1F),     // rs1: bits 52–48
            .rd = static_cast<uint8_t>((instruction >> 53) & 0x1F),      // rd: bits 57–53
            .opcode = static_cast<uint8_t>(opcode),
        };
        return rInstr;
    } else if (opcode == 0x08 || opcode == 0x09 || opcode == 0x0C || opcode == 0x0D || opcode == 0x0E || 
               opcode == 0x0F || opcode == 0x10 || opcode == 0x11 || opcode == 0x20 || opcode == 0x21 || 
               opcode == 0x22 || opcode == 0x23 || opcode == 0x24) {
        // I-Type instruction
        ITypeInstruction iInstr{
            .reserved = 0,
            .immediate = static_cast<uint64_t>((instruction >> 16) & 0xFFFFFFFF), // immediate: bits 47–16
            .rs1 = static_cast<uint8_t>((instruction >> 48) & 0x1F),     // rs1: bits 52–48
            .rd = static_cast<uint8_t>((instruction >> 53) & 0x1F),      // rd: bits 57–53
            .opcode = static_cast<uint8_t>(opcode),
        };
        return iInstr;
    } else if (opcode == 0x0A || opcode == 0x0B || opcode == 0x12 || opcode == 0x13 || opcode == 0x14 || 
               opcode == 0x15 || opcode == 0x16) {
        // J-Type instruction
        JTypeInstruction jInstr{
            .reserved = 0,
            .address = static_cast<uint32_t>((instruction >> 26) & 0xFFFFFFFF), // address: bits 57–26
            .opcode = static_cast<uint8_t>(opcode),
        };
        return jInstr;
    } else {
        // Invalid instruction
        throw std::runtime_error("Invalid instruction opcode");
    }
}

void InstructionSet::execute(const std::variant<RTypeInstruction, ITypeInstruction, JTypeInstruction>& instruction) {
    std::visit([this](auto&& instr) {
        using T = std::decay_t<decltype(instr)>;
        if constexpr (std::is_same_v<T, RTypeInstruction>) {
            this->executeRType(instr);
        } else if constexpr (std::is_same_v<T, ITypeInstruction>) {
            this->executeIType(instr);
        } else if constexpr (std::is_same_v<T, JTypeInstruction>) {
            this->executeJType(instr);
        }
    }, instruction);
}

void InstructionSet::executeRType(const RTypeInstruction& instr) {
    switch (instr.opcode) {
        case 0x01: // ADD
            cpu.registers.R[instr.rd] = cpu.registers.R[instr.rs1] + cpu.registers.R[instr.rs2];
            setFlags(cpu.registers.R[instr.rd], false, false);
            break;
        case 0x02: // SUB
            cpu.registers.R[instr.rd] = cpu.registers.R[instr.rs1] - cpu.registers.R[instr.rs2];
            setFlags(cpu.registers.R[instr.rd], false, false);
            break;
        case 0x03: // AND
            cpu.registers.R[instr.rd] = cpu.registers.R[instr.rs1] & cpu.registers.R[instr.rs2];
            setFlags(cpu.registers.R[instr.rd], false, false);
            break;
        case 0x04: // OR
            cpu.registers.R[instr.rd] = cpu.registers.R[instr.rs1] | cpu.registers.R[instr.rs2];
            setFlags(cpu.registers.R[instr.rd], false, false);
            break;
        case 0x05: // XOR
            cpu.registers.R[instr.rd] = cpu.registers.R[instr.rs1] ^ cpu.registers.R[instr.rs2];
            setFlags(cpu.registers.R[instr.rd], false, false);
            break;
        case 0x06: // LSL
            cpu.registers.R[instr.rd] = cpu.registers.R[instr.rs1] << instr.shamt;
            setFlags(cpu.registers.R[instr.rd], false, false);
            break;
        case 0x07: // LSR
            cpu.registers.R[instr.rd] = cpu.registers.R[instr.rs1] >> instr.shamt;
            setFlags(cpu.registers.R[instr.rd], false, false);
            break;
        case 0x17: // MUL
            cpu.registers.R[instr.rd] = cpu.registers.R[instr.rs1] * cpu.registers.R[instr.rs2];
            setFlags(cpu.registers.R[instr.rd], false, false);
            break;
        case 0x18: // DIV
            cpu.registers.R[instr.rd] = cpu.registers.R[instr.rs1] / cpu.registers.R[instr.rs2];
            setFlags(cpu.registers.R[instr.rd], false, false);
            break;
        case 0x19: // MOD
            cpu.registers.R[instr.rd] = cpu.registers.R[instr.rs1] % cpu.registers.R[instr.rs2];
            setFlags(cpu.registers.R[instr.rd], false, false);
            break;
        case 0x1A: // NOT
            cpu.registers.R[instr.rd] = ~cpu.registers.R[instr.rs1];
            setFlags(cpu.registers.R[instr.rd], false, false);
            break;
        case 0x1B: // NEG
            cpu.registers.R[instr.rd] = -cpu.registers.R[instr.rs1];
            setFlags(cpu.registers.R[instr.rd], false, false);
            break;
        case 0x1C: // INC
            cpu.registers.R[instr.rd] += 1;
            setFlags(cpu.registers.R[instr.rd], false, false);
            break;
        case 0x1D: // DEC
            cpu.registers.R[instr.rd] -= 1;
            setFlags(cpu.registers.R[instr.rd], false, false);
            break;
        case 0x1E: // ASL (Arithmetic shift left)
            cpu.registers.R[instr.rd] = cpu.registers.R[instr.rs1] << instr.shamt;
            setFlags(cpu.registers.R[instr.rd], false, false);
            break;
        case 0x1F: // ASR (Arithmetic shift right)
            cpu.registers.R[instr.rd] = cpu.registers.R[instr.rs1] >> instr.shamt;
            setFlags(cpu.registers.R[instr.rd], false, false);
            break;

        default:
            cpu.interrupts.triggerInterrupt(0x1);
            break;
    }
}

void InstructionSet::executeIType(const ITypeInstruction& instr) {
    switch (instr.opcode) {
        case 0x08: // LDR
            if (instr.rs1 == 0x2D) {
                cpu.registers.R[instr.rd] = cpu.memory.read(instr.immediate);
            } else {
                cpu.registers.R[instr.rd] = cpu.memory.read(cpu.registers.R[instr.rs1] + instr.immediate);
            }
            break;
        case 0x09: // STR
            if (instr.rs1 == 0x2D) {
                cpu.memory.write(instr.immediate, cpu.registers.R[instr.rd]);
            } else {
                cpu.memory.write(cpu.registers.R[instr.rs1] + instr.immediate, cpu.registers.R[instr.rd]);
            }
            break;
        case 0x0C: // BEQ
            if (cpu.registers.R[instr.rs1] == cpu.registers.R[instr.rd]) {
                cpu.registers.I0 += instr.immediate;
            }
            break;
        case 0x0D: // BNE
            if (cpu.registers.R[instr.rs1] != cpu.registers.R[instr.rd]) {
                cpu.registers.I0 += instr.immediate;
            }
            break;
        case 0x0E: // MOV
            cpu.registers.R[instr.rd] = cpu.registers.R[instr.rs1];
            break;
        case 0x0F: // CMP
            setFlags(cpu.registers.R[instr.rs1] - cpu.registers.R[instr.rd], false, false);
            break;
        case 0x10: // PUSH
            cpu.registers.S0 -= 4;
            cpu.memory.write(cpu.registers.S0, cpu.registers.R[instr.rd]);
            break;
        case 0x11: // POP
            cpu.registers.R[instr.rd] = cpu.memory.read(cpu.registers.S0);
            cpu.registers.S0 += 4;
            break;
        case 0x20: // SWI
            cpu.interrupts.triggerInterrupt(instr.immediate & 0xFF);
            break;
        case 0x21: // SEXT
            cpu.registers.R[instr.rd] = static_cast<int32_t>(cpu.registers.R[instr.rs1]); // Sign extend to 32-bit
            break;
        case 0x22: // ZEXT
            cpu.registers.R[instr.rd] = static_cast<uint32_t>(cpu.registers.R[instr.rs1]); // Zero extend to 32-bit
            break;
        case 0x23: // MFS
            if (instr.rs1 < cpu.registers.R.size()) {
                cpu.registers.R[instr.rd] = cpu.registers.R[instr.rs1];
            }
            break;
        case 0x24: // MTS
            if (instr.rd < cpu.registers.R.size()) {
                cpu.registers.R[instr.rs1] = cpu.registers.R[instr.rd];
            }
            break;
        case 0x25: // OUT
            cpu.io.writePort(instr.rd, cpu.registers.R[instr.rs1]);
            break;
        case 0x26: // IN
            cpu.registers.R[instr.rs1] = cpu.io.readPort(instr.rd);
            break;

        default:
            cpu.interrupts.triggerInterrupt(0x1);
            break;
    }
}

void InstructionSet::executeJType(const JTypeInstruction& instr) {
    switch (instr.opcode) {
        case 0x0A: // JMP
            cpu.registers.I0 = instr.address;
            break;
        case 0x0B: // JAL
            cpu.registers.R[31] = cpu.registers.I0; // Store return address in R31
            cpu.registers.I0 = instr.address;
            break;
        case 0x12: // CALL
            cpu.registers.S0 -= 4;
            cpu.memory.write(cpu.registers.S0, cpu.registers.I0); // Push return address onto the stack
            cpu.registers.I0 = instr.address;
            break;
        case 0x13: // RET
            cpu.registers.I0 = cpu.memory.read(cpu.registers.S0); // Pop return address from the stack
            cpu.registers.S0 += 4;
            break;
        case 0x14: // IRET
            cpu.interrupts.triggerIret();
            break;
        case 0x15: // NOP
            // No operation
            break;
        case 0x16: // HLT
            //TODO Halt? Right now we only got one core so maybe wait for interrupt?
            throw std::runtime_error("CPU Halted");
            break;
        default:
            cpu.interrupts.triggerInterrupt(0x1); // invalid opcode
            break;
    }
}

void InstructionSet::setFlags(uint32_t result, bool carry, bool overflow) {
    if (result == 0) cpu.registers.FR |= (1 << 1); // Zero flag
    if (result & 0x80000000) cpu.registers.FR |= (1 << 2); // Sign flag
    if (carry) cpu.registers.FR |= (1 << 0); // Carry flag
    if (overflow) cpu.registers.FR |= (1 << 6); // Overflow flag
}
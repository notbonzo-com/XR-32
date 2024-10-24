#include <components/cpu.hpp>
#include <stdexcept>

CPU::CPU(size_t memorySize) noexcept
    : registers(Registers{}), memory(memorySize, *this), io(*this), interrupts(*this), isa(*this)  {
    reset();
}

void CPU::reset() noexcept {
    registers = Registers{};
    registers.MSR = 0x1;
}

void CPU::executeNextInstruction() {
    uint64_t instruction = fetchInstruction();
    auto decoded = isa.decodeInstruction(instruction);
    isa.execute(decoded);
}

uint64_t CPU::fetchInstruction() const {
    uint64_t instruction = memory.read(registers.I0);
    return instruction;
}


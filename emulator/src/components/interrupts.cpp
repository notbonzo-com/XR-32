#include <components/memory.hpp>
#include <components/interrupts.hpp>
#include <components/cpu.hpp>
#include <cstdint>
#include <stdexcept>

void Interrupts::triggerInterrupt(uint8_t interruptNumber, uint8_t errorCode) {
    saveContext();
    cpu.registers.IE0 = errorCode;
    uint32_t isrAddress = fetchISRAddress(interruptNumber);
    cpu.registers.I0 = isrAddress;

}

void Interrupts::triggerIret() {
    restoreContext();
}

void Interrupts::saveContext() {
    cpu.registers.IE1 = cpu.registers.I0;   // Save the current instruction pointer (I0)
    cpu.registers.IE2 = cpu.registers.S0;   // Save the stack pointer (S0)
    cpu.registers.IE3 = cpu.registers.FR;   // Save the flags register (FR)
    cpu.registers.IE4 = cpu.registers.MSR;  // Save the mode/status register (MSR)

    cpu.registers.MSR |= 0x80000000;  // Set the highest bit for kernel mode
}

void Interrupts::restoreContext() {
    cpu.registers.I0 = cpu.registers.IE1;   // Restore the instruction pointer (I0)
    cpu.registers.S0 = cpu.registers.IE2;   // Restore the stack pointer (S0)
    cpu.registers.FR = cpu.registers.IE3;   // Restore the flags register (FR)
    cpu.registers.MSR = cpu.registers.IE4;  // Restore the mode/status register (MSR)
}

uint32_t Interrupts::fetchISRAddress(uint8_t interruptNumber) const {
    uint32_t ivtBase = cpu.registers.IVTR;  // The IVT base address
    uint32_t isrAddress = cpu.memory.readRaw(ivtBase + (interruptNumber * 4));  // Each IVT entry is 32 bits
    return isrAddress;
}


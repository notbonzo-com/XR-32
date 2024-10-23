#include <components/cpu.hpp>
#include <components/memory.hpp>
#include <stdexcept>
#include <cstring>

Memory::Memory(size_t size, CPU& cpuarg) : memory(size), cpu(cpuarg) {
    this->reset();
}

uint32_t Memory::readRaw(uint32_t address) const {
    if (address >= memory.size()) {
        throw std::out_of_range("Address out of bounds"); // Actuall emulator error here
    }
    uint32_t value;
    std::memcpy(&value, &memory[address], sizeof(uint32_t));
    return value;
}

void Memory::writeRaw(uint32_t address, uint32_t value) {
    if (address >= memory.size()) {
        throw std::out_of_range("Address out of bounds"); // not an exception, emulator error
    }
    std::memcpy(&memory[address], &value, sizeof(uint32_t));
}

uint32_t Memory::read(uint32_t virtualAddress) const {
    uint32_t physicalAddress = translate(virtualAddress);
    if (!checkAccessRights(physicalAddress, AccessType::Read)) {
        cpu.interrupts.triggerInterrupt(GeneralProtectionFault, 0x01); // GPF: Unauthorized read
        return 0;
    }
    return readRaw(physicalAddress);
}

void Memory::write(uint32_t virtualAddress, uint32_t value) {
    uint32_t physicalAddress = translate(virtualAddress);
    if (!checkAccessRights(physicalAddress, AccessType::Write)) {
        cpu.interrupts.triggerInterrupt(GeneralProtectionFault, 0x02); // GPF: Unauthorized write
        return;
    }
    writeRaw(physicalAddress, value);
}


bool Memory::checkAccessRights(uint32_t physicalAddress, AccessType accessType) const {
    if (accessType == AccessType::Read) {
        return true;
    }
    
    uint32_t msr = cpu.registers.MSR;
    bool kernelMode = (msr & 0x80000000) != 0;

    uint32_t pageDirectoryBase = cpu.registers.TPDR;
    uint32_t pageDirectoryIndex = (physicalAddress >> 22) & 0x3FF;
    uint32_t pageTableIndex = (physicalAddress >> 12) & 0x3FF;
    uint32_t pageDirectoryEntry = readRaw(pageDirectoryBase + pageDirectoryIndex * sizeof(uint32_t));

    bool pdeKernelOnly = (pageDirectoryEntry & (1 << 10)) != 0;
    if (pdeKernelOnly && !kernelMode) {
        return false;
    }

    uint32_t pageTableBase = pageDirectoryEntry & ~0xFFF;
    uint32_t pageTableEntry = readRaw(pageTableBase + pageTableIndex * sizeof(uint32_t));

    bool pteKernelOnly = (pageTableEntry & (1 << 10)) != 0;
    if (pteKernelOnly && !kernelMode) {
        return false;
    }

    return true;
}

uint32_t Memory::translateVirtualAddress(uint32_t virtualAddress) const {
    return translate(virtualAddress);
}

uint32_t Memory::translate(uint32_t virtualAddress) const {
    uint32_t pageDirectoryIndex = (virtualAddress >> 22) & 0x3FF;
    uint32_t pageTableIndex = (virtualAddress >> 12) & 0x3FF;
    uint32_t pageOffset = virtualAddress & 0xFFF;

    uint32_t pageDirectoryBase = cpu.registers.TPDR;
    uint32_t pageTableBase = read(pageDirectoryBase + pageDirectoryIndex * sizeof(uint32_t));

    if (!(pageTableBase & 0x1)) {
        cpu.interrupts.triggerInterrupt(InterruptType::PageFault, 0x00);
        return 0xFFFFFFFF;
    }

    uint32_t pageTableAddress = pageTableBase & ~0xFFF;
    uint32_t pageEntry = read(pageTableAddress + pageTableIndex * sizeof(uint32_t));

    if (!(pageEntry & 0x1)) {
        cpu.interrupts.triggerInterrupt(InterruptType::PageFault, 0x00);
        return 0xFFFFFFFF;
    }

    uint32_t physicalPageBase = pageEntry & ~0xFFF;
    uint32_t physicalAddress = physicalPageBase + pageOffset;

    return physicalAddress;
}

void Memory::reset() noexcept {
    std::memset(memory.data(), 0, memory.size());
}

#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstdint>
#include <vector>
#include <array>

class CPU; // Forward declaration
class Interrupts; // Forward declaration

/**
 * @brief Enum to define different types of memory access.
 */
enum class AccessType {
    Read,  ///< Represents a read operation.
    Write  ///< Represents a write operation.
};

/**
 * @brief Memory class simulates the system's memory, including support for paging.
 *
 * This class manages reading from and writing to memory, as well as translating virtual addresses
 * to physical addresses using a simplified paging mechanism.
 */
class Memory {
public:
    /**
     * @brief Constructs a Memory object with the specified size.
     * 
     * @param size The size of the memory in bytes.
     */
    explicit Memory(size_t size, CPU& cpuarg);

    /**
     * @brief Reads a 32-bit value from the specified address.
     * 
     * @param address The virtual memory address to read from.
     * @return The 32-bit value stored at the specified address.
     * @throws std::out_of_range if the address is outside the memory bounds or access is denied.
     */
    [[nodiscard]] uint32_t read(uint32_t address) const;

    /**
     * @brief Writes a 32-bit value to the specified address.
     * 
     * @param address The virtual memory address to write to.
     * @param value The 32-bit value to store at the specified address.
     * @throws std::out_of_range if the address is outside the memory bounds or access is denied.
     */
    void write(uint32_t address, uint32_t value);

    /**
     * @brief Reads a 32-bit value directly from the physical address without any privilege checks.
     * 
     * @param physicalAddress The physical memory address to read from.
     * @return The 32-bit value stored at the specified physical address.
     * @throws std::out_of_range if the address is outside the memory bounds.
     */
    [[nodiscard]] uint32_t readRaw(uint32_t physicalAddress) const;

    /**
     * @brief Writes a 32-bit value directly to the physical address without any privilege checks.
     * 
     * @param physicalAddress The physical memory address to write to.
     * @param value The 32-bit value to store at the specified physical address.
     * @throws std::out_of_range if the address is outside the memory bounds.
     */
    void writeRaw(uint32_t physicalAddress, uint32_t value);

    /**
     * @brief Translates a virtual address to a physical address using the page table.
     * 
     * @param virtualAddress The virtual address to translate.
     * @return The translated physical address.
     * @throws std::out_of_range if the virtual address is invalid or if translation fails.
     */
    [[nodiscard]] uint32_t translateVirtualAddress(uint32_t virtualAddress) const;

    /**
     * @brief Resets the memory by clearing its contents and resetting the page table.
     */
    void reset() noexcept;

private:
    std::vector<uint8_t> memory; ///< The main memory storage as a byte-addressable vector.
    CPU& cpu; ///< A reference to the CPU object for the TPDR register.

    /**
     * @brief Internal helper function to translate a virtual address to a physical address.
     * 
     * This function is used by `translateVirtualAddress` to perform the actual translation.
     * 
     * @param address The virtual address to translate.
     * @return The corresponding physical address.
     * @throws Triggers an internal interrupt if page not present.
     */
    [[nodiscard]] uint32_t translate(uint32_t address) const;

    /**
     * @brief Checks if the current CPU mode allows the specified type of access to the physical address.
     * 
     * @param physicalAddress The physical address to check.
     * @param accessType The type of access (read or write).
     * @return `true` if access is allowed, `false` otherwise.
     */
    [[nodiscard]] bool checkAccessRights(uint32_t physicalAddress, AccessType accessType) const;

    friend class CPU;
    friend class Interrupts;
};

#endif // MEMORY_HPP

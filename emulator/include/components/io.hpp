#ifndef IO_HPP
#define IO_HPP

#include <functional>
#include <unordered_map>
#include <cstdint>

class CPU;  // Forward declaration

/**
 * @brief The IO class manages input/output operations for the emulator.
 *
 * This class handles reading from and writing to I/O ports, allowing devices to be mapped to specific
 * ports for interaction with the CPU. It supports mapping custom read and write functions to different ports.
 */
class IO {
public:
    /**
     * @brief Constructs an IO object with a reference to the CPU.
     * 
     * @param cpuRef Reference to the CPU object that interacts with the IO.
     */
    explicit IO(CPU& cpuRef) noexcept : cpu(cpuRef) {}

    /**
     * @brief Reads a 32-bit value from the specified I/O port.
     * 
     * This method invokes the read function mapped to the specified port and returns the result.
     * If no device is mapped to the port, it returns 0 or a default value.
     * 
     * @param port The I/O port to read from.
     * @return The 32-bit value read from the port.
     */
    [[nodiscard]] uint32_t readPort(uint16_t port) const noexcept;

    /**
     * @brief Writes a 32-bit value to the specified I/O port.
     * 
     * This method invokes the write function mapped to the specified port, passing the value to be written.
     * If no device is mapped to the port, the write operation has no effect.
     * 
     * @param port The I/O port to write to.
     * @param value The 32-bit value to write to the port.
     */
    void writePort(uint16_t port, uint32_t value) noexcept;

    /**
     * @brief Maps a device to a specific I/O port.
     * 
     * This method allows a device to be mapped to a specific port by providing custom read and write functions.
     * When the CPU performs I/O operations on the port, these functions are invoked.
     * 
     * @param port The I/O port to map the device to.
     * @param readFunc The function to be called when reading from the port.
     * @param writeFunc The function to be called when writing to the port.
     * @throws std::invalid_argument if the port is already mapped.
     */
    void mapDevice(uint16_t port, std::function<uint32_t()> readFunc, std::function<void(uint32_t)> writeFunc) noexcept;

private:
    CPU& cpu;  ///< Reference to the CPU object for potential interactions with CPU state.

    std::unordered_map<uint16_t, std::function<uint32_t()>> readMap;  ///< Map of ports to their respective read functions.
    std::unordered_map<uint16_t, std::function<void(uint32_t)>> writeMap;  ///< Map of ports to their respective write functions.

    /**
     * @brief Helper function to check if a port is mapped.
     * 
     * @param port The I/O port to check.
     * @return true if the port is mapped, false otherwise.
     */
    [[nodiscard]] bool isPortMapped(uint16_t port) const noexcept;

    friend class CPU;
};

#endif // IO_HPP

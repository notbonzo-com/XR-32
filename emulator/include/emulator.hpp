#ifndef EMULATOR_HPP
#define EMULATOR_HPP

#include <components/cpu.hpp>
#include <components/memory.hpp>
#include <components/io.hpp>
#include <components/interrupts.hpp>

/**
 * @brief The Emulator class encapsulates the entire system emulation.
 *
 * This class manages the CPU, memory, IO, and interrupt components, providing methods to load programs,
 * execute the emulation, and reset the system state. The Emulator class serves as the main interface
 * for running the emulated system.
 */
class Emulator {
public:
    /**
     * @brief Constructs the Emulator object, initializing all components.
     * 
     * Initializes the CPU, memory, IO, and interrupt systems, preparing the emulator for program loading and execution.
     */
    Emulator();

    /**
     * @brief Loads a program from the specified file into memory.
     * 
     * This method reads the binary or assembly file specified by the filepath and loads its contents
     * into the emulated memory, starting at a predefined memory address.
     * 
     * @param filepath The path to the file containing the program to be loaded.
     * @param paddr The memory address at which the program should be loaded.
     * @param assemble If true, the file will be parsed as an assembly file and assembled, otherwise it will be loaded directly.
     * @throws std::runtime_error if the file cannot be read or if the program exceeds memory limits.
     */
    void loadProgram(std::string_view filepath, uint32_t paddr, bool assemble = false);

    /**
     * @brief Runs the emulation, executing the loaded program.
     * 
     * This method starts the emulation loop, continuously executing instructions until a halt condition is encountered
     * or an interrupt signals the end of execution.
     */
    void run();

    /**
     * @brief Resets the emulator to its initial state.
     * 
     * Resets the CPU, clears the memory, reinitializes the IO and interrupt systems, and prepares the emulator for a new run.
     */
    void reset();

private:
    CPU cpu;                 ///< The CPU component, responsible for instruction execution.
    Memory memory;           ///< The Memory component, simulating the system's memory.
    IO io;                   ///< The IO component, handling input/output operations.
    Interrupts interrupts;   ///< The Interrupts component, managing system interrupts.

    /**
     * @brief Executes a single emulation cycle.
     * 
     * This method fetches, decodes, and executes a single instruction, simulating one CPU cycle.
     * It also handles any interrupts that may occur during the cycle.
     */
    void executeCycle();
};

#endif // EMULATOR_HPP

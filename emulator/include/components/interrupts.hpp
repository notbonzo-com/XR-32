#ifndef INTERRUPTS_HPP
#define INTERRUPTS_HPP

#include <array>
#include <functional>
#include <cstdint>

class CPU;  // Forward declaration

/**
 * @brief The Interrupts class manages interrupt handling for the CPU.
 *
 * This class handles the registration of Interrupt Service Routines (ISRs), 
 * invoking ISRs when an interrupt occurs, and managing the CPU context during interrupts.
 */
class Interrupts {
public:
    /**
     * @brief Constructs an Interrupts object with a reference to the CPU.
     * 
     * @param cpuRef Reference to the CPU object that will be managed by this Interrupts class.
     */
    explicit Interrupts(CPU& cpuRef) noexcept : cpu(cpuRef) {}

    /**
     * @brief Triggers an interrupt of the give number
     * 
     * @param interruptNumber The number of the interrupt to trigger
     * @param error_code <optional> The error code to set to EI0
     */
    void triggerInterrupt(uint8_t interruptNumber, uint8_t error_code = 0);

    /**
     * @brief Handles the IRET instruction
     * 
     * This method handles the execution of an IRET instruction.
     */
    void triggerIret();

private:
    CPU& cpu;  ///< Reference to the CPU object to manage context and state during interrupts.

    /**
     * @brief Saves the current CPU context before an ISR is executed.
     * 
     * This method stores the state of CPU registers and flags to preserve the execution context.
     */
    void saveContext();

    /**
     * @brief Restores the CPU context after an ISR has completed.
     * 
     * This method restores the CPU registers and flags to their state before the interrupt occurred.
     */
    void restoreContext();

    /**
     * @brief Helper function to fetch the ISR address from the IVT.
     * 
     * @param interruptNumber The interrupt number to fetch the ISR address for.
     */
    uint32_t fetchISRAddress(uint8_t interruptNumber) const;

    friend class CPU;
};

/**
 * @brief Enum representing different interrupt types.
 */
enum InterruptType {
    DivideByZero = 0x00,
    InvalidOpcode = 0x01,
    PageFault = 0x02,
    GeneralProtectionFault = 0x03,
    OverflowException = 0x04,
    DoubleFault = 0x05,
    AlignmentCheck = 0x06,
    NonMaskableInterrupt = 0x07,
    UserInterrupt1 = 0x08,
    UserInterrupt2 = 0x09,
    UserInterrupt3 = 0x0A,
    Reserved = 0xFF,
};

/**
 * @brief Enum representing error codes for various exceptions.
 */
enum GeneralProtectionFaultErrorCode {
    PrivilegedInstruction = 0x00,
    UserModeToKernelMemory = 0x01,
    ExecuteNonExecutableMemory = 0x02,
    WriteToReadOnlyMemory = 0x03,
    UnauthorizedIOAccess = 0x04,
    InvalidCpuModeInstruction = 0x05,
    ReservedSystemRegisterAccess = 0x06,
    WriteToPrivilegedRegister = 0x07,
};

enum PageFaultErrorCode {
    PageNotPresent = 0x00,
    WriteOnReadOnlyPage = 0x01,
    UserModeToKernelPage = 0x02,
    ReservedBitsSet = 0x03,
    InstructionFetchFromNonExecutablePage = 0x04,
};

enum OverflowExceptionErrorCode {
    OverflowDuringAddSub = 0x00,
    OverflowDuringMultiply = 0x01,
    UnderflowDuringDivide = 0x02,
    UnderflowDuringSubtract = 0x03,
    OverflowDuringShift = 0x04,
};

enum AlignmentCheckErrorCode {
    MisalignedAccess = 0x00,
    Misaligned16BitWordAccess = 0x01,
    Misaligned32BitWordAccess = 0x02,
    Misaligned64BitWordAccess = 0x03,
    MisalignedAccessInUserMode = 0x04,
};

#endif // INTERRUPTS_HPP
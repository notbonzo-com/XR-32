# XR-32
eXtended RISC 32 bit architecture

RISC-inspired with a reduced set of instructions compared to x86.
This documentation goes over the current revision of the architecture, revision 1.0v1
For other revisions, please refer to the [History](/-Documentation/History.md)

## Registers
### Common use registers:
 - 32 general-purpose registers (`R0-R31`), 32-bit wide.
 - `I0` (Instruction Pointer 0) - Stores the address of the next instruction to be executed and automatically increments, 32-bit wide. Unoperatable on directly.

### Special use registers:
 - `S0` (Stack Pointer 0) - Stores the address of the top of the stack and is automatically incremented/decremented on stack operations, 32-bit wide.
 - `S1` (Stack Pointer 1) - Kernel stack pointer, used for specialized stack operations such as handling interrupts or managing a secondary stack (Not usable in usermode). `S1` also automatically adjusts with stack operations.
 - `FR` (Flags Register) - 8-bit wide.
 - `IVTR` - Interrupt vector table pointer, stores the physical address of the interrupt vector table, 32-bit wide.
 - `IE0` - Interrupt type register, stores the optional error code of an interrupt (cleared to 0 by hardware if interrupt doesn't use an error code), 8-bit wide.
 - `IE1` - Interrupt type register, stores the `I0` of the interrupted instruction, 32-bit wide.
 - `IE2` - Interrupt type register, stores the `S0` of the current task (`S0` is *NOT* overriden, so the stack is in both `IE2` and `S0`), 32-bit wide.
 - `IE3` - Interrupt type register, stores the `FR` of the interrupted instruction, 8-bit wide.
 - `IE4` - Interrupt type register, stores the `MSR` of the interrupted state, 32-bit wide.
 - `TPDR` - Top page directory pointer, stores the physical address of the top page directory, 32-bit wide.
 - `TSP` - Task state pointer, optional register for kernel to point it to the task's current state, 32-bit wide.
 - `PRR` - Processor revision register, holds the processor revision number, read-only, 8-bit wide. (Example: 01000001 -> 0100v0001 -> 1.0v1)
 - `MSR` - Mode status register, mode complex usage explained in [MSR](#MSR) section, 32-bit wide.

### Register Hexadecimal Representation
| Register | Hex | Register | Hex | Register | Hex | Register | Hex |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `R0` | 0x0 | `R8` | 0x8 | `R16` | 0x10 | `R24` | 0x18 |
| `R1` | 0x1 | `R9` | 0x9 | `R17` | 0x11 | `R25` | 0x19 |
| `R2` | 0x2 | `R10` | 0xA | `R18` | 0x12 | `R26` | 0x1A |
| `R3` | 0x3 | `R11` | 0xB | `R19` | 0x13 | `R27` | 0x1B |
| `R4` | 0x4 | `R12` | 0xC | `R20` | 0x14 | `R28` | 0x1C |
| `R5` | 0x5 | `R13` | 0xD | `R21` | 0x15 | `R29` | 0x1D |
| `R6` | 0x6 | `R14` | 0xE | `R22` | 0x16 | `R30` | 0x1E |
| `R7` | 0x7 | `R15` | 0xF | `R23` | 0x17 | `R31` | 0x1F |
| `I0` | 0x20 | `S0` | 0x21 | `S1` | 0x22 | `FR` | 0x23 |
| `IVTR` | 0x24 | `IE0` | 0x25 | `IE1` | 0x26 | `IE2` | 0x27 |
| `IE3` | 0x28 | `TPDR` | 0x29 | `TSP` | 0x2A | `PRR` | 0x2B |
| `MSR` | 0x2C |  |  |  |  |  |  |

### Flags
The `FR` register is used to store the internal state of the processor, also known as "flags".
There are 8 flags in the `FR` register:

 -> **Carry (C)** bit 0: Indicates a carry or borrow between the lower four bits (nibble) and the upper four bits.
 -> **Zero (Z)** bit 1: Indicates that the result of the operation is zero.
 -> **Sign (S)** bit 2: Indicates that the result of the operation is negative.
 -> **Overflow (V)** bit 3: Indicates that there was an overflow during the operation.
 -> **Error (E)** bit 4: Indicates that the operation was not successfull. Can be used freely by software but also used by `cmp`.
 -> **Interrupt (I)** bit 5: Defines if interrupts are enabled.
 -> **Growth (G)** bit 6: Controls the growth direction of the `S0` (Stack Pointer 0). When set, `S0` grows downward (from high memory to low memory); when cleared, it grows upward (from low memory to high memory).
 -> **Trap (T)** bit 7: If enabled, after every instruction, a trap interrupt is generated.

The high three bits ( I - T ) are only modifiable in supervisor mode.

### MSR
The highest bit of the `MSR` is used to specify if the processor is operating in supervisor mode or usermode.
The next 3 bits are reserved
The remaining 28 bits can be freely used to point to any usermode-available kernel structure.
The `MSR` is only writable in supervisor mode and any attempt to write it in usermode will trigger a [Integrity Protection Fault (IPF)](#integrity-protection-fault-error-code-table) with the error code 0x07. However, it is possible to read the `MSR` register in usermode.

## Instructions
### Instruction Format
All instructions are 64 bits wide, there are types of instructions:
 - **R/I-Type instructions**:
    - Opcode `op` (6 bits): 63-58
    - Addressing Mode `am` (2 bits): 57-56
    - Main operand, depending on `am` is either:
        - Immediate `i` (32 bits): 55-24
        - Register `rs` (32 bits): 55-24
    - Second operand, if enabled (higher bit of `am`):
        - Register offset `ro` (8 bits): 23-15
    - Reserved (8 bits): 7-0
 - **N/A-Type instructions**:
    - Opcode `op` (6 bits): 63-58
    - Reserved (58 bitd): 58-0
 - **RR/I-Type instructions**:
    - Opcode `op` (6 bits): 63-58
    - Addressing Mode `am` (2 bits): 57-56
    - Main operand, depending on `am` is either:
        - Immediate `i` (32 bits): 48-15 - higher bit of `am` defines if this is an immediate value or an address
        - Register `rs` (32 bits): 48-15
    - Destination Register `rd` (8 bits): 55-48
    - Reserved (16 bits): 15-0

### Instruction Set Architecture Table (ISA)
| Instruction | Opcode | Type | Addressing Modes | Affected Flags | Description |
| ----------- | ------ | ---- | ---------------- | -------------- | ----------- |
| `ADD`       | `0x01` | RR/I | Addr, Imm        | C, Z, S, V     | Adds `rs`, `i` or value at address `i` into `rd`, depending on `am`. Carry flag is used to extend the result if needed, zero flag is set if the result is zero, sign flag is set if the result is negative and overflow flag is set if there was an overflow. This instruction can only operate on general purpose registers, `R0` to `R31` as both `rs` and `rd`. |
| `SUB`       | `0x02` | RR/I | Addr, Imm        | Z, S, V        | Substracts `rs`, `i`, or the value at address `i` from `rd`, depending on `am`. Zero flag is set if the result is zero, sign flag is set if the result is negative and overflow flag is set if there was an underflow. This instruction can only operate on general purpose registers, `R0` to `R31` as both `rs` and `rd`. |
| `MUL`       | `0x03` | RR/I | Addr, Imm        | C, Z, S, V     | Multiplies `rd` with `rs`, `i`, or the value at address `i` and stores the result in `rd` depending on `am`. Carry flag is used to extend the result if needed. Zero flag is set if the result is zero, sign flag is set if the result is negative and overflow flag is set if there was an overflow. This instruction can only operate on general purpose registers, `R0` to `R31` as both `rs` and `rd`. |
| `DIV`       | `0x04` | RR/I | Addr, Imm        | Z, S, E     | Divides `rd` by `rs`, `i`, or the value at address `i` and stores the result in `rd` depending on `am`. Zero flag is set if the result is zero, sign flag is set if the result is negative and error flag is set if attempting to divide by zero. This instruction can only operate on general purpose registers, `R0` to `R31` as both `rs` and `rd`. |
| `AND`       | `0x05` | RR/I | Addr, Imm        | Z           | Performs bitwise AND on `rd` and `rs`, `i`, or the value at address `i` and stores the result in `rd` depending on `am`. Zero flag is set if the result is zero. This instruction can only operate on general purpose registers, `R0` to `R31` as both `rs` and `rd`. |
| `OR`        | `0x06` | RR/I | Addr, Imm        | Z           | Performs bitwise OR on `rd` and `rs`, `i`, or the value at address `i` and stores the result in `rd` depending on `am`. Zero flag is set if the result is zero. This instruction can only operate on general purpose registers, `R0` to `R31` as both `rs` and `rd`. |
| `XOR`       | `0x07` | RR/I | Addr, Imm        | Z           | Performs bitwise XOR on `rd` and `rs`, `i`, or the value at address `i` and stores the result in `rd` depending on `am`. Zero flag is set if the result is zero. This instruction can only operate on general purpose registers, `R0` to `R31` as both `rs` and `rd`. |
| `LSL`       | `0x08` | RR/I | Addr, Imm        | C, Z        | Perform a logical shift left on `rd` by `rs`, `i`, or the value at address `i` depending on `am`. Carry flag is set to the **latest** bit shifted out. Zero flag is set if the result is zero. This instruction can only operate on general purpose registers, `R0` to `R31` as both `rs` and `rd`. |
| `LSR`       | `0x09` | RR/I | Addr, Imm        | C, Z        | Perform a logical shift right on `rd` by `rs`, `i`, or the value at address `i` depending on `am`. Carry flag is set to the **latest** bit shifted out. Zero flag is set if the result is zero. This instruction can only operate on general purpose registers, `R0` to `R31` as both `rs` and `rd`. |
| `CMP`       | `0x0A` | RR/I | Addr, Imm        | C, Z, S, E  | Compares `rd` with `rs`, `i`, or the value at address `i` depending on `am`. If both values are equal, carry flag `C` is set. If one of the values is zero, zero flag `Z` is set. Sign flag `S` is set if both values are negative and error flag `E` is set if `rd` is greater than operand or cleared if less than. This instruction can only operate on general purpose registers, `R0` to `R31` as both `rs` and `rd`. |
| `MOV`       | `0x0B` | RR/I | Addr, Imm        | Z, S        | Copies `rs`, `i`, or the value at address `i` into `rd`, depending on `am`. If value copied is zero, zero flag `Z` is set. Sign flag `S` is set if value copied is negative. This instruction can only operate on general purpose registers, `R0` to `R31` as both `rs` and `rd`. |
| `ZEXT`      | `0x0C` | RR/I | N/A              | N/A         | Allows to sign extend an 8-bit register (`FR`, `IE3` and `PRR`) into a 32-bit general purpose register (`R0` to `R31`) as `rd`. Only allows `am` as 00 and `rs` as `FR`, `PRR` or if in supervisor mode `IE3`. |
| `MFS`       | `0x0D` | RR/I | N/A              | N/A         | Allows to copy a 32-bit special use register into a general purpose register (`R0` to `R31`) as `rd`. Only allows `am` as 00 and `rs` as `S0`, or in supervisor mode any special use register. |
| `MTS`       | `0x0E` | RR/I | N/A              | N/A         | Allows to copy a general purpose register (`R0` to `R31`) into a special use register as `rs`. Only allows `am` as 00 and `rd` as `S0`, or in supervisor mode any special use register. Can not operate on read only special use registers (`PRR`). |
| `LDR`       | `0x0F` | R/I  | Imm, Reg, Offset | Z, S        | Allows to load a value from memory specified at address `i`, or address held by register `rs`, plus optionally `offset` into a general purpose register (`R0` to `R31`) as `rd`, depending on `am`. If value loaded is zero, zero flag `Z` is set. Sign flag `S` is set if value loaded is negative. |
| `STR`       | `0x10` | R/I  | Imm, Reg, Offset | N/A         | Allows to store a value from general purpose register (`R0` to `R31`) into memory specified at address `i`, or address held by register `rs`, plus optionally `offset` depending on `am`. |
| `JMP`       | `0x11` | R/I  | Imm, Reg, Offset | N/A         | Allows to jump to address `i`, or address held by register `rs`, plus optionally `offset` depending on `am`. This jump is uncondition and doesn't save any state or return address |
| `JAR`       | `0x12` | R/I  | Imm, Reg, Offset | N/A         | Allows to jump to address `i`, or address held by register `rs`, plus optionally `offset` depending on `am`. This jump is uncondition and saves return address into `R31`. |
| `BEQ`       | `0x13` | R/I  | Imm, Reg, Offset | N/A         | Allows to branch to address `i`, or address held by register `rs`, plus optionally `offset` depending on `am`. Only branches if carry flag `C` is set. |
| `BNE`       | `0x14` | R/I  | Imm, Reg, Offset | N/A         | Allows to branch to address `i`, or address held by register `rs`, plus optionally `offset` depending on `am`. Only branches if carry flag `C` is cleared. |
| `BZ`        | `0x15` | R/I  | Imm, Reg, Offset | N/A         | Allows to branch to address `i`, or address held by register `rs`, plus optionally `offset` depending on `am`. Only branches if zero flag `Z` is set. |
| `BNZ`       | `0x16` | R/I  | Imm, Reg, Offset | N/A         | Allows to branch to address `i`, or address held by register `rs`, plus optionally `offset` depending on `am`. Only branches if zero flag `Z` is set. |
| `BG`        | `0x17` | R/I  | Imm, Reg, Offset | N/A         | Allows to branch to address `i`, or address held by register `rs`, plus optionally `offset` depending on `am`. Only branches if error flag `E` is set. (Meaning `rd` was greater in `CMP`) |
| `BL`        | `0x18` | R/I  | Imm, Reg, Offset | N/A         | Allows to branch to address `i`, or address held by register `rs`, plus optionally `offset` depending on `am`. Only branches if error flag `E` is cleared. (Meaning `rd` was less in `CMP`) |
| `CALL`      | `0x19` | R/I  | Imm, Reg, Offset | N/A         | Allows to call a subroutine at address `i`, or address held by register `rs`, plus optionally `offset` depending on `am`. Return address is pushed to user stack. |
| `KCALL`     | `0x1A` | R/I  | Imm, Reg, Offset | N/A         | Allows to call a subroutine at address `i`, or address held by register `rs`, plus optionally `offset` depending on `am`. Return address is pushed to kernel stack. Only allowed in supervisor mode. |
| `PUSH`      | `0x1B` | R/I  | Imm, Reg         | N/A         | Allows to push a value from general purpose register (`R0` to `R31`) in `rs` or a 32 bit immidiate (`i`) on onto user stack. |
| `POP`       | `0x1C` | R/I  | Reg              | N/A         | Allows to pop a value from user stack into general purpose register (`R0` to `R31`) |
| `KPUSH`     | `0x1D` | R/I  | Imm, Reg         | N/A         | Allows to push a value from general purpose register (`R0` to `R31`) in `rs` or a 32 bit immidiate (`i`) on onto kernel stack. Only allowed in supervisor mode. |
| `KPOP`      | `0x1E` | R/I  | Reg              | N/A         | Allows to pop a value from kernel stack into general purpose register (`R0` to `R31`). Only allowed in supervisor mode. |
| `SWI`       | `0x1F` | R/I  | Reg, Imm         | N/A         | Generates an interrupt by software, with the interrupt number specified in `rs` or `i` depending on `am`. |
| `INC`       | `0x20` | R/I  | Reg              | N/A         | Increments `rs` by 1. |
| `DEC`       | `0x21` | R/I  | Reg              | N/A         | Decrements `rs` by 1. |
| `RET`       | `0x22` | N/A  | N/A              | N/A         | Pops return address from user stack and jumps to it. |
| `KRET`      | `0x23` | N/A  | N/A              | N/A         | Pops return address from kernel stack and jumps to it. Only allowed in supervisor mode. |
| `IRET`      | `0x24` | N/A  | N/A              | N/A         | Restores the pre-interrupt state from `IE` registers and jumps to `IE1` address. Only allowed in supervisor mode. |
| `NOP`       | `0x25` | N/A  | N/A              | N/A         | Does nothing. |
| `HLT`       | `0x26` | N/A  | N/A              | N/A         | Halts the processor. |
| `IN`        | `0x27` | RR/I | Reg, Imm         | N/A         | Allows to read a value from a port into general purpose register (`R0` to `R31`) as `rd`. The port is specified by the value in `rs` or `i` depending on `am`. |
| `OUT`       | `0x28` | RR/I | Reg, Imm         | N/A         | Allows to write a value from general purpose register (`R0` to `R31`) as `rd` to a port specified by `rs` or `i` depending on `am`. |

## Interrupts and the Interrupt Vector Table (IVT)
The Interrupt Vector Table (IVT) is a fixed table in memory that contains the addresses of the interrupt service routines (ISRs). Each entry in the IVT corresponds to a specific interrupt number and points to the memory address where the ISR for that interrupt is located.

### IVT Structure
- The IVT starts at memory address held by `IVTR` of linear memory.
- Each entry in the IVT is 32 bits (4 bytes) wide, representing the memory address of the ISR.
- Interrupt numbers (INT_NUM) are assigned sequentially starting from `0`.

### Reserved Interrupts
| Vector Number | Error Code | Name | Description |
| ------------- | ---------- | ---- | ----------- |
| `0x0`         | None       | IOP  | Invalid Optcode interrupt occurs when the cpu recives a non-valid opcode while fetching a new instruction. |
| `0x1`         | Yes        | PF   | Page Fault interrupt occurs when one of possible page fault conditions occur. More information about specific causes can be found in the [Page Fault Error Code Table](#page-fault-error-code-table) |
| `0x2`         | Yes        | IPF  | Integrity Protection Fault interrupt occurs when one of the possible integrity protection faults occur. More information about specific causes can be found in the [Integrity Protection Fault Error Code Table](#integrity-protection-fault-error-code-table) |
| `0x3`         | Yes        | NMI  | Non Maskable Interrupt interrupt occurs when an error occurs while trying to handle an interrupt. More information about specific causes can be found in the [Non Maskable Interrupt Error Code Table](#non-maskable-interrupt-error-code-table) |
| `0x4`         | None       | BRK  | Breakpoint interrupt occurs every cycle when the trap `T` flag is set. |

### Interrupt Handling Process
When an interrupt occurs, the CPU follows this procedure:

1. **Save Context**:
 - Store the current state of the CPU (registers):
   - `IE0` stores the optional error code (if applicable) or `0`.
   - `IE1` stores the `I0` (Instruction Pointer) of the interrupted instruction.
   - `IE2` stores the `S0` (Stack Pointer) of the current task.
   - `IE3` stores the `FR` (Flags Register) of the interrupted instruction.
   - `IE4` stores the `MSR` (Mode Status Register) of the interrupted state.

2. **Restore Default Flags and MSR**:
 - `FR` (Flags Register) is set to 0x00.
 - `MSR` (Mode Status Register) is set to 0x80000000.

3. **Fetch the ISR Address from IVT**:
 - Using the interrupt vector number the corresponding ISR address is fetched from the IVT using the simple formula:
    ISR_Addr = IVTR + (INT_VEC_NUM * 4)

4. **Jump to ISR Address**:
 - The ISR address is loaded into the `I0` (Instruction Pointer) register and the CPU jumps to the ISR address.

5. **Restore Context**:
 - The saved state of the CPU (registers) is restored from the `IE` registers.

6. **Return from ISR**:
 - The CPU checks the `MSR` (now restored from `IE4`) to determine if it should switch back to user mode.
 - Execution resumes at the next instruction after the one that was interrupted.

### Error Codes for Exceptions
Some exceptions in the XR-32 architecture generate error codes that provide additional information about the cause of the exception. These error codes are stored in the `IE0` register when the exception occurs. Below are the error codes for the Integrity Protection Fault (IPF), Page Fault (PF) and Non Maskable Interrupt (NMI) exceptions:

#### Integrity Protection Fault Error Code Table
The Integrity Protection Fault occurs when the CPU detects an illegal memory access, privilege violation, or other protection-related errors. The error code in `IE0` provides details about the cause of the fault.

| Error Code (Hex) | Description                                                            |
|------------------|------------------------------------------------------------------------|
| `0x00`           | Attempt to execute a privileged instruction                            |
| `0x01`           | Unauthorized access to I/O ports                                       |
| `0x02`           | Attempt to execute an instruction in an invalid CPU mode               |
| `0x03`           | Illegal access to a reserved system register (such as writing to `I0`) |
| `0x04`           | Write to privileged register                                           |

#### Page Fault Error Code Table
The Page Fault occurs when the CPU tries to access a page that is not present in memory or when there is a violation of access rights.

| Error Code (Hex) | Description                                    |
|------------------|------------------------------------------------|
| `0x00`           | Page not present                               |
| `0x01`           | Write operation on a read-only page            |
| `0x02`           | User-mode access to a supervisor page          |
| `0x03`           | Reserved bits set in the page table            |
| `0x04`           | Instruction fetch from a non-executable page   |

#### Non Maskable Interrupt Error Code Table
The Non Maskable Interrupt occurs when the CPU fails to handle an interrupt. The error code in `IE0` provides details about the cause of the fault.

| Error Code (Hex) | Description                                                            |
|------------------|------------------------------------------------------------------------|
| `0x00`           | IVT entry not present                                                  |
| `0x01`           | ISR Address is 0x00000000 (reserved)                                   |

## Paging
Paging is a memory management technique that allows the physical address space of a process to be noncontiguous. It is designed to efficiently manage memory, prevent fragmentation, and enhance security by isolating process address spaces.

### Key Components of Paging in XR-32
1. **Page Directory and Top Page Directory (`TPDR`)**:
   - **Purpose**: Stores the physical address of the top-level page directory. This directory contains pointers to page tables.
   - **Size**: 4 KB (each entry is 32 bits, or 4 bytes, and there are 1024 entries).
   - **Entries**: Each entry points to a page table in physical memory.

2. **Page Table**:
   - **Purpose**: Contains the physical addresses of the pages.
   - **Size**: 4 KB (each entry is 32 bits, or 4 bytes, and there are 1024 entries).
   - **Entries**: Each entry points to a 4 KB page in physical memory.

3. **Page Table Entries (PTEs)**:
   - Each entry in a page table is 32 bits.
   - **Structure of a PTE**:
     - **Bits 0-11**: Reserved for flags (e.g., Present, Read/Write, User/Supervisor).
     - **Bits 12-31**: Physical address of the page (aligned to 4 KB).

4. **Page Size**:
   - XR-32 uses a fixed page size of 4 KB.

5. **Relevant Flags in PTE**:
   - **P (Present)**: Indicates whether the page is present in physical memory (1 if present, 0 if not).
   - **R/W (Read/Write)**: Indicates if the page is writable (1 for writable, 0 for read-only).
   - **U/S (User/Supervisor)**: Determines the privilege level (1 for user, 0 for supervisor).
   - **A (Accessed)**: Set by the CPU when the page is accessed. (Fun fact, its not set by the CPU while emulating)
   - **D (Dirty)**: Set by the CPU when the page is cached by the cpu. (Fun fact, its not set by the CPU while emulating)
   - **The rest are reserved for future revisions or avaiable for use by the kernel**

### Page Translation Process
When the CPU accesses a linear address, it undergoes a two-level translation:

1. **Page Directory Lookup**:
   - The most significant 10 bits of the linear address are used as an index into the Page Directory.
   - The Page Directory Entry (PDE) points to the base address of the relevant Page Table.

2. **Page Table Lookup**:
   - The next 10 bits of the linear address are used as an index into the Page Table.
   - The Page Table Entry (PTE) provides the base address of the physical page.

3. **Offset**:
   - The least significant 12 bits of the linear address are used as an offset within the 4 KB page.

### Linear Address Translation Example
Given a 32-bit linear address:
```
31 22 21 12 11 0
+-----------+------------+-------------+
| Directory | Table | Offset |
| Index | Index | |
+-----------+------------+-------------+
```
- **Directory Index**: Bits 31-22
- **Table Index**: Bits 21-12
- **Offset**: Bits 11-0

### Page Directory and Page Table Structures
#### Page Directory Entry (PDE) Structure
Each entry in the Page Directory points to a Page Table. There are no flags for Page Directories and the full 32 bits point to the physical address of the relevant Page Table.

#### Page Table Entry (PTE) Structure
Each entry in a Page Table points to a physical page in memory. The structure of a Page Table Entry (PTE) is as follows:

| Bits      | Field                     | Description                                                                                       |
|-----------|---------------------------|---------------------------------------------------------------------------------------------------|
| 31 - 12   | Physical Page Base Address| The physical base address of a 4 KB page in memory. This is a 20-bit address, aligned to 4 KB.    |
| 11        | Dirty (D)                 | Set by the CPU when the page is cached by the CPU.                                                |
| 10        | Accessed (A)              | Set by the CPU when the page is accessed.                                                         |
| 9 - 7     | AVL                       | Available for use by the kernel for any purpose.                                                  |
| 6         | Reserved                  | Reserved for future use. Must be set to 0.                                                        |
| 5         | User/Supervisor (U/S)     | Determines the privilege level: 1 for user mode, 0 for supervisor mode.                           |
| 4         | Read/Write (R/W)          | Indicates if the page is writable: 1 for writable, 0 for read-only.                               |
| 3 - 2     | AVL                       | Available for use by the kernel for any purpose.                                                  |
| 1         | Reserved                  | Reserved for future use. Must be set to 0.                                                        |
| 0         | Present (P)               | Indicates if the page is present in physical memory: 1 if present, 0 if not.                      |

## I/O Operations in XR-32 Architecture
Input/Output (I/O) operations are critical for enabling the XR-32 architecture to interact with external devices, such as keyboards, displays, storage devices, and other peripherals. The XR-32 architecture supports a flexible I/O mechanism that allows for efficient communication with these devices.

### Memory-Mapped I/O vs. Port-Mapped I/O
In the XR-32 architecture, I/O can be handled using either Memory-Mapped I/O or Port-Mapped I/O:

1. **Memory-Mapped I/O**:
   - In this approach, I/O devices are mapped into the same address space as program memory and data memory. This allows I/O devices to be accessed using standard memory instructions like `LDR` and `STR`.
   - Each I/O device is assigned a unique memory address range, which the CPU uses to read from or write to the device.

2. **Port-Mapped I/O**:
   - In contrast, Port-Mapped I/O uses a separate address space specifically for I/O devices. Ports are addressed using special instructions (`IN` and `OUT`) that are distinct from standard memory access instructions.
   - Each I/O device is assigned a unique port number, which the CPU uses to perform I/O operations.

### Interrupt-Driven I/O
In addition to basic I/O operations, the XR-32 architecture supports interrupt-driven I/O, which can improve efficiency by allowing the CPU to perform other tasks while waiting for an I/O operation to complete. When an I/O device is ready, it can trigger an interrupt, causing the CPU to temporarily halt its current task, handle the I/O operation, and then resume its previous activity. This is specific to each I/O device.

### Direct Memory Access (DMA)
DMA For high speed transfers will be implemented in future revisions.
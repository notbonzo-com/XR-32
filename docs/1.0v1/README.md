# XR-32
eXtended RISC 32 bit architecture

RISC-inspired with a reduced set of instructions compared to x86.
This documentation goes over the current revision of the architecture, revision 1.0v1
For other revisions, please refer to the [History](docs/History.md)

## Registers
 - 32 general-purpose registers (`R0-R31`), 32-bit wide.
 - `I0` (Instruction Pointer 0) - Stores the address of the next instruction to be executed and automatically increments, 32-bit wide.
 - `S0` (Stack Pointer 0) - Stores the address of the top of the stack and is automatically incremented/decremented on stack operations, 32-bit wide.
 - `S1` (Stack Pointer 1) - Kernel stack pointer, used for specialized stack operations such as handling interrupts or managing a secondary stack (Not usable in usermode). `S1` also automatically adjusts with stack operations, allowing for efficient management of multiple stack frames or contexts.
 - `FR` (Flags Register) - 8-bit wide.

 Special use registers:
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
| Register | Hexadecimal Representation |
| --- | --- |
| `R0` | 0x0 |
| `R1` | 0x1 |
| `R2` | 0x2 |
| `R3` | 0x3 |
| `R4` | 0x4 |
| `R5` | 0x5 |
| `R6` | 0x6 |
| `R7` | 0x7 |
| `R8` | 0x8 |
| `R9` | 0x9 |
| `R10` | 0xA |
| `R11` | 0xB |
| `R12` | 0xC |
| `R13` | 0xD |
| `R14` | 0xE |
| `R15` | 0xF |
| `R16` | 0x10 |
| `R17` | 0x11 |
| `R18` | 0x12 |
| `R19` | 0x13 |
| `R20` | 0x14 |
| `R21` | 0x15 |
| `R22` | 0x16 |
| `R23` | 0x17 |
| `R24` | 0x18 |
| `R25` | 0x19 |
| `R26` | 0x1A |
| `R27` | 0x1B |
| `R28` | 0x1C |
| `R29` | 0x1D |
| `R30` | 0x1E |
| `R31` | 0x1F |
| `I0` | 0x20 |
| `S0` | 0x21 |
| `S1` | 0x22 |
| `FR` | 0x23 |
| `IVTR` | 0x24 |
| `IE0` | 0x25 |
| `IE1` | 0x26 |
| `IE2` | 0x27 |
| `IE3` | 0x28 |
| `TPDR` | 0x29 |
| `TSP` | 0x2A |
| `PRR` | 0x2B |
| `MSR` | 0x2C |

### Flags
The `FR` register is used to store the internal state of the processor, also known as "flags".
There are 8 flags in the `FR` register:

| Flag | Bit | Description |
| --- | --- | --- |
| **Carry (C)** | 0 | Indicates an overflow out of the most significant bit in arithmetic operations. Used primarily in addition and subtraction. |
| **Zero (Z)** | 1 | Set when the result of an operation is zero. Used for conditional jumping based on whether a value is zero. |
| **Sign (S)** | 2 | Reflects the sign of the result in signed arithmetic operations. Set if the result is negative (i.e., the most significant bit is 1). |
| **Auxiliary (A)** | 3 | Used in Binary-Coded Decimal (BCD) arithmetic operations. Indicates a carry or borrow between the lower four bits (nibble) and the upper four bits. |
| **Interrupt (I)** | 4 | Enables or disables interrupts. When set, interrupts are enabled; when cleared, interrupts are disabled. |
| **Direction (D)** | 5 | Controls the growth direction of the `S0` (Stack Pointer 0). When set, `S0` grows downward (from high memory to low memory); when cleared, it grows upward (from low memory to high memory). |
| **Overflow (O)** | 6 | Indicates that the result of an arithmetic operation is too large to be represented in the destination operand (signed overflow). |
| **Negative (N)** | 7 | Used as a general-purpose flag in some architectures to indicate a negative result, though in this context, it may serve specific CPU-specific purposes. |

#### Flag Descriptions and Usage

##### 1. **Carry (C) Flag**
- **Description**: The Carry flag is set when an arithmetic operation results in a carry out of the most significant bit (MSB) in unsigned arithmetic operations. For subtraction, it indicates a borrow.
- **Typical Use**:
  - **Addition**: When adding two unsigned numbers, if the sum exceeds the maximum value that can be stored in the destination register, the Carry flag is set.
  - **Subtraction**: When subtracting, if a borrow occurs (i.e., the minuend is less than the subtrahend), the Carry flag is set.
  - **Multi-word arithmetic**: In extended precision arithmetic, the Carry flag from one addition/subtraction is used in the next.

##### 2. **Zero (Z) Flag**
- **Description**: The Zero flag is set when the result of an arithmetic or logical operation is zero.
- **Typical Use**:
  - **Conditional jumping**: Instructions that depend on the result being zero, such as `JE` (Jump if Equal) and `JZ` (Jump if Zero), will check the Zero flag.
  - **Loop Control**: Used in loops to determine if the end condition (e.g., a counter reaching zero) has been met.

##### 3. **Sign (S) Flag**
- **Description**: The Sign flag indicates the sign of the result in signed arithmetic operations. It is set if the MSB (which indicates the sign in signed numbers) is 1, meaning the result is negative.
- **Typical Use**:
  - **Signed Comparisons**: The Sign flag is useful for determining the outcome of signed comparisons and conditional jumps based on whether a result is positive or negative.
  - **Arithmetic Operations**: Helps distinguish between positive and negative results in signed operations.

##### 4. **Auxiliary (A) Flag**
- **Description**: The Auxiliary flag is used in Binary-Coded Decimal (BCD) arithmetic. It indicates a carry or borrow between the lower and upper nibbles (4-bit sections) of the result.
- **Typical Use**:
  - **BCD Adjustments**: Primarily used in instructions that adjust the result of BCD arithmetic operations to ensure a valid BCD result.
  - **Legacy Systems**: Though not commonly used in modern systems, it’s essential for operations requiring precise BCD calculations.

##### 5. **Interrupt (I) Flag**
- **Description**: The Interrupt flag controls the CPU's ability to respond to maskable interrupts. When this flag is set, interrupts are enabled; when it is cleared, interrupts are disabled.
- **Typical Use**:
  - **Critical Sections**: Temporarily disable interrupts during critical code sections to prevent race conditions or inconsistent state.
  - **System Initialization**: Set or clear during boot or when entering/exiting critical modes of operation.

##### 6. **Direction (D) Flag**
- **Description**: The Direction flag controls the growth direction of the `S0` (Stack Pointer 0). When set, `S0` grows downward (from high memory to low memory); when cleared, it grows upward (from low memory to high memory).
- **Typical Use**:
  - **Stack Management**: Controls the direction in which the stack grows, allowing for flexible stack management depending on the needs of the application or operating system.
  - **Function Calls**: Determines how local variables and return addresses are pushed onto the stack during function calls.

##### 7. **Overflow (O) Flag**
- **Description**: The Overflow flag is set when the result of a signed arithmetic operation is too large to be represented in the destination register (i.e., it causes a signed overflow).
- **Typical Use**:
  - **Signed Arithmetic**: Used to detect overflow in signed addition and subtraction, especially important for detecting errors in signed operations.
  - **Conditional jumping**: Can be used in instructions like `JO` (Jump if Overflow) to handle overflow conditions gracefully.

##### 8. **Negative (N) Flag**
- **Description**: This flag indicates whether the result of an operation is negative, although its exact usage may vary depending on the architecture. It can be redundant with the Sign flag in some contexts.
- **Typical Use**:
  - **General-Purpose**: Can be used in custom instructions or specific scenarios where a simple indication of negativity is required.
  - **Alternative Sign Flag**: In some architectures, this flag might be used alongside or instead of the Sign flag for specific operations.

#### Usage of Flags in Assembly
In assembly programming for XR-32, these flags will be implicitly set by many operations and can be explicitly checked or modified using specific instructions. They play a crucial role in conditional execution, loops, arithmetic operations, and system-level programming.
```assembly
ADD R1, R2, R3    ; Add R2 and R3, result in R1
JZ  ZeroLabel     ; Jump to ZeroLabel if Zero flag is set (if R1 is 0)
```
In this example, if the addition of `R2` and `R3` results in zero, the Zero flag (`Z`) is set, and the program jumps to ZeroLabel.
**Example: Handling Overflow**
```assembly
ADD R1, R2, R3    ; Add R2 and R3, result in R1
JO  OverflowLabel ; Jump to OverflowLabel if the Overflow flag is set
```
Here, the Overflow flag is checked to ensure that any signed overflow during the addition is properly handled.

### MSR
The highest bit of the `MSR` is used to specify if the processor is operating in kernel mode or user mode.
The next 3 bits are reserved
The remaining 28 bits can be freely used to point to any usermode-available kernel structure.
The `MSR` is only writable in kernel mode any any attempt to write it in user mode will trigger a [General Protection Fault (GPF)](#general-protection-fault-gpf---interrupt-0x03) with the error code 0x07.

## Instruction Format
Fixed-length 64-bit instructions.

### R-Type (Register): *`opcode | rd | rs1 | rs2 | shamt | func`*
- Opcode (6 bits): Bits 63–58
- rd (5 bits): Bits 57–53 (destination register)
- rs1 (5 bits): Bits 52–48 (source register 1)
- rs2 (5 bits): Bits 47–43 (source register 2)
- shamt (6 bits): Bits 42–37 (shift amount for shift operations)
- func (8 bits): Bits 36–29 (reserved for future use)
- reserved (29 bits): Bits 28–0

### I-Type (Immediate): *`opcode | rd | rs1 | immediate`*
- Opcode (6 bits): Bits 63–58
- rd (5 bits): Bits 57–53 (destination register)
- rs1 (5 bits): Bits 52–48 (source register 1)
- Immediate (32 bits): Bits 47–16 (immediate value, sign/zero-extended)
- reserved (16 bits): Bits 15–0

### J-Type (Jump): *`opcode | address`*
- Opcode (6 bits): Bits 63–58
- Address (32 bits): Bits 57–26 (jump target address)
- reserved (26 bits): Bits 25–0

### Instruction Set Architecture Table (ISA)
| Instruction | Opcode | Addressing Mode | Affected Flags  | Description |
|-------------|--------|-----------------|-----------------|-------------|
| `ADD`       | `0x01` | Register        | C, Z, S, O      | Adds two registers (`rs1` + `rs2`) and stores the result in `rd`. |
| `SUB`       | `0x02` | Register        | C, Z, S, O      | Subtracts `rs2` from `rs1` and stores the result in `rd`. |
| `AND`       | `0x03` | Register        | Z               | Performs bitwise AND on `rs1` and `rs2`, stores the result in `rd`. |
| `OR`        | `0x04` | Register        | Z               | Performs bitwise OR on `rs1` and `rs2`, stores the result in `rd`. |
| `XOR`       | `0x05` | Register        | Z               | Performs bitwise XOR on `rs1` and `rs2`, stores the result in `rd`. |
| `LSL`       | `0x06` | Register, Imm   | C, Z            | Logical shift left `rs1` by `shamt` positions, stores result in `rd`. |
| `LSR`       | `0x07` | Register, Imm   | C, Z            | Logical shift right `rs1` by `shamt` positions, stores result in `rd`. |
| `LDR`       | `0x08` | Register, Base+Offset | N/A       | Loads the value from memory (`rs1 + offset`) into `rd`. |
| `STR`       | `0x09` | Register, Base+Offset | N/A       | Stores the value in `rs2` to memory location (`rs1 + offset`). |
| `JMP`       | `0x0A` | Absolute Address | N/A            | Sets the Program Counter (`I0`) to the provided address. |
| `JAL`       | `0x0B` | Absolute Address | N/A            | Stores return address in `R31`, then jumps to the address. |
| `BEQ`       | `0x0C` | Register, Offset | Z              | If `rs1` equals `rs2`, adds the offset to the `I0`. |
| `BNE`       | `0x0D` | Register, Offset | Z              | If `rs1` does not equal `rs2`, adds the offset to the `I0`. |
| `MOV`       | `0x0E` | Register         | N/A            | Copies the value from `rs1` to `rd`. |
| `CMP`       | `0x0F` | Register         | Z, S, C, O     | Compares `rs1` and `rs2`, sets flags based on the result. |
| `PUSH`      | `0x10` | Register         | N/A            | Pushes `rs1` onto the stack, adjusts `S0` accordingly. |
| `POP`       | `0x11` | Register         | N/A            | Pops the top of the stack into `rd`, adjusts `S0`. |
| `CALL`      | `0x12` | Absolute Address | N/A            | Pushes the return address onto the stack and jumps to the target address. |
| `RET`       | `0x13` | N/A              | N/A            | Pops the return address from the stack and jumps to it. |
| `IRET`      | `0x14` | N/A 	            | N/A            | Returns from an interrupt, restoring `I0` and `S0` from `IE*`. |
| `NOP`       | `0x15` | N/A              | N/A            | No operation; the processor does nothing for one cycle. |
| `HLT`       | `0x16` | N/A              | N/A            | Halts the processor until the next interrupt or reset. |
| `MUL`       | `0x17` | Register         | C, Z, S, O     | Multiplies `rs1` by `rs2`, stores the result in `rd`. |
| `DIV`       | `0x18` | Register         | C, Z, S, O     | Divides `rs1` by `rs2`, stores the quotient in `rd`. |
| `MOD`       | `0x19` | Register         | C, Z, S, O     | Divides `rs1` by `rs2`, stores the remainder in `rd`. |
| `NOT`       | `0x1A` | Register         | Z, S           | Performs bitwise NOT on `rs1`, stores the result in `rd`. |
| `NEG`       | `0x1B` | Register         | C, Z, S, O     | Negates the value in `rs1`, stores the result in `rd`. |
| `INC`       | `0x1C` | Register         | C, Z, S, O     | Increments the value in `rd` by 1. |
| `DEC`       | `0x1D` | Register         | C, Z, S, O     | Decrements the value in `rd` by 1. |
| `ASL`       | `0x1E` | Register, Imm    | C, Z           | Arithmetic shift left `rs1` by `shamt` positions, stores result in `rd`. |
| `ASR`       | `0x1F` | Register, Imm    | C, Z           | Arithmetic shift right `rs1` by `shamt` positions, stores result in `rd`. |
| `SWI`       | `0x20` | Immediate        | N/A            | Software interrupt, triggers an interrupt with the specified immediate value |
| `SEXT`      | `0x21` | Register         | N/A            | Sign-extends a value in `rs1` to 32 bits, stores the result in `rd`. |
| `ZEXT`      | `0x22` | Register         | N/A            | Zero-extends a value in `rs1` to 32 bits, stores the result in `rd`. |
| `MFS`       | `0x23` | Register         | N/A            | Move special register `rs1` to `rd`. |
| `MTS`       | `0x24` | Register         | N/A            | Move register `rd` to special register `rs1`. |

Instructions `SWI`, `HLT`, `NOP,` `IRET`, and `RET` are treated as J-type instructions, where the address field is either not used or used as an immediate value.

This table represents the complete instruction set for the XR-32 architecture, which is designed to balance simplicity and power by incorporating both RISC and CISC elements. Each instruction is assigned a unique hexadecimal opcode, ensuring that the assembly language remains straightforward while providing the necessary operations for a wide range of tasks.

The table categorizes the instructions by type, detailing their addressing modes, which define how the operands are specified. For instance, many instructions use a register addressing mode, where operands are contained within the CPU's registers. Some instructions utilize immediate values or base+offset addressing, offering flexibility in accessing data.

Flags are critical to the architecture's operation, as they reflect the state of the CPU after each instruction is executed. For example, arithmetic operations like `ADD` and `SUB` affect flags such as the Carry (`C`), Zero (`Z`), Sign (`S`), and Overflow (`O`). These flags can then be tested by conditional instructions, enabling the implementation of loops, conditional branches, and error handling mechanisms.

The inclusion of both logical (`LSL`, `LSR`) and arithmetic (`ASL`, `ASR`) shift operations allows for precise control over bit-level data manipulation, a crucial feature for low-level programming tasks such as cryptography or data compression.

`func` operand in R-type instructions is currently unused but is reserved for future revisions.

## Interrupts and the Interrupt Vector Table (IVT)

The Interrupt Vector Table (IVT) is a fixed table in memory that contains the addresses of the interrupt service routines (ISRs). Each entry in the IVT corresponds to a specific interrupt number and points to the memory address where the ISR for that interrupt is located.

### IVT Structure
- The IVT starts at a predefined memory address (`0x0000_0000`).
- Each entry in the IVT is 32 bits (4 bytes) wide, representing the memory address of the ISR.
- Interrupt numbers (INT_NUM) are assigned sequentially starting from `0`.

### Example IVT Entries
| INT_NUM | Address      | Description                  |
|---------|--------------|------------------------------|
| `0x00`  | `0x0000_0000` | Divide by Zero Exception      |
| `0x01`  | `0x0000_0010` | Invalid Opcode Exception      |
| `0x02`  | `0x0000_0020` | Page Fault Exception          |
| `0x03`  | `0x0000_0030` | General Protection Fault      |
| `0x04`  | `0x0000_0040` | Overflow Exception            |
| `0x05`  | `0x0000_0050` | Double Fault Exception        |
| `0x06`  | `0x0000_0060` | Alignment Check Exception     |
| `0x07`  | `0x0000_0070` | Non-Maskable Interrupt        |
| `0x08`  | `0x0000_0080` | User-defined Interrupt 1      |
| `0x09`  | `0x0000_0090` | User-defined Interrupt 2      |
| `0x0A`  | `0x0000_00A0` | User-defined Interrupt 3      |
| ...     | ...           | ...                           |
| `0xFF`  | `0x0000_0FF0` | Reserved                      |

### Accessing the IVT
- The `IVTR` (Interrupt Vector Table Register) holds the base address of the IVT. The IVT can be relocated by changing the value of `IVTR`.
- To locate the ISR for a specific interrupt, the CPU reads the corresponding entry from the IVT:
```
ISR_Address = IVTR + (INT_NUM * 4)
```
### Interrupt Handling Procedure

When an interrupt occurs, the CPU follows this procedure:

1. **Check the `MSR` Register**:
 - Determine whether the interrupt occurred in user mode or kernel mode by checking the highest bit of `MSR`.
 - If the interrupt occurs in user mode, the CPU switches to kernel mode by setting the appropriate bit in `MSR`.

2. **Save Context**:
 - Store the current state of the CPU (registers):
   - `IE0` stores the optional error code (if applicable) or `0`.
   - `IE1` stores the `I0` (Instruction Pointer) of the interrupted instruction.
   - `IE2` stores the `S0` (Stack Pointer) of the current task.
   - `IE3` stores the `FR` (Flags Register) of the interrupted instruction.
   - `IE4` stores the `MSR` (Mode Status Register) of the interrupted state.

3. **Fetch ISR Address**:
 - Use the interrupt number (INT_NUM) to fetch the ISR address from the IVT:
   ```
   ISR_Address = IVTR + (INT_NUM * 4)
   ```
 - Set `I0` to the ISR address, directing the CPU to execute the ISR.

4. **Execute ISR**:
 - The CPU executes the ISR located at the fetched address. The ISR is responsible for handling the specific interrupt (e.g., resetting hardware, processing data).

5. **Restore Context**:
 - After the ISR completes, the CPU restores the saved context:
   - Load `IE1` (I0), `IE2` (S0), `IE3` (FR), and `IE4` (MSR).
   - Return to the instruction that was interrupted.

6. **Return from Interrupt**:
 - The CPU checks the `MSR` (now restored from `IE4`) to determine if it should switch back to user mode (if applicable).
 - Execution resumes at the next instruction after the one that was interrupted.

### Error Codes for Exceptions
Some exceptions in the XR-32 architecture generate error codes that provide additional information about the cause of the exception. These error codes are stored in the `IE0` register when the exception occurs. Below are the error codes for the General Protection Fault (GPF), Page Fault (PF), Overflow Exception (OF), and Alignment Check Fault (#ACF):

#### General Protection Fault (GPF) - Interrupt `0x03`
The General Protection Fault occurs when the CPU detects an illegal memory access, privilege violation, or other protection-related errors. The error code in `IE0` provides details about the cause of the fault.

| Error Code (Hex) | Description                                    |
|------------------|------------------------------------------------|
| `0x00`           | Attempt to execute a privileged instruction |
| `0x01`           | User-mode access to kernel-mode memory |
| `0x02`           | Attempt to execute non-executable memory |
| `0x03`           | Attempt to write to a read-only memory from user-mode |
| `0x04`           | Unauthorized access to I/O ports |
| `0x05`           | Attempt to execute an instruction in an invalid CPU mode |
| `0x06`           | Illegal access to a reserved system register (such as writing to `I0`) |
| `0x07`           | Write to privileged register                   |

#### Page Fault (PF) - Interrupt `0x02`
The Page Fault occurs when the CPU tries to access a page that is not present in memory or when there is a violation of access rights.

| Error Code (Hex) | Description                                    |
|------------------|------------------------------------------------|
| `0x00`           | Page not present                               |
| `0x01`           | Write operation on a read-only page            |
| `0x02`           | User-mode access to a kernel-mode page         |
| `0x03`           | Reserved bits set in the page table            |
| `0x04`           | Instruction fetch from a non-executable page   |

### Overflow Exception (OF) and Underflow Exception - Interrupt `0x04`
The Overflow and Underflow exceptions occur when an arithmetic operation exceeds the capacity of the destination operand.

| Error Code (Hex) | Description                                    |
|------------------|------------------------------------------------|
| `0x00`           | Overflow occurred during addition/subtraction  |
| `0x01`           | Overflow occurred during multiplication        |
| `0x02`           | Underflow occurred during division             |
| `0x03`           | Underflow occurred during subtraction          |
| `0x04`           | Overflow occurred during shift operation       |

### Alignment Check Fault (#ACF) - Interrupt `0x06`
The Alignment Check Fault occurs when the CPU accesses data that is not properly aligned in memory, violating alignment requirements.

| Error Code (Hex) | Description                                    |
|------------------|------------------------------------------------|
| `0x00`           | Misaligned memory access (general)             |
| `0x01`           | Misaligned access to a 16-bit word             |
| `0x02`           | Misaligned access to a 32-bit double word      |
| `0x03`           | Misaligned access to a 64-bit quad word        |
| `0x04`           | Misaligned access in user mode                 |

### Error Code Storage in `IE0`
When an exception that generates an error code occurs, the error code is stored in the `IE0` register. If the exception does not generate an error code, `IE0` is cleared to `0x00` by hardware.

## Example Usage in Assembly
```assembly
; Example: Handling a Page Fault with specific error code

PF_Handler:
    MFS R1, IE0          ; Move special register IE0 to R1
    CMP  R1, 0x00        ; Check if it's a "page not present" fault
    JE   PageNotPresent  ; Jump to handler for page not present
    ; Other error handling code...

PageNotPresent:
    ; Handle the case where the page is not present
    ; ...
    IRET                 ; Return from interrupt
```

## Paging

Paging is a memory management technique that allows the physical address space of a process to be noncontiguous. It is designed to efficiently manage memory, prevent fragmentation, and enhance security by isolating process address spaces.

### Key Components of Paging in XR-32

1. **Top Page Directory (`TPDR`)**:
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
   - **U/S (User/Supervisor)**: Determines the privilege level (1 for user, 0 for supervisor/kernel).
   - **A (Accessed)**: Set by the CPU when the page is accessed.
   - **D (Dirty)**: Set by the CPU when the page is modified. (Fun fact, its not set by the CPU when the page is accessed.)

### Page Translation Process

When the CPU accesses a virtual address, it undergoes a two-level translation:

1. **Page Directory Lookup**:
   - The most significant 10 bits of the virtual address are used as an index into the Page Directory.
   - The Page Directory Entry (PDE) points to the base address of the relevant Page Table.

2. **Page Table Lookup**:
   - The next 10 bits of the virtual address are used as an index into the Page Table.
   - The Page Table Entry (PTE) provides the base address of the physical page.

3. **Offset**:
   - The least significant 12 bits of the virtual address are used as an offset within the 4 KB page.

### Virtual Address Translation

Given a 32-bit virtual address:
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

In the XR-32 architecture, both the Page Directory and Page Table are 4 KB in size, containing 1024 entries, each 32 bits (4 bytes) wide. These entries are used to map virtual addresses to physical addresses.

#### Page Directory Entry (PDE) Structure

Each entry in the Page Directory points to a Page Table. The structure of a Page Directory Entry (PDE) is as follows:

| Bits      | Field                   | Description                                                                                        |
|-----------|-------------------------|----------------------------------------------------------------------------------------------------|
| 31 - 12   | Page Table Base Address  | The physical base address of a Page Table. This is a 20-bit address, aligned to 4 KB.             |
| 11        | Reserved                 | This bit is reserved and should be set to 0.                                                      |
| 10        | Kernel Mode (K)          | If this bit is set, all page tables can only be writen from kernel mode. Overrides PTE values.    |
| 9         | Available (AVL)          | This bit is available for use by the operating system.                                            |
| 8         | Accessed (A)             | Set by the CPU when the page table is accessed.                                                   |
| 7         | Reserved                 | This bit is reserved and should be set to 0.                                                      |
| 6         | Available (AVL)          | This bit is available for use by the operating system.                                            |
| 5         | Available (AVL)          | This bit is available for use by the operating system.                                            |
| 4         | Page Size (PS)           | Must be set to 0, indicating 4 KB pages.                                                          |
| 3         | Cache Disabled (CD)      | When set, caching for the page table is disabled.                                                 |
| 2         | Write-Through (WT)       | When set, write-through caching is used.                                                          |
| 1         | Read/Write (R/W)         | Indicates whether the pages pointed to by this table are read-only (0) or read/write (1).         |
| 0         | Present (P)              | Indicates if the Page Table is present in memory (1) or not (0).                                  |

- **Page Table Base Address (Bits 31-12)**: This 20-bit field contains the physical address of a Page Table. Since the Page Table is aligned to 4 KB, the lower 12 bits of this address are always zero.
- **Present (P) (Bit 0)**: If set to 1, the Page Table is present in physical memory. If set to 0, accessing this entry will trigger a Page Fault.

#### Page Table Entry (PTE) Structure

Each entry in a Page Table points to a physical page in memory. The structure of a Page Table Entry (PTE) is as follows:

| Bits      | Field                   | Description                                                                                        |
|-----------|-------------------------|----------------------------------------------------------------------------------------------------|
| 31 - 12   | Physical Page Base Address | The physical base address of a 4 KB page in memory. This is a 20-bit address, aligned to 4 KB.  |
| 11        | Global (G)               | When set, prevents the TLB from invalidating the entry across context switches.                   |
| 10        | Kernel Mode (K)          | If this bit is set, page can only be writen to from kernel mode.                                  |
| 9         | Available (AVL)          | This bit is available for use by the operating system.                                            |
| 8         | Available (AVL)          | This bit is available for use by the operating system.                                            |
| 7         | Accessed (A)             | Set by the CPU when the page is accessed.                                                         |
| 6         | Dirty (D)                | Set by the CPU when the page is written to.                                                       |
| 5         | Reserved                 | This bit is reserved and should be set to 0.                                                      |
| 4         | Reserved                 | This bit is reserved and should be set to 0.                                                      |
| 3         | Cache Disabled (CD)      | When set, caching for the page is disabled.                                                       |
| 2         | Write-Through (WT)       | When set, write-through caching is used.                                                          |
| 1         | Read/Write (R/W)         | Indicates whether the page is read-only (0) or read/write (1).                                    |
| 0         | Present (P)              | Indicates if the page is present in memory (1) or not (0).                                        |

- **Physical Page Base Address (Bits 31-12)**: This 20-bit field contains the physical address of the 4 KB page. The lower 12 bits of this address are always zero, ensuring that the page is aligned on a 4 KB boundary.
- **Dirty (D) (Bit 6)**: This bit is set by the CPU when the page is modified, indicating that the page needs to be written back to disk or another storage medium.
- **Accessed (A) (Bit 7)**: This bit is set by the CPU whenever the page is accessed, used for tracking page usage for algorithms like Least Recently Used (LRU).

#### Example of PDE and PTE Structures

##### Page Directory Entry (PDE)

Suppose we have a PDE pointing to a Page Table at the physical address `0x00123C00`. The entry would look like this:

| Bit Position  | 31-12          | 11  | 10 | 9  | 8  | 7  | 6  | 5  | 4  | 3  | 2  | 1  | 0  |
|---------------|----------------|-----|----|----|----|----|----|----|----|----|----|----|----|
| Value         | 0x000123C       | 0   | 0  | 0  | 0  | 0  | 0  | 0  | 0  | 0  | 0  | 1  | 1  |

- **Page Table Base Address**: `0x000123C` (bits 31-12).
- **Present (P)**: `1`, indicating that the Page Table is present.
- **Read/Write (R/W)**: `1`, indicating that pages in this table can be written to.

##### Page Table Entry (PTE)

Suppose we have a PTE pointing to a physical page at address `0x00456A00`. The entry would look like this:

| Bit Position  | 31-12          | 11 | 10 | 9  | 8  | 7  | 6  | 5  | 4  | 3  | 2  | 1  | 0  |
|---------------|----------------|----|----|----|----|----|----|----|----|----|----|----|----|
| Value         | 0x000456A       | 0  | 0  | 0  | 0  | 1  | 1  | 0  | 0  | 0  | 0  | 1  | 1  |

- **Physical Page Base Address**: `0x000456A` (bits 31-12).
- **Present (P)**: `1`, indicating that the page is present.
- **Read/Write (R/W)**: `1`, indicating that the page can be written to.
- **Accessed (A)**: `1`, indicating that the page has been accessed.
- **Dirty (D)**: `1`, indicating that the page has been modified.

### Example Page Table and Directory Setup

```assembly
; Set up a page directory and page table for a simple process
MOV R1, PageDirectoryBase  ; R1 = Base address of Page Directory
MOV R2, PageTableBase      ; R2 = Base address of Page Table

; Set up a page directory entry (PDE)
MOV R3, R2                 ; Load the base address of Page Table into R3
OR R3, 0x03                ; Mark the entry as present and writable
STR [R1 + DirectoryIndex * 4], R3 ; Store the PDE in the Page Directory

; Set up a page table entry (PTE)
MOV R4, PageBaseAddress    ; R4 = Base address of the physical page
OR R4, 0x03                ; Mark the page as present and writable
STR [R2 + TableIndex * 4], R4 ; Store the PTE in the Page Table

MTS TPDR, R1               ; Set the Page Directory base address
```

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

### I/O Registers

The XR-32 architecture may define specific I/O registers that control and monitor the state of connected peripherals. These registers are typically memory-mapped or accessible via port-mapped I/O, depending on the device and the system design. 

- **Control Registers**: These are used to send commands to the I/O device, such as configuring settings or initiating operations.
- **Status Registers**: These registers provide feedback from the device, such as indicating whether it is ready to send or receive data.
- **Data Registers**: These registers are used to transfer data to and from the I/O device.

### I/O Instructions

The XR-32 architecture includes specific instructions designed to facilitate I/O operations using port-mapped I/O:

- **`IN rd, rs`**:
  - This instruction reads data from an I/O port. The port number is specified in the source register (`rs`), and the data read from the port is stored in the destination register (`rd`).

- **`OUT rd, rs`**:
  - This instruction writes data to an I/O port. The port number is specified in the destination register (`rd`), and the data to be written is taken from the source register (`rs`).

These instructions allow for efficient communication with I/O devices, with minimal overhead, and provide a simple interface for developers to interact with hardware.

### Interrupt-Driven I/O

In addition to basic I/O operations, the XR-32 architecture supports interrupt-driven I/O, which can improve efficiency by allowing the CPU to perform other tasks while waiting for an I/O operation to complete. When an I/O device is ready, it can trigger an interrupt, causing the CPU to temporarily halt its current task, handle the I/O operation, and then resume its previous activity.

### Direct Memory Access (DMA) (Optional)

For systems requiring high-speed data transfer between I/O devices and memory, the XR-32 architecture can support Direct Memory Access (DMA). DMA allows certain peripherals to directly read from or write to memory without involving the CPU for each byte of data transferred. This significantly reduces CPU overhead and improves system performance in data-intensive applications.
# XR-32 Assembler Grammar Specification (NASM-inspired)

This document outlines the grammar and conventions for the XR-32 architecture assembler, version 1.0v1. The syntax has been adapted to closely resemble NASM's style for readability and familiarity. The XR-32 assembly language is simple, RISC-like, and provides low-level control over CPU operations, featuring an efficient instruction set.

## 1. General Structure of Assembly Code

An XR-32 assembly program consists of labels, instructions, directives, and comments. Each line typically follows this format:

```asm
label:    instruction operand1, operand2    ; optional comment
```

- **label**: An identifier followed by a colon (`:`). It marks a position in code and can be used as a reference point for jumps and branches.
- **instruction**: The operation to be performed (e.g., `mov`, `add`, `jmp`). Instructions are case-insensitive.
- **operand1, operand2**: Registers, immediate values, or memory locations used by the instruction.
- **comment**: Begins with a semicolon (`;`) and continues to the end of the line. Comments are ignored by the assembler.

### Example
```asm
start:    mov r1, 10             ; Load immediate value 10 into r1
          add r2, r1, r3         ; Add r1 and r3, store result in r2
          jmp end                ; Jump to end label
end:      hlt                    ; Halt execution
```

## 2. Tokens

### 2.1 Labels
Labels are used to mark specific locations in code. They must begin with an alphabetic character (`A-Z`, `a-z`) and can include alphanumeric characters and underscores (`_`). Labels are case-sensitive. Nested labels can be created using a period (`.`) for hierarchical organization.

### Example of Advanced Labels
```asm
start:
    .loop:
        add r1, r2, r3
        cmp r1, 0
        jne .loop
    .done:
        hlt
```

### 2.2 Literals
Literals are values embedded directly into instructions.

- **Decimal**: e.g., `42`
- **Hexadecimal**: Prefixed with `0x`, e.g., `0x2A`

### 2.3 Registers
Registers are defined by the XR-32 architecture, including general-purpose registers (`r0`-`r31`) and special-purpose registers (`s0`, `s1`, `fr`, etc.). Register names are case-insensitive.

## 3. Instructions

### 3.1 Instruction Format
Instructions consist of an opcode and operands. Operands may be registers, literals, or memory addresses.

### 3.2 Operand Types
- **Register**: Use register names, e.g., `r4`.
- **Immediate**: Immediate values are used directly, e.g., `5` or `0xFF`.
- **Memory**: Memory locations are accessed via labels or addresses.

### 3.3 Syntax Examples
```asm
add r2, r3, r4           ; r2 = r3 + r4
sub r1, r5, 20           ; r1 = r5 - 20 (immediate value)
mov r0, 0x1000           ; Load value 0x1000 into r0
```

### 3.4 Instruction Types

#### 3.4.1 Arithmetic and Logical Instructions
- `add dest, src1, src2` : Add `src1` and `src2`, store result in `dest`.
- `sub dest, src1, imm` : Subtract immediate value from `src1`, store result in `dest`.
- `and dest, src1, src2` : Bitwise AND between `src1` and `src2`, store result in `dest`.

#### 3.4.2 Data Movement Instructions
- `mov dest, src` : Move value from `src` to `dest`.
- `ldr dest, [address]` : Load value from memory address into `dest`.
- `str src, [address]` : Store value from `src` into memory address.

#### 3.4.3 Branching Instructions
- `jmp label` : Unconditional jump to `label`.
- `beq reg, label` : Branch to `label` if `reg` is zero.
- `call label` : Call subroutine at `label`, saving return address.

### 3.5 Addressing Modes
- **Immediate Addressing**: Direct value, e.g., `mov r1, 10`.
- **Register Addressing**: Use register directly, e.g., `add r2, r3, r4`.
- **Memory Addressing**: Access using a label or address, e.g., `ldr r1, [data]`.

## 4. Directives

Directives control the behavior of the assembler and define data. They are not translated into machine code.

- `.org [address]` : Set the origin address for the code.
- `db value` : Define a byte.
- `dw value` : Define a word.
- `dd value` : Define a double word.

### Example
```asm
.org [0x1000]           ; Set code start address to 0x1000
data:   dw 0xFF         ; Define a word with value 0xFF
```

## 5. Pseudo-Instructions

Pseudo-instructions are simplified forms that are translated by the assembler into one or more real instructions.

- `nop` : No operation, often used for alignment or timing.
- `move` : Alias for `mov` for consistency.

## 6. Error Handling

The assembler will report errors with specific messages:
- **Syntax Error**: Incorrect instruction format or illegal use of registers.
- **Undefined Label**: A label is referenced but not defined.
- **Out of Range**: Literal values or addresses that exceed valid limits.

To fix errors, verify instruction syntax, ensure all labels are defined before use, and check that literals are within valid range.

## 7. Example Program

Here is an example program that demonstrates basic arithmetic and memory operations:

```asm
.org [0x2000]
start:    ldr r1, [value1]       ; Load value from address `value1` into r1
          ldr r2, [value2]       ; Load value from address `value2` into r2
          add r3, r1, r2         ; Add r1 and r2, store result in r3
          str r3, [result]       ; Store result at address `result`
          hlt                    ; Halt the processor

value1:   dw 0x3000
value2:   dw 0x3004
result:   dw 0x3008
```

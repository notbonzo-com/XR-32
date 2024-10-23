# XR-32 Emulator/Assembler/Disassembler

Welcome to the XR-32 emulator/assembler/disassembler repository! This tool is designed to simulate the XR-32 architecture, allowing developers to assemble XR-32 assembly code into machine code, disassemble machine code back into human-readable assembly, and emulate the execution of XR-32 binaries on their machine.

## Table of Contents
- [Features](#features)
- [Getting Started](#getting-started)
  - [Installation](#installation)
  - [Usage](#usage)
- [Flags and Command-Line Options](#flags-and-command-line-options)
- [Example Usage](#example-usage)
- [Architecture Overview](#architecture-overview)
- [Contributing](#contributing)
- [License](#license)

## Features
- **Emulation:** Run XR-32 binaries in a simulated environment, complete with full register, memory, and stack support.
- **Assembly:** Convert XR-32 assembly language into machine code, ready to be executed by the emulator.
- **Disassembly:** Reverse-engineer XR-32 machine code back into assembly language for analysis or debugging.
- **Interrupt and Paging Support:** Fully emulate XR-32's interrupt handling and paging features.
- **Debugging:** Ability to configure register dumps on certain events (interrupt and every N cycles).

## Getting Started

### Installation
To install the XR-32 emulator/assembler/disassembler, follow these steps:

1. Clone the repository:
   ```
   git clone https://github.com/TODO/xr32-emulator.git
   cd xr32-emulator
   ```

2. Build the project:
   ```
   make all
   ```

3. The `xr32-tool` binary will be created in the project directory.

### Usage
The `xr32-tool` binary provides all three functionalities—emulation, assembly, and disassembly. The tool automatically determines the operation mode based on the provided flags.

### Flags and Command-Line Options

- **General Flags:**
  - `-h`, `--help`: Displays help information about the tool and its usage.
  - `-v`, `--version`: Outputs the version of the emulator.

- **Assembly Mode:**
  - `-a`, `--assemble <source_file>`: Assembles the specified XR-32 assembly source file into a binary file.
  - `-o`, `--output <output_file>`: Specifies the output file for the assembled binary (default is `output.bin`).

- **Disassembly Mode:**
  - `-d`, `--disassemble <binary_file>`: Disassembles the specified XR-32 binary file into assembly code.
  - `-o`, `--output <output_file>`: Specifies the output file for the disassembled assembly code (default is `output.asm`).

- **Emulation Mode:**
  - `-e`, `--emulate <binary_file>`: Emulates the execution of the specified XR-32 binary file.
  - `-hdd`, `--harddisk <hdd_image>`: Loads the specified hard disk image for the emulated system.
  - `-fda`, `--floppy <floppy_image>`: Loads the specified floppy disk image.
  - `--B`, `--bios <bios_file>`: Specifies the BIOS file to load for system emulation.
  - `--mem <size>`: Specifies the amount of memory for the emulated system (e.g., `--mem 256M` for 256 MB).
  - `--serial <output>`: Redirects serial port output to stdout or a specified file.
  - `--debugcon <output>`: Redirects debug console output (port e9) to stdout or a specified file.
  - `--trace`: Enables instruction tracing, printing each executed instruction into stderr.
  - `-D`, `--dump <condition>`: Dumps the CPU state based on the specified condition:
    - `int`: Dump on every interrupt.
    - `<number>`: Dump after every specified number of clock cycles.

### Example Usage

- **Assembling a File:**
  ```
  ./xr32-tool --assemble program.asm --output program.bin
  ```

- **Disassembling a Binary:**
  ```
  ./xr32-tool --disassemble program.bin --output program.asm
  ```

- **Emulating a Program with BIOS and HDD:**
  ```
  ./xr32-tool --emulate --bios bios.bin -hdd hdd.img --mem 512M --serial serial_output.log --debugcon debug_output.log
  ```

- **Dumping on Every Interrupt and Redirecting Output:**
  ```
  ./xr32-tool --emulate -D int --serial stdout --debugcon debug_output.log
  ```

## Architecture Overview

### XR-32 Architecture Summary
The XR-32 is a 32-bit RISC-inspired architecture designed to balance simplicity and power. It features:

- **32 General-Purpose Registers (`R0-R31`)**: Each 32 bits wide.
- **Instruction Pointer (`I0`)**: Points to the next instruction to execute.
- **Two Stack Pointers (`S0`, `S1`)**: `S0` for user-mode and `S1` for kernel-mode stack operations.
- **Flags Register (`FR`)**: An 8-bit register holding various processor flags.
- **Special Registers**: Including `IVTR` for interrupt vector management, `TPDR` for paging, and `MSR` for mode status.

### Instruction Set
The XR-32 supports a variety of instructions for arithmetic, logic, memory access, control flow, and special operations. Key instruction formats include R-Type, I-Type, and J-Type, with a 32-bit fixed instruction size.

## Contributing
We welcome contributions from the community! To contribute:

1. Fork the repository.
2. Create a new branch for your feature or bugfix.
3. Commit your changes and open a pull request.

Please ensure your code follows our style guidelines and includes appropriate tests.

## License
This project is licensed under the MIT License. See the `LICENSE` file for details.

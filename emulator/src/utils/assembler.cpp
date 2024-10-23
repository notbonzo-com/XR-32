#include <utils/assembler.hpp>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

int32_t Assembler::parseImmediate(const std::string& immStr) {
    if (immStr.find("0x") == 0 || immStr.find("0X") == 0) {
        return std::stoi(immStr, nullptr, 16);
    } else {
        return std::stoi(immStr);
    }
}

bool Assembler::assemble(const std::string& sourceFile, const std::string& outputFile) {
    std::ifstream input(sourceFile);
    if (!input.is_open()) {
        throw std::runtime_error("Failed to open source file: " + sourceFile);
    }

    std::ofstream output(outputFile, std::ios::binary);
    if (!output.is_open()) {
        throw std::runtime_error("Failed to open output file: " + outputFile);
    }

    std::string line;
    while (std::getline(input, line)) {
        uint64_t machineCode = parseAssemblyLine(line);
        output.write(reinterpret_cast<const char*>(&machineCode), sizeof(machineCode));
    }

    input.close();
    output.close();
    return true;
}

uint64_t Assembler::parseAssemblyLine(const std::string& line) {
    std::vector<std::string> tokens = tokenizeLine(line);
    if (tokens.empty()) {
        throw std::runtime_error("Failed to parse empty instruction");
    }

    const std::string& mnemonic = tokens[0];
    uint64_t opcode = CPU::findInstruction(mnemonic);

    if (mnemonic == "NOP" || mnemonic == "HLT" || mnemonic == "IRET" || mnemonic == "RET") {
        return (opcode << 58);
    }

    if (mnemonic == "PUSH" || mnemonic == "POP") {
        if (tokens.size() != 2) {
            throw std::runtime_error(mnemonic + " requires exactly 1 register");
        }
        uint8_t rd = CPU::findRegister(tokens[1]);
        if (rd == 0xFF) {
            throw std::runtime_error("Invalid register in " + mnemonic + " instruction");
        }
        return (opcode << 58) | (static_cast<uint64_t>(rd) << 53);
    }

    if (mnemonic == "OUT" || mnemonic == "IN") {
        if (tokens.size() != 3) {
            throw std::runtime_error(mnemonic + " requires exactly 2 registers");
        }
        uint8_t rd = CPU::findRegister(tokens[1]);
        uint8_t rs1 = CPU::findRegister(tokens[2]);
        if (rd == 0xFF || rs1 == 0xFF) {
            throw std::runtime_error("Invalid register in " + mnemonic + " instruction");
        }
        return (opcode << 58) | (static_cast<uint64_t>(rd) << 53) | (static_cast<uint64_t>(rs1) << 48);
    }

    if (mnemonic == "SWI") {
        if (tokens.size() != 2) {
            throw std::runtime_error("SWI requires exactly 1 immediate value");
        }
        uint64_t immediate = std::stoul(tokens[1]);
        return (opcode << 58) | ((immediate & 0xFF) << 16);
    }

    if (mnemonic == "NOT" || mnemonic == "NEG") {
        if (tokens.size() != 3) {
            throw std::runtime_error(mnemonic + " requires exactly 2 registers");
        }
        uint8_t rd = CPU::findRegister(tokens[1]);
        uint8_t rs1 = CPU::findRegister(tokens[2]);
        uint8_t rs2 = 0;
        if (rd == 0xFF || rs1 == 0xFF) {
            throw std::runtime_error("Invalid register in " + mnemonic + " instruction");
        }
        return (opcode << 58) | (static_cast<uint64_t>(rd) << 53) |
               (static_cast<uint64_t>(rs1) << 48) | (static_cast<uint64_t>(rs2) << 43);
    }

    if (mnemonic == "INC" || mnemonic == "DEC") {
        if (tokens.size() != 2) {
            throw std::runtime_error(mnemonic + " requires exactly 1 register");
        }
        uint8_t rd = CPU::findRegister(tokens[1]);
        if (rd == 0xFF) {
            throw std::runtime_error("Invalid register in " + mnemonic + " instruction");
        }
        return (opcode << 58) | (static_cast<uint64_t>(rd) << 53);
    }

    if (mnemonic == "SEXT" || mnemonic == "ZEXT") {
        if (tokens.size() != 3) {
            throw std::runtime_error(mnemonic + " requires exactly 2 registers");
        }
        uint8_t rd = CPU::findRegister(tokens[1]);
        uint8_t rs1 = CPU::findRegister(tokens[2]);
        if (rd == 0xFF || rs1 == 0xFF) {
            throw std::runtime_error("Invalid register in " + mnemonic + " instruction");
        }
        return (opcode << 58) | (static_cast<uint64_t>(rd) << 53) |
               (static_cast<uint64_t>(rs1) << 48);
    }

    if (mnemonic == "MTS" || mnemonic == "MFS") {
        if (tokens.size() != 3) {
            throw std::runtime_error(mnemonic + " requires exactly 2 registers");
        }
        uint8_t rd = CPU::findRegister(tokens[1]);
        uint8_t rs1 = CPU::findRegister(tokens[2]);
        if (rd == 0xFF || rs1 == 0xFF) {
            throw std::runtime_error("Invalid register in " + mnemonic + " instruction");
        }
        return (opcode << 58) | (static_cast<uint64_t>(rd) << 53) |
               (static_cast<uint64_t>(rs1) << 48);
    }

    switch (opcode) {
        case 0x01: // ADD
        case 0x02: // SUB
        case 0x03: // AND
        case 0x04: // OR
        case 0x05: // XOR
        case 0x17: // MUL
        case 0x18: // DIV
        case 0x19: // MOD
            return assembleRType(tokens);

        case 0x08: // LDR
        case 0x09: // STR
        case 0x0C: // BEQ
        case 0x0D: // BNE
        case 0x0E: // MOV
        case 0x0F: // CMP
        case 0x21: // SEXT
        case 0x22: // ZEXT
        case 0x23: // MFS
        case 0x24: // MTS
        case 0x25: // OUT
        case 0x26: // IN
        case 0x06: // LSL
        case 0x07: // LSR
            return assembleIType(tokens);

        case 0x0A: // JMP
        case 0x0B: // JAL
        case 0x12: // CALL
            return assembleJType(tokens);

        case 0xFFFFFFFF: // Invalid opcode case handled earlier
            throw std::runtime_error("Invalid opcode: " + mnemonic);

        default:
            throw std::runtime_error("Unknown instruction type for opcode: " + mnemonic);
    }
}

uint64_t Assembler::assembleRType(const std::vector<std::string>& tokens) {
    if (tokens.size() != 4) {
        throw std::runtime_error("R-Type instruction requires 3 operands (rd, rs1, rs2)");
    }

    uint8_t rd = CPU::findRegister(tokens[1]);
    uint8_t rs1 = CPU::findRegister(tokens[2]);
    uint8_t rs2 = CPU::findRegister(tokens[3]);
    uint64_t opcode = CPU::findInstruction(tokens[0]);

    if (rd == 0xFF || rs1 == 0xFF || rs2 == 0xFF) {
        throw std::runtime_error("Invalid register in R-Type instruction");
    }

    return (opcode << 58) | (static_cast<uint64_t>(rd) << 53) |
           (static_cast<uint64_t>(rs1) << 48) | (static_cast<uint64_t>(rs2) << 43);
}

uint64_t Assembler::assembleIType(const std::vector<std::string>& tokens) {
    uint8_t rd, rs1;
    int32_t immediate;
    uint64_t opcode = CPU::findInstruction(tokens[0]);

    if (tokens[0] == "LDR" || tokens[0] == "STR") {
        if (tokens.size() == 3) {
            // If rs1 is omitted, use 0x2D for absolute addressing
            rd = CPU::findRegister(tokens[1]);
            rs1 = 0x2D;
            immediate = std::stoi(tokens[2]);
        } else if (tokens.size() == 4) {
            // Standard base + offset addressing
            rd = CPU::findRegister(tokens[1]);
            rs1 = CPU::findRegister(tokens[2]);
            immediate = std::stoi(tokens[3]);
        } else {
            throw std::runtime_error(tokens[0] + " requires 2 or 3 operands");
        }
    } else if (tokens[0] == "MOV" || tokens[0] == "CMP") {
        if (tokens.size() != 3) {
            throw std::runtime_error(tokens[0] + " requires exactly 2 operands");
        }
        rd = CPU::findRegister(tokens[1]);
        rs1 = CPU::findRegister(tokens[2]);
        immediate = 0;
    } else {
        if (tokens.size() != 4) {
            throw std::runtime_error(tokens[0] + " requires exactly 3 operands");
        }
        rd = CPU::findRegister(tokens[1]);
        rs1 = CPU::findRegister(tokens[2]);
        immediate = std::stoi(tokens[3]);
    }

    if (rd == 0xFF || rs1 == 0xFF) {
        throw std::runtime_error("Invalid register in " + tokens[0] + " instruction");
    }

    return (opcode << 58) | (static_cast<uint64_t>(rd) << 53) |
           (static_cast<uint64_t>(rs1) << 48) | ((static_cast<uint64_t>(immediate) & 0xFFFFFFFF) << 16);
}

uint64_t Assembler::assembleJType(const std::vector<std::string>& tokens) {
    if (tokens.size() != 2) {
        throw std::runtime_error("J-Type instruction requires 1 operand (address)");
    }

    uint64_t address = parseImmediate(tokens[1]);
    uint64_t opcode = CPU::findInstruction(tokens[0]);

    return (opcode << 58) | ((address & 0xFFFFFFFF) << 26);
}

std::vector<std::string> Assembler::tokenizeLine(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream stream(line);
    std::string token;

    while (stream >> token) {
        tokens.push_back(token);
    }

    return tokens;
}
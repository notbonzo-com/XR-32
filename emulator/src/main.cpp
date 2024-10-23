#include <iostream>
#include <string>
#include <emulator.hpp>
#include <fstream>
#include <utils/argparser.hpp>
#include <utils/assembler.hpp>
#include <optional>
#include <iomanip>

constexpr std::string_view resetColor = "\033[0m";
constexpr std::string_view boldColor = "\033[1m";
constexpr std::string_view yellowColor = "\033[33m";
constexpr std::string_view greenColor = "\033[32m";
constexpr std::string_view cyanColor = "\033[36m";

struct Config {
    std::optional<std::string> assembleFile;
    std::optional<std::string> outputFile;
    std::optional<std::string> disassembleFile;
    std::optional<std::string> emulateFile;
    std::optional<std::string> hddImage;
    std::optional<std::string> floppyImage;
    std::optional<std::string> biosFile;
    std::optional<std::string> memSize;
    std::optional<std::string> serialOutput;
    std::optional<std::string> debugconOutput;
    std::optional<std::string> dumpCondition;
    bool trace = false;
    bool showHelp = false;
    bool showVersion = false;
};

namespace config {
    constexpr std::string_view version = "0.0.1";

    constexpr std::string_view helpFlag = "--help";
    constexpr std::string_view helpShort = "-h";
    constexpr std::string_view versionFlag = "--version";
    constexpr std::string_view versionShort = "-v";

    constexpr std::string_view assembleFlag = "--assemble";
    constexpr std::string_view assembleShort = "-a";
    constexpr std::string_view outputFlag = "--output";
    constexpr std::string_view outputShort = "-o";

    constexpr std::string_view disassembleFlag = "--disassemble";
    constexpr std::string_view disassembleShort = "-d";
    constexpr std::string_view emulateFlag = "--emulate";
    constexpr std::string_view emulateShort = "-e";
    constexpr std::string_view hddFlag = "--harddisk";
    constexpr std::string_view floppyFlag = "--floppy";
    constexpr std::string_view biosFlag = "--bios";
    constexpr std::string_view memFlag = "--mem";
    constexpr std::string_view serialFlag = "--serial";
    constexpr std::string_view debugconFlag = "--debugcon";
    constexpr std::string_view traceFlag = "--trace";
    constexpr std::string_view dumpFlag = "--dump";
    constexpr std::string_view dumpShort = "-D";
}

void printHelp() {
    std::cout << boldColor << "XR-32 Emulator/Assembler/Disassembler\n" << resetColor
              << "\nUsage: " << cyanColor << "xr32-tool [options]\n" << resetColor
              << "\n" << boldColor << "Options:\n" << resetColor
              << yellowColor << "  -h, --help                " << resetColor << "Display this help information\n"
              << yellowColor << "  -v, --version             " << resetColor << "Show the version of the emulator\n"
              << "\n" << boldColor << "Assembly Mode:\n" << resetColor
              << yellowColor << "  -a, --assemble <source_file>\n" << resetColor
              << greenColor << "                            " << resetColor << "Assemble the specified XR-32 assembly source file into a binary file\n"
              << yellowColor << "  -o, --output <output_file>\n" << resetColor
              << greenColor << "                            " << resetColor << "Specify the output file for the assembled binary (default: output.bin)\n"
              << "\n" << boldColor << "Disassembly Mode:\n" << resetColor
              << yellowColor << "  -d, --disassemble <binary_file>\n" << resetColor
              << greenColor << "                            " << resetColor << "Disassemble the specified XR-32 binary file into assembly code\n"
              << yellowColor << "  -o, --output <output_file>\n" << resetColor
              << greenColor << "                            " << resetColor << "Specify the output file for the disassembled assembly code (default: output.asm)\n"
              << "\n" << boldColor << "Emulation Mode:\n" << resetColor
              << yellowColor << "  -e, --emulate <binary_file>\n" << resetColor
              << greenColor << "                            " << resetColor << "Emulate the execution of the specified XR-32 binary file\n"
              << yellowColor << "  -hdd, --harddisk <hdd_image>\n" << resetColor
              << greenColor << "                            " << resetColor << "Load the specified hard disk image for the emulated system\n"
              << yellowColor << "  -fda, --floppy <floppy_image>\n" << resetColor
              << greenColor << "                            " << resetColor << "Load the specified floppy disk image\n"
              << yellowColor << "  --B, --bios <bios_file>\n" << resetColor
              << greenColor << "                            " << resetColor << "Specify the BIOS file to load for system emulation\n"
              << yellowColor << "  --mem <size>\n" << resetColor
              << greenColor << "                            " << resetColor << "Specify the amount of memory for the emulated system (e.g., --mem 256M for 256 MB)\n"
              << yellowColor << "  --serial <output>\n" << resetColor
              << greenColor << "                            " << resetColor << "Redirect serial port output to stdout or a specified file\n"
              << yellowColor << "  --debugcon <output>\n" << resetColor
              << greenColor << "                            " << resetColor << "Redirect debug console output (port e9) to stdout or a specified file\n"
              << yellowColor << "  --trace                   " << resetColor << "Enable instruction tracing, printing each executed instruction into stderr\n"
              << yellowColor << "  -D, --dump <condition>\n" << resetColor
              << greenColor << "                            " << resetColor << "Dump the CPU state based on the specified condition:\n"
              << greenColor << "                              int     " << resetColor << "Dump on every interrupt\n"
              << greenColor << "                              <number>" << resetColor << " Dump after every specified number of clock cycles\n";
}

Config parseArguments(int argc, char** argv) {
    Config config;

    auto assembleLambda = [&](std::optional<std::string> value) {
        if (!value) {
            config.assembleFile = std::string();
        } else {
            config.assembleFile = *value;
        }
    };

    auto outputLambda = [&](std::optional<std::string> value) {
        if (!value) {
            config.outputFile = std::string();
        } else {
            config.outputFile = *value;
        }
    };

    ArgParser parser({
        {config::helpFlag, [&](std::optional<std::string>) { config.showHelp = true; }},
        {config::helpShort, [&](std::optional<std::string>) { config.showHelp = true; }},
        {config::versionFlag, [&](std::optional<std::string>) { config.showVersion = true; }},
        {config::versionShort, [&](std::optional<std::string>) { config.showVersion = true; }},
        {config::assembleFlag, assembleLambda},
        {config::assembleShort, assembleLambda},
        {config::outputFlag, outputLambda},
        {config::outputShort, outputLambda},
        {config::disassembleFlag, [&](std::optional<std::string> value) { config.disassembleFile = value; }},
        {config::disassembleShort, [&](std::optional<std::string> value) { config.disassembleFile = value; }},
        {config::emulateFlag, [&](std::optional<std::string> value) { config.emulateFile = value; }},
        {config::emulateShort, [&](std::optional<std::string> value) { config.emulateFile = value; }},
        {config::hddFlag, [&](std::optional<std::string> value) { config.hddImage = value; }},
        {config::floppyFlag, [&](std::optional<std::string> value) { config.floppyImage = value; }},
        {config::biosFlag, [&](std::optional<std::string> value) { config.biosFile = value; }},
        {config::memFlag, [&](std::optional<std::string> value) { config.memSize = value; }},
        {config::serialFlag, [&](std::optional<std::string> value) { config.serialOutput = value; }},
        {config::debugconFlag, [&](std::optional<std::string> value) { config.debugconOutput = value; }},
        {config::traceFlag, [&](std::optional<std::string>) { config.trace = true; }},
        {config::dumpFlag, [&](std::optional<std::string> value) { config.dumpCondition = value; }},
        {config::dumpShort, [&](std::optional<std::string> value) { config.dumpCondition = value; }}
    });

    parser.parse(argc, argv);
    return config;
}

void handleConfig(const Config& config) {
    if (config.showHelp) {
        printHelp();
        return;
    }

    if (config.showVersion) {
        std::cout << "XR-32 Emulator version " << config::version << std::endl;
        return;
    }

    if (config.assembleFile) {
        Assembler assembler;
        std::string outputFile = config.outputFile.value_or("");

        std::vector<uint8_t> machineCode;

        if (config.assembleFile->empty()) {
            std::cout << "Assembling from stdin. Type assembly code below (CTRL+D to end input):\n";
            std::string line;
            while (std::getline(std::cin, line)) {
                uint64_t binaryInstruction = assembler.parseAssemblyLine(line);
                for (int i = 0; i < 8; ++i) {
                    machineCode.push_back(static_cast<uint8_t>((binaryInstruction >> (i * 8)) & 0xFF));
                }
            }
        } else {
            std::cout << "Assembling file: " << *config.assembleFile << std::endl;
            bool success = assembler.assemble(*config.assembleFile, outputFile);

            if (!success) {
                std::cerr << "Error: Assembly failed for file " << *config.assembleFile << std::endl;
                return;
            }
        }

        if (!outputFile.empty()) {
            std::ofstream outFile(outputFile, std::ios::binary);
            if (!outFile) {
                std::cerr << "Error: Unable to open output file: " << outputFile << std::endl;
                return;
            }
            outFile.write(reinterpret_cast<const char*>(machineCode.data()), machineCode.size());
            std::cout << "Assembly successful. Output written to " << outputFile << std::endl;
        }

        std::cout << "Assembled binary (hex):\n";
        std::cout << std::hex << std::setfill('0');
        for (size_t i = 0; i < machineCode.size(); ++i) {
            std::cout << "0x" << std::setw(2) << static_cast<int>(machineCode[i]);
            if (i < machineCode.size() - 1) {
                std::cout << " ";
            }
        }
        std::cout << std::endl;

        return;
    }

    if (config.disassembleFile) {
        std::cout << "Disassembling file: " << *config.disassembleFile << std::endl;
        // TODO: Implement disassembly logic
    }

    if (config.emulateFile) {
        std::cout << "Emulating file: " << *config.emulateFile << std::endl;
        // TODO: Implement emulation logic
    }
}

int main(int argc, char** argv) {
    const auto config = parseArguments(argc, argv);
    handleConfig(config);
    return 0;
}

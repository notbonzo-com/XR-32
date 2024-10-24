#include <components/io.hpp>
#include <stdexcept>

uint32_t IO::readPort(uint16_t port) const noexcept {
    if (isPortMapped(port)) {
        return readMap.at(port)();
    }
    return 0;
}

void IO::writePort(uint16_t port, uint32_t value) noexcept {
    if (isPortMapped(port)) {
        writeMap.at(port)(value);
    }
}

void IO::mapDevice(uint16_t port, std::function<uint32_t()> readFunc, std::function<void(uint32_t)> writeFunc) {
    if (isPortMapped(port)) {
        throw std::invalid_argument("Port is already mapped");
    }
    readMap[port] = std::move(readFunc);
    writeMap[port] = std::move(writeFunc);
}

bool IO::isPortMapped(uint16_t port) const noexcept {
    return readMap.find(port) != readMap.end() && writeMap.find(port) != writeMap.end();
}

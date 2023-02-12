#include "Cpu.hpp"

using namespace chip8;

const size_t Cpu::kRequireRamSize = 4096;

Cpu::Cpu(IRandomAccessMemory* ram) : ram_(ram) {
    if (ram_.getCapacity() < kRequireRamSize) {
        throw std::runtime_error("RAM size is not enough");
    }
}

void Cpu::run() {
    while (true) {
        consumeClock();
    }
}

void Cpu::consumeClock() {
    uint16_t code = fetch();
    decode(code);
    execute();
}

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

uint16_t Cpu::fetch() {
    auto data = ram_->load(regs_.pc, 1);
    ++regs_.pc;
    return data[0];
}

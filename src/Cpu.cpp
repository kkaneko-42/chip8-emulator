#include "Cpu.hpp"

using namespace chip8;

Cpu::Cpu(IRandomAccessMemory* ram) : ram_(ram) {

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

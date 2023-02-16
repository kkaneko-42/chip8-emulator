#include "Cpu.hpp"
#include "Ram.hpp"
#include "TerminalDisplay.hpp"
#include "Keyboard.hpp"
#include <iostream>
#include <cstring>
#define USAGE "./chip8 <code to run>"

using namespace chip8;

// NOTE: codeはnull終端を仮定
static void importCode(Ram& ram, const char* code) {
    std::string code_str(code);
    // FIXME: magic number
    ram.store(0x200, {code_str.begin(), code_str.end()});
}

int main(int ac, char** av) {
    if (ac < 2) {
        std::cerr << USAGE << std::endl;
        return 1;
    }

    Ram ram;
    TerminalDisplay display;
    Keyboard keyboard;
    Cpu cpu(&ram, &display, &keyboard);

    importCode(ram, av[1]);
    cpu.init();
    cpu.run();
    return 0;
}

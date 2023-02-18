#include "Cpu.hpp"
#include "Ram.hpp"
#include "TerminalDisplay.hpp"
#include "Keyboard.hpp"
#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <cstring>
#define USAGE "./chip8 <filename>"

using namespace chip8;

// NOTE: codeはnull終端を仮定
static void importCode(Ram& ram, const char* filename) {
    std::ifstream ifs(filename, std::ios::in);
    if (!ifs) {
        throw std::runtime_error("failed to open file");
    }

    std::string code;
    ifs >> code;
    // FIXME: magic number
    ram.store(0x200, {code.begin(), code.end()});
}

int main(int ac, char** av) {
    if (ac < 2) {
        std::cerr << USAGE << std::endl;
        return 1;
    }

    Ram ram;
    TerminalDisplay display;
    Keyboard keyboard;
    Logger logger;
    Cpu cpu(&ram, &display, &keyboard, &logger);

    try {
        importCode(ram, av[1]);
        cpu.init();
        cpu.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

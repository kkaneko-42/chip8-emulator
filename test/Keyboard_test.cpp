#include "../src/Keyboard.hpp"
#include <unistd.h>
#include <termios.h>
#include <cstdlib>
#include <iostream>

using namespace chip8;

int main() {
    Keyboard keyboard;

    for (const auto& kv : Keyboard::kKeycodeMap) {
        Cpu::KeyCode keycode = kv.first;

        while (true) {
            std::cout << std::hex << "Wait for pressing " << keycode << " ..." << std::endl;
            if (keyboard.isPressing(keycode)) {
                std::cout << "pressed!!" << std::endl;
                break;
            }
        }

        std::cout << std::endl;
    }
}

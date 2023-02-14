#include "../src/TerminalDisplay.hpp"
#include <unistd.h>
#include <cassert>

using namespace chip8;

int main() {
    TerminalDisplay display;
    std::vector<unsigned char> sprites = {
        0xf0, 0x90, 0x90, 0x90, 0xf0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xf0, 0x10, 0xf0, 0x80, 0xf0, // 2
        0xf0, 0x10, 0xf0, 0x10, 0xf0, // 3
        0x90, 0x90, 0xf0, 0x10, 0x10, // 4
        0xf0, 0x80, 0xf0, 0x10, 0xf0, // 5
        0xf0, 0x80, 0xf0, 0x90, 0xf0  // 6
    };

    display.clear();
    for (size_t i = 0; i < sprites.size(); i += 5) {
        std::vector<unsigned char> sprite = {sprites.begin() + i, sprites.begin() + i + 5};
        display.renderSprite(0, 0, sprite);
        sleep(1);
        assert(display.renderSprite(0, 0, sprite));
        sleep(1);
    }

    return 0;
}

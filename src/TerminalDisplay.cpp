#include "TerminalDisplay.hpp"

using namespace chip8;

const size_t TerminalDisplay::kWidth = 64;
const size_t TerminalDisplay::kHeight = 32;

TerminalDisplay::TerminalDisplay() {
    initscr();
}

TerminalDisplay::~TerminalDisplay() {
    endwin();
}

bool TerminalDisplay::renderSprite(size_t x, size_t y, const std::vector<unsigned char>& sprite) {
    bool collision = false;

    for (const auto& sprite_row : sprite) {
        if ((frame_buffer_[y] & sprite_row) == 0) {
            collision = true;
        }
        frame_buffer_[y] ^= static_cast<uint64_t>(sprite_row) >> x;
        ++y;
    }

    return collision;
}

void TerminalDisplay::clear() {
    erase();
}

#include "TerminalDisplay.hpp"
#include <climits>

using namespace chip8;

const char TerminalDisplay::kPlotChar = '*';
const char TerminalDisplay::kEmptyChar = ' ';

TerminalDisplay::TerminalDisplay() {
    initscr();
}

TerminalDisplay::~TerminalDisplay() {
    endwin();
}

bool TerminalDisplay::renderSprite(size_t x, size_t y, const std::vector<unsigned char>& sprite) {
    bool collision = false;

    for (const auto& sprite_row : sprite) {
        auto row_bits = std::bitset<WIDTH>(sprite_row) << (WIDTH - x - CHAR_BIT);

        if ((frame_buffer_[y] & row_bits) != 0) {
            collision = true;
        }
        frame_buffer_[y] ^= row_bits;
        ++y;
    }
    render();

    return collision;
}

void TerminalDisplay::clear() {
    erase();
    refresh();
}

void TerminalDisplay::render() {
    for (size_t y = 0; y < HEIGHT; ++y) {
        for (size_t x = 0; x < WIDTH; ++x) {
            char c = (frame_buffer_[y][WIDTH - x - 1] == 1) ? kPlotChar : kEmptyChar;
            mvaddch(y, x, c);
        }
    }
    refresh();
}

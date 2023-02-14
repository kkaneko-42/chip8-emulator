#include "TerminalDisplay.hpp"

const size_t TerminalDisplay::kResolutionX = 64;
const size_t TerminalDisplay::kResolutionY = 32;

TerminalDisplay::TerminalDisplay() {
    initscr();
}

TerminalDisplay::~TerminalDisplay() {
    endwin();
}

bool TerminalDisplay::renderSprite(size_t x, size_t y, const std::vector<unsigned char>& sprite) {

}

void TerminalDisplay::clear() {

}

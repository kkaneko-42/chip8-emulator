#pragma once

#include "Cpu.hpp"
#include <bitset>
#include <ncurses.h>
#define WIDTH 64
#define HEIGHT 32

namespace chip8 {
    class TerminalDisplay : public Cpu::IDisplay {
        public:
            TerminalDisplay();
            ~TerminalDisplay();
            bool renderSprite(size_t x, size_t y, const std::vector<unsigned char>& sprite) override;
            void clear() override;
            void getResolution(size_t& x, size_t& y) override { x = WIDTH; y = HEIGHT; };

        private:
            static const char kPlotChar, kEmptyChar;

            void render();

            std::bitset<WIDTH> frame_buffer_[HEIGHT];
    };
}

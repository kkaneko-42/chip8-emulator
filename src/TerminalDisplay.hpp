#pragma once

#include "Cpu.hpp"
#include <ncurses.h>

namespace chip8 {
    class TerminalDisplay : public Cpu::IDisplay {
        public:
            TerminalDisplay();
            ~TerminalDisplay();
            bool renderSprite(size_t x, size_t y, const std::vector<unsigned char>& sprite) override;
            void clear() override;
            void getResolution(size_t& x, size_t& y) override { x = kResolutionX; y = kResolutionY; };

        private:
            static const size_t kResolutionX, kResolutionY;
    };
}

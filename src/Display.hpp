#pragma once

#include "Cpu.hpp"

namespace chip8 {
    class Display : public Cpu::IDisplay {
        public:
            void renderSprite(size_t x, size_t y, const std::vector<unsigned char>& sprite) override;
            void getResolution(size_t& x, size_t& y) override;
    };
}

#pragma once

#include "Cpu.hpp"

namespace chip8 {
    class Ram : public Cpu::IRandomAccessMemory {
        public:
            Ram();
            std::vector<unsigned char> load(uint16_t addr, size_t n) override;
            void store(uint16_t addr, const std::vector<unsigned char>& data) override;
            size_t getCapacity() override;

        private:
            static const size_t kCapacity;
            std::vector<unsigned char> data_;
    };
}

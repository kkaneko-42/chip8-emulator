#pragma once

#include "Cpu.hpp"
#include <map>

namespace chip8 {
    class Keyboard : public Cpu::IKeyboard {
        public:
            Keyboard();
            ~Keyboard();
            bool isPressing(Cpu::KeyCode keycode) override;
            Cpu::KeyCode acquireKey() override;
            
            static const std::map<Cpu::KeyCode, char> kKeycodeMap;
    };
}

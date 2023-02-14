#pragma once

#include "Cpu.hpp"
#include <map>

namespace chip8 {
    class Keyboard : public Cpu::IKeyboard {
        public:
            Keyboard();
            bool isPressing(Cpu::KeyCode keycode) override;
            
            static const std::map<Cpu::KeyCode, char> kKeycodeMap;
    };
}

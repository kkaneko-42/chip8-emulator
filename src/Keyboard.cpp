#include "Keyboard.hpp"
#include <ncurses.h>

using namespace chip8;

const std::map<Cpu::KeyCode, char> Keyboard::kKeycodeMap = {
    {Cpu::KeyCode::KEY_INVALID, EOF},
    {Cpu::KeyCode::KEY_0, 'x'},
    {Cpu::KeyCode::KEY_1, '1'},
    {Cpu::KeyCode::KEY_2, '2'},
    {Cpu::KeyCode::KEY_3, '3'},
    {Cpu::KeyCode::KEY_4, 'q'},
    {Cpu::KeyCode::KEY_5, 'w'},
    {Cpu::KeyCode::KEY_6, 'e'},
    {Cpu::KeyCode::KEY_7, 'a'},
    {Cpu::KeyCode::KEY_8, 's'},
    {Cpu::KeyCode::KEY_9, 'd'},
    {Cpu::KeyCode::KEY_A, 'z'},
    {Cpu::KeyCode::KEY_B, 'c'},
    {Cpu::KeyCode::KEY_C, '4'},
    {Cpu::KeyCode::KEY_D, 'r'},
    {Cpu::KeyCode::KEY_E, 'f'},
    {Cpu::KeyCode::KEY_F, 'v'}
};

Keyboard::Keyboard() {
    initscr();
}

Keyboard::~Keyboard() {
    endwin();
}

// 極めて短い間入力を受け付け、指定された入力があればtrue, なければfalseを返す
bool Keyboard::isPressing(Cpu::KeyCode keycode) {
    const int kInputDelay = 1; // milliseconds
    const char kMappedKeycode = kKeycodeMap.find(keycode)->second;

    timeout(kInputDelay);
    char c = getch();
    
    return (c == kMappedKeycode);
}

Cpu::KeyCode Keyboard::acquireKey() {
    char c = getch();
    
    // 取得したcがkKeycodeMapに含まれるか？
    for (const auto& kv : kKeycodeMap) {
        char mapped_keycode = kv.second;
        if (c == mapped_keycode) {
            return kv.first;
        }
    }

    // 入力が不正
    return Cpu::KeyCode::KEY_INVALID;
}

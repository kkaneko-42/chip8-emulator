#include "Keyboard.hpp"
#include <fcntl.h>
#include <cstdio>
#include <termios.h>
#include <unistd.h>
#include <iostream>

using namespace chip8;

const std::map<Cpu::KeyCode, char> Keyboard::kKeycodeMap = {
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
    struct termios save_settings;
    struct termios settings;

    tcgetattr(STDIN_FILENO, &save_settings);
    settings = save_settings;
    settings.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &settings);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
}

bool Keyboard::isPressing(Cpu::KeyCode keycode) {
    char c = getchar();
    fflush(stdin);

    std::cout << "c: " << c << std::endl;
    return c == kKeycodeMap.find(keycode)->second;
}

#include "../src/Keyboard.hpp"
#include <unistd.h>
#include <termios.h>
#include <cstdlib>
#include <iostream>
#include <ncurses.h>

using namespace chip8;

static void testAcquireKey() {
    Keyboard keyboard;

    printw("==== acquire test ====\n");
    for (const auto& kv : Keyboard::kKeycodeMap) {
        Cpu::KeyCode keycode = kv.first;

        while (true) {
            printw("Wait for %d...", keycode);
            if (keyboard.acquireKey() == keycode) {
                addstr("OK\n");
                break;
            } else {
                addstr("KO. try again\n");
            }
        }
    }
    clear();
}

static void testIsPressing() {
    Keyboard keyboard;

    printw("==== isPressing test ====\n");
    for (const auto& kv : Keyboard::kKeycodeMap) {
        Cpu::KeyCode keycode = kv.first;

        printw("Wait for %d", keycode);
        while (true) {
            if (keyboard.isPressing(keycode)) {
                printw("OK!\n");
                break;
            }
        }
    }
    clear();
}

int main() {
    testAcquireKey();
    testIsPressing();

    return 0;
}

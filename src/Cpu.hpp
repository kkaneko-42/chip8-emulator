#pragma once

#include <cstdint>
#define STACK_SIZE 16

namespace chip8 {
    class Cpu {
        struct Registers {
            // 一般的な用途のレジスタ
            unsigned char v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, va, vb, vc, vd, ve, vf;
            // メモリアドレスを格納するレジスタ
            uint16_t i;
            // タイマー
            // 0以外の値の時、60Hzで値がデクリメントされる
            unsigned char delay_timer, sound_timer;
            // program counter
            uint16_t pc;
            // stack pointer
            unsigned char sp;
            // NOTE: STACK_SIZEは、入れ子にできるルーチンの数(深さ)
            uint16_t stack[STACK_SIZE];
        };
    };
}

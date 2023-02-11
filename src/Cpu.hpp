#pragma once

#include <vector>
#include <cstdint>
#define STACK_SIZE 16

namespace chip8 {
    class Cpu {
        public:
            struct Registers {
                // 一般的な用途のレジスタ
                unsigned char v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, va, vb, vc, vd, ve, vf;
                // メモリアドレスを格納するレジスタ
                // NOTE: 通常、下位12bitのみ使われる
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

            class IRandomAccessMemory {
                public:
                    virtual ~IRamdomAccessMemory() {};

                    virtual std::vector<unsigned char> load(uint16_t addr, size_t n) = 0;
                    virtual void store(uint16_t addr, const std::vector<unsigned char>& data) = 0;
                    virtual size_t getCapacity() = 0;
            };
    };
}

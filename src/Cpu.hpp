#pragma once

#include <vector>
#include <map>
#include <cstdint>
#define STACK_SIZE 16

namespace chip8 {
    class Cpu {
        public:
            #ifdef TEST
            friend class Cpu_test;
            #endif

            struct Registers {
                // 一般的な用途のレジスタ
                uint8_t v[16];
                // メモリアドレスを格納するレジスタ
                // NOTE: 通常、下位12bitのみ使われる
                uint16_t i;
                // タイマー
                // 0以外の値の時、60Hzで値がデクリメントされる
                uint8_t delay_timer, sound_timer;
                // program counter
                uint16_t pc;
                // stack pointer
                uint8_t sp;
                // NOTE: STACK_SIZEは、入れ子にできるルーチンの数(深さ)
                uint16_t stack[STACK_SIZE];

                Registers() :
                v(), i(),
                delay_timer(), sound_timer(),
                pc(), sp(UINT8_MAX),
                stack() {}
            };

            class IRandomAccessMemory {
                public:
                    virtual ~IRandomAccessMemory() {}

                    virtual std::vector<unsigned char> load(uint16_t addr, size_t n) = 0;
                    virtual void store(uint16_t addr, const std::vector<unsigned char>& data) = 0;
                    virtual size_t getCapacity() = 0;
            };

            class IDisplay {
                public:
                    virtual ~IDisplay() {}

                    virtual bool renderSprite(size_t x, size_t y, const std::vector<unsigned char>& sprite) = 0;
                    virtual void clear() = 0;
                    virtual void getResolution(size_t& x, size_t& y) = 0;
            };

            enum OpeCode {
                SYS_ADDR            = 0x0000,
                CLS                 = 0x00e0,
                RET                 = 0x00ee,
                JP_ADDR             = 0x1000,
                CALL_ADDR           = 0x2000,
                SE_Vx_BYTE          = 0x3000,
                SNE_Vx_BYTE         = 0x4000,
                SE_Vx_Vy            = 0x5000,
                LD_Vx_BYTE          = 0x6000,
                ADD_Vx_BYTE         = 0x7000,
                LD_Vx_Vy            = 0x8000,
                OR_Vx_Vy            = 0x8001,
                AND_Vx_Vy           = 0x8002,
                XOR_Vx_Vy           = 0x8003,
                ADD_Vx_Vy           = 0x8004,
                SUB_Vx_Vy           = 0x8005,
                SHR_Vx_Vy           = 0x8006,
                SUBN_Vx_Vy          = 0x8007,
                SHL_Vx_Vy           = 0x800e,
                SNE_Vx_Vy           = 0x9000,
                LD_I_ADDR           = 0xa000,
                JP_V0_ADDR          = 0xb000,
                RND_Vx_BYTE         = 0xc000,
                DRW_Vx_Vy_NIBBLE    = 0xd000,
                SKP_Vx              = 0xe09e,
                SKNP_Vx             = 0xe0a1,
                LD_Vx_DT            = 0xf007,
                LD_Vx_K             = 0xf00a,
                LD_DT_Vx            = 0xf015,
                LD_ST_Vx            = 0xf018,
                ADD_I_Vx            = 0xf01e,
                LD_F_Vx             = 0xf029,
                LD_B_Vx             = 0xf033,
                LD_I_Vx             = 0xf055,
                LD_Vx_I             = 0xf065,
            };

            Cpu(IRandomAccessMemory* ram);
            Cpu(IRandomAccessMemory* ram, IDisplay* display);
            void run();
            void consumeClock();
            void setRam(IRandomAccessMemory* ram);
            void setDisplay(IDisplay* display);

            static const size_t kRequireRamSize;
            static const size_t kRequireDisplayWidth, kRequireDisplayHeight;

        private:
            struct OpeInfo {
                OpeCode opecode;
                uint16_t operand;
            };

            typedef void (Cpu::*Operation)(OpeInfo);

            uint16_t fetch();
            OpeInfo decode(uint16_t code);
            void execute(OpeInfo info);

            void sysAddr(OpeInfo info);
            void cls(OpeInfo info);
            void ret(OpeInfo info);
            void jpAddr(OpeInfo info);
            void callAddr(OpeInfo info);
            void seVxByte(OpeInfo info);
            void sneVxByte(OpeInfo info);
            void seVxVy(OpeInfo info);
            void ldVxByte(OpeInfo info);
            void addVxByte(OpeInfo info);
            void ldVxVy(OpeInfo info);
            void orVxVy(OpeInfo info);
            void andVxVy(OpeInfo info);
            void xorVxVy(OpeInfo info);
            void addVxVy(OpeInfo info);
            void subVxVy(OpeInfo info);
            void shrVxVy(OpeInfo info);
            void subnVxVy(OpeInfo info);
            void shlVxVy(OpeInfo info);
            void sneVxVy(OpeInfo info);
            void ldIAddr(OpeInfo info);
            void jpV0Addr(OpeInfo info);
            void rndVxByte(OpeInfo info);
            void drwVxVyNibble(OpeInfo info);
            void skpVx(OpeInfo info);
            void sknpVx(OpeInfo info);
            void ldVxDt(OpeInfo info);
            void ldVxK(OpeInfo info);
            void ldDtVx(OpeInfo info);
            void ldStVx(OpeInfo info);
            void addIVx(OpeInfo info);
            void ldFVx(OpeInfo info);
            void ldBVx(OpeInfo info);
            void ldIVx(OpeInfo info);
            void ldVxI(OpeInfo info);

            static const std::map<OpeCode, Operation> kOperationMap;
            IRandomAccessMemory* ram_;
            IDisplay* display_;
            Registers regs_;
    };
}

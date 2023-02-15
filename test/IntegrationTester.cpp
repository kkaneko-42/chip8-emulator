#include "../src/Cpu.hpp"
#include "../src/TerminalDisplay.hpp"
#include "../src/Ram.hpp"
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <iostream>
#include <unistd.h>

namespace chip8 {
    class IntegrationTester {
        class TestRam : public Cpu::IRandomAccessMemory {
            public:
                TestRam() {
                    data_.resize(4096);
                }

                std::vector<unsigned char> load(uint16_t addr, size_t n) override {
                    std::vector<unsigned char> res = {data_.begin() + addr, data_.begin() + addr + n};
                    return res;
                }

                void store(uint16_t addr, const std::vector<unsigned char>& data) override {
                    for (uint16_t i = 0; i < data.size(); ++i) {
                        data_[i + addr] = data[i];
                    }
                }

                size_t getCapacity() override {
                    return 4096;
                }

                std::vector<unsigned char> data_; 
        };

        public:
            void testAll() {
                testDrwVxVyNibble();
                testCls();
                testRamIntegration();

                std::cout << "OK" << std::endl;
            }

            void testDrwVxVyNibble() {
                TestRam ram;
                TerminalDisplay display;
                Cpu ins(&ram, &display);
                Cpu::OpeInfo info = {Cpu::OpeCode::DRW_Vx_Vy_NIBBLE, 0x0ab5};
                ram.store( 0, {0xf0, 0x90, 0x90, 0x90, 0xf0}); // 0
                ram.store(42, {0xf0, 0x80, 0xf0, 0x80, 0x80}); // f

                ins.regs_.i = 0;
                ins.regs_.v[0xa] = 0;
                ins.regs_.v[0xb] = 0;
                ins.execute(info);
                assert(ins.regs_.v[0xf] == 0);
                sleep(1);

                ins.execute(info);
                assert(ins.regs_.v[0xf] == 1);
                sleep(1);

                ins.regs_.i = 42;
                ins.regs_.v[0xa] = Cpu::kRequireDisplayWidth - CHAR_BIT;
                ins.regs_.v[0xb] = Cpu::kRequireDisplayHeight - 5; // sprite height
                ins.execute(info);
                assert(ins.regs_.v[0xf] == 0);
                sleep(1);
            }
    
            void testCls() {
                TestRam ram;
                TerminalDisplay display;
                Cpu ins(&ram, &display);
                std::vector<unsigned char> fill_sprite(Cpu::kRequireDisplayHeight, 0xff);
                
                for (size_t x = 0; x < Cpu::kRequireDisplayWidth; x += CHAR_BIT) {
                    display.renderSprite(x, 0, fill_sprite);
                }
                sleep(1);

                Cpu::OpeInfo info = {Cpu::OpeCode::CLS, 0x0000};
                ins.execute(info);
                sleep(1);
            }

            void testRamIntegration() {
                testLdIAddr();
                testLdIVx();
                testLdVxI();
                testAddIVx();
            }

            void testLdIAddr() {
                Ram ram;
                Cpu ins(&ram, false);

                Cpu::OpeInfo info = {Cpu::OpeCode::LD_I_ADDR, 0x0fff};
                ins.execute(info);
                assert(ins.regs_.i == 0x0fff);

                info.operand = 0x0000;
                ins.execute(info);
                assert(ins.regs_.i == 0x0000);

                info.operand = 0x0042;
                ins.execute(info);
                assert(ins.regs_.i == 0x0042);
            }

            void testLdIVx() {
                Ram ram;
                Cpu ins(&ram, false);

                for (size_t i = 0; i < 8; ++i) {
                    ins.regs_.v[i] = static_cast<uint8_t>(std::rand());
                }

                Cpu::OpeInfo info = {Cpu::OpeCode::LD_I_Vx, 0x0700};
                ins.execute(info);
                for (size_t i = 0; i < 8; ++i) {
                    assert(ram.data_[i] == ins.regs_.v[i]);
                }

                ins.regs_.i = 42;
                ins.execute(info);
                for (size_t i = 0; i < 8; ++i) {
                    assert(ram.data_[i + ins.regs_.i] == ins.regs_.v[i]);
                }
            }

            void testLdVxI() {
                Ram ram;
                Cpu ins(&ram, false);

                for (size_t i = 0; i < 8; ++i) {
                    ram.data_[i] = static_cast<unsigned char>(std::rand());
                }
                Cpu::OpeInfo info = {Cpu::OpeCode::LD_Vx_I, 0x0700};
                ins.execute(info);
                for (size_t i = 0; i < 8; ++i) {
                    assert(ins.regs_.v[i] == ram.data_[i]);
                }

                for (size_t i = 0; i < 0xa; ++i) {
                    ram.data_[i + 42] = static_cast<unsigned char>(std::rand());
                }
                info.operand = 0x0900;
                ins.regs_.i = 42;
                ins.execute(info);
                for (size_t i = 0; i < 0xa; ++i) {
                    assert(ins.regs_.v[i] == ram.data_[i + 42]);
                }
            }

            void testAddIVx() {
                Ram ram;
                Cpu ins(&ram, false);

                ins.regs_.v[0] = 42;
                Cpu::OpeInfo info = {Cpu::OpeCode::ADD_I_Vx, 0x0000};
                ins.execute(info);
                assert(ins.regs_.i == 42);

                ins.regs_.v[0xa] = 13;
                info.operand = 0x0f00;
                ins.execute(info);
                assert(ins.regs_.i = 42 + 13);
            }
    };
}

int main() {
    chip8::IntegrationTester tester;
    tester.testAll();
    return 0;
}

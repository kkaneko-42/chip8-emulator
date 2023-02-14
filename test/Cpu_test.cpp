#include "../src/Cpu.hpp"
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <iostream>

using namespace chip8;

namespace chip8 {
    class Cpu_test {
        public:
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

            Cpu_test() {
                std::srand(time(nullptr));
            }

            void testAll() {
                testFetch();
                testDecode();
                testRetCall();
                testJp();
                testLdVxDt();
                testLdDtVx();
                testLdStVx();
                testLdIAddr();
                testLdVxVy();
                testLdVxByte();
                testLdIVx();
                testLdVxI();
                testAddIVx();
                testAddVxVy();
                testAddVxByte();
                testOrVxVy();
                testAndVxVy();
                testXorVxVy();
                testSubVxVy();
                testSeVxByte();
                testSneVxByte();
                testSeVxVy();
                testSneVxVy();
                testJpV0Addr();
                testShrVxVy();

                std::cout << "OK" << std::endl;
            }

            void testFetch() {
                TestRam ram;
                ram.store(0, {0xff, 0x66, 0x42, 0x33});
                Cpu ins = Cpu(&ram);

                uint16_t res = ins.fetch();
                assert(res == 0xff66);
                
                res = ins.fetch();
                assert(res == 0x4233);
            }

            void testDecode() {
                TestRam ram;
                Cpu ins = Cpu(&ram);

                auto res = ins.decode(0x00e0);
                assert(res.opecode == Cpu::OpeCode::CLS);

                res = ins.decode(0x1fff);
                assert(res.opecode == Cpu::OpeCode::JP_ADDR && res.operand == 0x0fff);
            
                res = ins.decode(0x82a3);
                assert(res.opecode == Cpu::OpeCode::XOR_Vx_Vy && res.operand == 0x02a0);
            
                res = ins.decode(0x8be4);
                assert(res.opecode == Cpu::OpeCode::ADD_Vx_Vy);
            }

            void testRetCall() {
                TestRam ram;
                Cpu ins = Cpu(&ram);

                Cpu::OpeInfo info = {Cpu::OpeCode::CALL_ADDR, 64};
                ins.execute(info);
                assert(ins.regs_.pc == 64 && ins.regs_.stack[0] == 0 && ins.regs_.sp == 0);

                info = {Cpu::OpeCode::CALL_ADDR, 100};
                ins.execute(info);
                assert(ins.regs_.pc == 100 && ins.regs_.stack[1] == 64 && ins.regs_.sp == 1);

                info = {Cpu::OpeCode::RET};
                ins.execute(info);
                assert(ins.regs_.pc == 64 && ins.regs_.sp == 0);

                ins.execute(info);
                assert(ins.regs_.pc == 0 && ins.regs_.sp == UINT8_MAX);
            }

            void testJp() {
                TestRam ram;
                Cpu ins = Cpu(&ram);

                Cpu::OpeInfo info = {Cpu::OpeCode::JP_ADDR, 100};
                ins.execute(info);
                assert(ins.regs_.pc == 100);

                info.operand = 0x0fff;
                ins.execute(info);
                assert(ins.regs_.pc == 0x0fff);

                info.operand = 0;
                ins.execute(info);
                assert(ins.regs_.pc == 0);
            }

            void testLdVxDt() {
                TestRam ram;
                Cpu ins = Cpu(&ram);

                ins.regs_.delay_timer = 100;
                Cpu::OpeInfo info = {Cpu::OpeCode::LD_Vx_DT, 0x0a00};
                ins.execute(info);
                assert(ins.regs_.v[0xa] == 100);

                ins.regs_.delay_timer = UINT8_MAX;
                info.operand = 0x0000;
                ins.execute(info);
                assert(ins.regs_.v[0x0] == UINT8_MAX);

                ins.regs_.delay_timer = 0;
                info.operand = 0x0f00;
                ins.execute(info);
                assert(ins.regs_.v[0xf] == 0);
            }
    
            void testLdDtVx() {
                TestRam ram;
                Cpu ins = Cpu(&ram);

                ins.regs_.v[0xa] = 100;
                Cpu::OpeInfo info = {Cpu::OpeCode::LD_DT_Vx, 0x0a00};
                ins.execute(info);
                assert(ins.regs_.delay_timer == 100);

                ins.regs_.v[0] = UINT8_MAX;
                info.operand = 0x0000;
                ins.execute(info);
                assert(ins.regs_.delay_timer == UINT8_MAX);

                ins.regs_.v[0xf] = 0;
                info.operand = 0x0f00;
                ins.execute(info);
                assert(ins.regs_.delay_timer == 0);
            }
    
            void testLdStVx() {
                TestRam ram;
                Cpu ins = Cpu(&ram);

                ins.regs_.v[0xa] = 100;
                Cpu::OpeInfo info = {Cpu::OpeCode::LD_ST_Vx, 0x0a00};
                ins.execute(info);
                assert(ins.regs_.sound_timer == 100);

                ins.regs_.v[0] = UINT8_MAX;
                info.operand = 0x0000;
                ins.execute(info);
                assert(ins.regs_.sound_timer == UINT8_MAX);

                ins.regs_.v[0xf] = 0;
                info.operand = 0x0f00;
                ins.execute(info);
                assert(ins.regs_.sound_timer == 0);
            }
    
            void testLdIAddr() {
                TestRam ram;
                Cpu ins = Cpu(&ram);

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
    
            void testLdVxVy() {
                TestRam ram;
                Cpu ins = Cpu(&ram);

                ins.regs_.v[0] = 0;
                ins.regs_.v[0xf] = 42;
                Cpu::OpeInfo info = {Cpu::OpeCode::LD_Vx_Vy, 0x00f0};
                ins.execute(info);
                assert(ins.regs_.v[0] == 42);

                ins.regs_.v[1] = UINT8_MAX;
                info.operand = 0x0010;
                ins.execute(info);
                assert(ins.regs_.v[0] == UINT8_MAX);
            }
    
            void testLdVxByte() {
                TestRam ram;
                Cpu ins = Cpu(&ram);

                Cpu::OpeInfo info = {Cpu::OpeCode::LD_Vx_BYTE, 0x0a42};
                ins.execute(info);
                assert(ins.regs_.v[0xa] == 0x42);

                info.operand = 0x0aff;
                ins.execute(info);
                assert(ins.regs_.v[0xa] == 0xff);

                info.operand = 0x0042;
                ins.execute(info);
                assert(ins.regs_.v[0] == 0x42);
            }
    
            void testLdIVx() {
                TestRam ram;
                Cpu ins = Cpu(&ram);

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
                TestRam ram;
                Cpu ins = Cpu(&ram);

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
                TestRam ram;
                Cpu ins = Cpu(&ram);

                ins.regs_.v[0] = 42;
                Cpu::OpeInfo info = {Cpu::OpeCode::ADD_I_Vx, 0x0000};
                ins.execute(info);
                assert(ins.regs_.i == 42);

                ins.regs_.v[0xa] = 13;
                info.operand = 0x0f00;
                ins.execute(info);
                assert(ins.regs_.i = 42 + 13);
            }
    
            void testAddVxVy() {
                TestRam ram;
                Cpu ins = Cpu(&ram);

                Cpu::OpeInfo info = {Cpu::OpeCode::ADD_Vx_Vy, 0x00a0};
                ins.regs_.v[0] = 2;
                ins.regs_.v[0xa] = UINT8_MAX;
                
                ins.execute(info);
                uint8_t expect = (2 + UINT8_MAX) & 0x00ff;
                assert(ins.regs_.v[0] == expect);
                assert(ins.regs_.v[0xf] == 1);

                ins.regs_.v[1] = 42;
                info.operand = 0x0010;
                ins.execute(info);
                expect += 42;
                assert(ins.regs_.v[0] = expect);
                assert(ins.regs_.v[0xf] == 0);
            }
    
            void testAddVxByte() {
                TestRam ram;
                Cpu ins = Cpu(&ram);

                Cpu::OpeInfo info = {Cpu::OpeCode::ADD_Vx_BYTE, 0x00ff};
                ins.execute(info);
                assert(ins.regs_.v[0] == 0xff);

                info.operand = 0x0a42;
                ins.execute(info);
                assert(ins.regs_.v[0xa] == 0x42);
            }
    
            void testOrVxVy() {
                TestRam ram;
                Cpu ins = Cpu(&ram);
                Cpu::OpeInfo info = {Cpu::OpeCode::OR_Vx_Vy, 0x0010};
                uint8_t a = std::rand(), b = std::rand();

                ins.regs_.v[0] = a;
                ins.regs_.v[1] = b;
                ins.execute(info);
                assert(ins.regs_.v[0] == (a | b));

                a = ins.regs_.v[0];
                ins.regs_.v[1] = b = std::rand();
                ins.execute(info);
                assert(ins.regs_.v[0] == (a | b));
            }
    
            void testAndVxVy() {
                TestRam ram;
                Cpu ins = Cpu(&ram);
                Cpu::OpeInfo info = {Cpu::OpeCode::AND_Vx_Vy, 0x0010};
                uint8_t a = std::rand(), b = std::rand();

                ins.regs_.v[0] = a;
                ins.regs_.v[1] = b;
                ins.execute(info);
                assert(ins.regs_.v[0] == (a & b));

                a = ins.regs_.v[0];
                ins.regs_.v[1] = b = std::rand();
                ins.execute(info);
                assert(ins.regs_.v[0] == (a & b));
            }
    
            void testXorVxVy() {
                TestRam ram;
                Cpu ins = Cpu(&ram);
                Cpu::OpeInfo info = {Cpu::OpeCode::XOR_Vx_Vy, 0x0010};
                uint8_t a = std::rand(), b = std::rand();

                ins.regs_.v[0] = a;
                ins.regs_.v[1] = b;
                ins.execute(info);
                assert(ins.regs_.v[0] == (a ^ b));

                a = ins.regs_.v[0];
                ins.regs_.v[1] = b = std::rand();
                ins.execute(info);
                assert(ins.regs_.v[0] == (a ^ b));
            }
    
            void testSubVxVy() {
                TestRam ram;
                Cpu ins = Cpu(&ram);

                Cpu::OpeInfo info = {Cpu::OpeCode::SUB_Vx_Vy, 0x00a0};
                ins.regs_.v[0] = 42;
                ins.regs_.v[0xa] = UINT8_MAX;
                
                ins.execute(info);
                uint8_t expect = (42 - UINT8_MAX) & 0x00ff;
                assert(ins.regs_.v[0] == expect);
                assert(ins.regs_.v[0xf] == 0);

                ins.regs_.v[1] = 4;
                info.operand = 0x0010;
                ins.execute(info);
                expect -= 4;
                assert(ins.regs_.v[0] = expect);
                assert(ins.regs_.v[0xf] == 1);
            }
    
            void testSeVxByte() {
                TestRam ram;
                Cpu ins = Cpu(&ram);

                // SE Va, 0x42
                ram.data_[0] = 0x3a;
                ram.data_[1] = 0x42;
                // dummy
                ram.data_[4] = 0x42;
                ram.data_[5] = 0x24;
                // set compared value
                ins.regs_.v[0xa] = 0x42;

                // SE Va, 0x42が読まれて、次の命令がスキップされる
                ins.consumeClock();
                // したがって、次のfetchではram[4] ram[5]が取れる
                assert(ins.fetch() == 0x4224);
            }
    
            void testSneVxByte() {
                TestRam ram;
                Cpu ins = Cpu(&ram);

                // SNE Va, 0x42
                ram.data_[0] = 0x4a;
                ram.data_[1] = 0x42;
                // dummy
                ram.data_[4] = 0x42;
                ram.data_[5] = 0x24;
                // set compared value
                ins.regs_.v[0xa] = 0xff;

                // SNE Va, 0x42が読まれて、次の命令がスキップされる
                ins.consumeClock();
                // したがって、次のfetchではram[4] ram[5]が取れる
                assert(ins.fetch() == 0x4224);
            }
    
            void testSeVxVy() {
                TestRam ram;
                Cpu ins = Cpu(&ram);

                // SE Va, Vb
                ram.data_[0] = 0x5a;
                ram.data_[1] = 0xb0;
                // dummy
                ram.data_[4] = 0x42;
                ram.data_[5] = 0x24;
                // set compared value
                ins.regs_.v[0xa] = 0x42;
                ins.regs_.v[0xb] = 0x42;

                // SE Va, Vbが読まれて、次の命令がスキップされる
                ins.consumeClock();
                // したがって、次のfetchではram[4] ram[5]が取れる
                assert(ins.fetch() == 0x4224);
            }
    
            void testSneVxVy() {
                TestRam ram;
                Cpu ins = Cpu(&ram);

                // SNE Va, Vb
                ram.data_[0] = 0x9a;
                ram.data_[1] = 0xb0;
                // dummy
                ram.data_[4] = 0x42;
                ram.data_[5] = 0x24;
                // set compared value
                ins.regs_.v[0xa] = 0x42;
                ins.regs_.v[0xb] = 0xff;

                // SNE Va, Vbが読まれて、次の命令がスキップされる
                ins.consumeClock();
                // したがって、次のfetchではram[4] ram[5]が取れる
                assert(ins.fetch() == 0x4224);
            }
    
            void testJpV0Addr() {
                TestRam ram;
                Cpu ins = Cpu(&ram);
                Cpu::OpeInfo info = {Cpu::OpeCode::JP_V0_ADDR, 0x0042};

                ins.regs_.v[0] = 0x0024;
                ins.execute(info);
                assert(ins.regs_.pc == (0x0024 + 0x0042));
            }
    
            void testShrVxVy() {
                TestRam ram;
                Cpu ins = Cpu(&ram);
                Cpu::OpeInfo info = {Cpu::OpeCode::SHR_Vx_Vy, 0x0a00};

                uint8_t value = 3;
                ins.regs_.v[0xa] = value;
                ins.execute(info);
                assert(ins.regs_.v[0xa] == value / 2);
                assert(ins.regs_.v[0xf] == 1);

                value = 16;
                ins.regs_.v[0xa] = value;
                ins.execute(info);
                assert(ins.regs_.v[0xa] == value / 2);
                assert(ins.regs_.v[0xf] == 0);
            }
    };
}

int main() {
    Cpu_test tester;
    tester.testAll();
    return 0;
}

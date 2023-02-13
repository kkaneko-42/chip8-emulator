#include "../src/Cpu.hpp"
#include <cassert>
#include <iostream>

using namespace chip8;

namespace chip8 {
    class Cpu_test {
        public:
            class TestRam : public Cpu::IRandomAccessMemory {
                public:
                    std::vector<uint16_t> load(uint16_t addr, size_t n) override {
                        std::vector<uint16_t> res = {data_.begin() + addr, data_.end()};
                        return res;
                    }

                    void store(uint16_t addr, const std::vector<uint16_t>& data) override {
                        for (uint16_t i = 0; i < data.size(); ++i) {
                            data_[i + addr] = data[i];
                        }
                    }

                    size_t getCapacity() override {
                        return 4096;
                    }

                    TestRam() {
                        data_.resize(4096);
                    }

                private:
                    std::vector<uint16_t> data_; 
            };

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

                std::cout << "OK" << std::endl;
            }

            void testFetch() {
                TestRam ram;
                ram.store(0, {2, 3, 4, 5, 6});
                Cpu ins = Cpu(&ram);

                uint16_t res = ins.fetch();
                assert(res == 2 && ins.regs_.pc == 1);
                
                res = ins.fetch();
                assert(res == 3 && ins.regs_.pc == 2);
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
    };
}

int main() {
    Cpu_test tester;
    tester.testAll();
    return 0;
}

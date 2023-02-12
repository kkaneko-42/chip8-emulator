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
    };
}

int main() {
    Cpu_test tester;
    tester.testAll();
    return 0;
}

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
    };
}

int main() {
    Cpu_test tester;
    tester.testAll();
    return 0;
}

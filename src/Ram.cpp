#include "Ram.hpp"

using namespace chip8;

const size_t Ram::kCapacity = 4096;

Ram::Ram() {
    data_.resize(kCapacity);
}

std::vector<unsigned char> Ram::load(uint16_t addr, size_t n) {
    return {data_.begin() + addr, data_.begin() + addr + n};
}

void Ram::store(uint16_t addr, const std::vector<unsigned char>& data) {
    for (size_t i = 0; i < data.size(); ++i) {
        data_[i + addr] = data[i];
    }
}

size_t Ram::getCapacity() {
    return data_.size();
}

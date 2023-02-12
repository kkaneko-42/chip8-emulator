#include "Cpu.hpp"
#include <stdexcept>
#include <iostream>

using namespace chip8;

const size_t Cpu::kRequireRamSize = 4096;

Cpu::Cpu(IRandomAccessMemory* ram) :
ram_(ram),
kOperationMap({
    {SYS_ADDR, &Cpu::sysAddr},
    {CLS, &Cpu::cls},
    {RET, &Cpu::ret},
    {JP_ADDR, &Cpu::jpAddr},
    {CALL_ADDR, &Cpu::callAddr},
    {SE_Vx_BYTE, &Cpu::seVxByte},
    {SNE_Vx_BYTE, &Cpu::sneVxByte},
    {SE_Vx_Vy, &Cpu::seVxVy},
    {LD_Vx_BYTE, &Cpu::ldVxByte},
    {ADD_Vx_BYTE, &Cpu::addVxByte},
    {LD_Vx_Vy, &Cpu::addVxVy},
    {OR_Vx_Vy, &Cpu::orVxVy},
    {AND_Vx_Vy, &Cpu::andVxVy},
    {XOR_Vx_Vy, &Cpu::xorVxVy},
    {SUB_Vx_Vy, &Cpu::subVxVy},
    {SHR_Vx_Vy, &Cpu::shrVxVy},
    {SUBN_Vx_Vy, &Cpu::subnVxVy},
    {SHL_Vx_Vy, &Cpu::shlVxVy},
    {SNE_Vx_Vy, &Cpu::sneVxVy},
    {LD_I_ADDR, &Cpu::ldIAddr},
    {JP_V0_ADDR, &Cpu::jpV0Addr},
    {RND_Vx_BYTE, &Cpu::rndVxByte},
    {DRW_Vx_Vy_NIBBLE, &Cpu::drwVxVyNibble},
    {SKP_Vx, &Cpu::skpVx},
    {SKNP_Vx, &Cpu::sknpVx},
    {LD_Vx_DT, &Cpu::ldVxDt},
    {LD_Vx_K, &Cpu::ldVxK},
    {LD_DT_Vx, &Cpu::ldDtVx},
    {LD_ST_Vx, &Cpu::ldStVx},
    {ADD_I_Vx, &Cpu::addIVx},
    {LD_F_Vx, &Cpu::ldFVx},
    {LD_B_Vx, &Cpu::ldBVx},
    {LD_I_Vx, &Cpu::ldIVx},
    {LD_Vx_I, &Cpu::ldVxI}
})
{
    if (ram_->getCapacity() < kRequireRamSize) {
        throw std::runtime_error("RAM size is not enough");
    }
}

void Cpu::run() {
    while (true) {
        consumeClock();
    }
}

void Cpu::consumeClock() {
    uint16_t code = fetch();
    auto ope_info = decode(code);
    execute(ope_info);
}

uint16_t Cpu::fetch() {
    auto data = ram_->load(regs_.pc, 1);
    ++regs_.pc;
    return data[0];
}

Cpu::OpeInfo Cpu::decode(uint16_t code) {
    OpeInfo info;

    for (const auto& kv : kOperationMap) {
        OpeCode reserved_opecode = kv.first;
        if ((code & reserved_opecode) == reserved_opecode) {
            info.opecode = reserved_opecode;
            info.operand = (code & ~reserved_opecode);
            return info;
        }
    }

    throw std::runtime_error("invalid opecode");
}

void Cpu::sysAddr(OpeInfo info) {}
void Cpu::cls(OpeInfo info) {}
void Cpu::ret(OpeInfo info) {}
void Cpu::jpAddr(OpeInfo info) {}
void Cpu::callAddr(OpeInfo info) {}
void Cpu::seVxByte(OpeInfo info) {}
void Cpu::sneVxByte(OpeInfo info) {}
void Cpu::seVxVy(OpeInfo info) {}
void Cpu::ldVxByte(OpeInfo info) {}
void Cpu::addVxByte(OpeInfo info) {}
void Cpu::ldVxVy(OpeInfo info) {}
void Cpu::orVxVy(OpeInfo info) {}
void Cpu::andVxVy(OpeInfo info) {}
void Cpu::xorVxVy(OpeInfo info) {}
void Cpu::addVxVy(OpeInfo info) {}
void Cpu::subVxVy(OpeInfo info) {}
void Cpu::shrVxVy(OpeInfo info) {}
void Cpu::subnVxVy(OpeInfo info) {}
void Cpu::shlVxVy(OpeInfo info) {}
void Cpu::sneVxVy(OpeInfo info) {}
void Cpu::ldIAddr(OpeInfo info) {}
void Cpu::jpV0Addr(OpeInfo info) {}
void Cpu::rndVxByte(OpeInfo info) {}
void Cpu::drwVxVyNibble(OpeInfo info) {}
void Cpu::skpVx(OpeInfo info) {}
void Cpu::sknpVx(OpeInfo info) {}
void Cpu::ldVxDt(OpeInfo info) {}
void Cpu::ldVxK(OpeInfo info) {}
void Cpu::ldDtVx(OpeInfo info) {}
void Cpu::ldStVx(OpeInfo info) {}
void Cpu::addIVx(OpeInfo info) {}
void Cpu::ldFVx(OpeInfo info) {}
void Cpu::ldBVx(OpeInfo info) {}
void Cpu::ldIVx(OpeInfo info) {}
void Cpu::ldVxI(OpeInfo info) {}

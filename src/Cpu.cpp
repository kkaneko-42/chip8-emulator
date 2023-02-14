#include "Cpu.hpp"
#include <stdexcept>
#include <iostream>

using namespace chip8;

const size_t Cpu::kRequireRamSize = 4096;

Cpu::Cpu(IRandomAccessMemory* ram) :
regs_({.sp {UINT8_MAX}}),
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
    {LD_Vx_Vy, &Cpu::ldVxVy},
    {OR_Vx_Vy, &Cpu::orVxVy},
    {AND_Vx_Vy, &Cpu::andVxVy},
    {XOR_Vx_Vy, &Cpu::xorVxVy},
    {ADD_Vx_Vy, &Cpu::addVxVy},
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
    auto data = ram_->load(regs_.pc, 2);
    uint16_t code = (data[0] << 8) | data[1];
    regs_.pc += 2;

    return code;
}

Cpu::OpeInfo Cpu::decode(uint16_t code) {
    OpeInfo info;

    for (auto it = kOperationMap.rbegin(); it != kOperationMap.rend(); ++it) {
        OpeCode reserved_op = it->first;
        if ((code & reserved_op) == reserved_op) {
            info.opecode = reserved_op;
            info.operand = (code ^ reserved_op);
            return info;
        }
    }

    throw std::runtime_error("invalid opecode");
}

void Cpu::execute(OpeInfo info) {
    auto found = kOperationMap.find(info.opecode);
    if (found != kOperationMap.end()) {
        (this->*found->second)(info);
    }
}

void Cpu::sysAddr(OpeInfo info) {}
void Cpu::cls(OpeInfo info) {}

void Cpu::ret(OpeInfo info) {
    // TODO: stack pointer validation
    regs_.pc = regs_.stack[regs_.sp--];
}

void Cpu::jpAddr(OpeInfo info) {
    uint16_t dst_addr = (info.operand & 0x0fff);
    regs_.pc = dst_addr;
}

void Cpu::callAddr(OpeInfo info) {
    // TODO: stack pointer validation
    regs_.stack[++regs_.sp] = regs_.pc;
    regs_.pc = (info.operand & 0x0fff);
}

void Cpu::seVxByte(OpeInfo info) {}
void Cpu::sneVxByte(OpeInfo info) {}
void Cpu::seVxVy(OpeInfo info) {}

void Cpu::ldVxByte(OpeInfo info) {
    uint16_t reg_idx = (info.operand & 0x0f00) >> 8;
    uint8_t imm = (info.operand & 0x00ff);

    regs_.v[reg_idx] = imm;
}

void Cpu::addVxByte(OpeInfo info) {}

void Cpu::ldVxVy(OpeInfo info) {
    uint16_t x_idx = ((info.operand & 0x0f00) >> 8);
    uint16_t y_idx = ((info.operand & 0x00f0) >> 4);

    regs_.v[x_idx] = regs_.v[y_idx];
}

void Cpu::orVxVy(OpeInfo info) {}
void Cpu::andVxVy(OpeInfo info) {}
void Cpu::xorVxVy(OpeInfo info) {}
void Cpu::addVxVy(OpeInfo info) {}
void Cpu::subVxVy(OpeInfo info) {}
void Cpu::shrVxVy(OpeInfo info) {}
void Cpu::subnVxVy(OpeInfo info) {}
void Cpu::shlVxVy(OpeInfo info) {}
void Cpu::sneVxVy(OpeInfo info) {}

void Cpu::ldIAddr(OpeInfo info) {
    uint16_t addr = (info.operand & 0x0fff);
    regs_.i = addr;
}

void Cpu::jpV0Addr(OpeInfo info) {}
void Cpu::rndVxByte(OpeInfo info) {}
void Cpu::drwVxVyNibble(OpeInfo info) {}
void Cpu::skpVx(OpeInfo info) {}
void Cpu::sknpVx(OpeInfo info) {}

void Cpu::ldVxDt(OpeInfo info) {
    uint16_t reg_idx = (info.operand >> 8);
    regs_.v[reg_idx] = regs_.delay_timer;
}

void Cpu::ldVxK(OpeInfo info) {}

void Cpu::ldDtVx(OpeInfo info) {
    uint16_t reg_idx = (info.operand >> 8);
    regs_.delay_timer = regs_.v[reg_idx];
}

void Cpu::ldStVx(OpeInfo info) {
    uint16_t reg_idx = (info.operand >> 8);
    regs_.sound_timer = regs_.v[reg_idx];
}

void Cpu::addIVx(OpeInfo info) {}
void Cpu::ldFVx(OpeInfo info) {}
void Cpu::ldBVx(OpeInfo info) {}

void Cpu::ldIVx(OpeInfo info) {
    size_t reg_end = (info.operand >> 8);

    std::vector<unsigned char> data(reg_end + 1);
    for (size_t i = 0; i <= reg_end; ++i) {
        data[i] = regs_.v[i];
    }

    ram_->store(regs_.i, data);
}

void Cpu::ldVxI(OpeInfo info) {
    size_t reg_end = (info.operand >> 8);
    auto data = ram_->load(regs_.i, reg_end + 1);

    for (size_t i = 0; i <= reg_end; ++i) {
        regs_.v[i] = data[i];
    }
}

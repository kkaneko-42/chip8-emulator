#include "Cpu.hpp"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <random>
#include <thread>
#include <chrono>

using namespace chip8;

const size_t Cpu::kRequireRamSize = 4096;
const size_t Cpu::kRequireDisplayWidth = 64;
const size_t Cpu::kRequireDisplayHeight = 32;
const size_t Cpu::kFontHeight = 5;
const std::map<Cpu::OpeCode, Cpu::Operation> Cpu::kOperationMap = {
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
};

std::string Cpu::OpeInfo::toString() {
    std::stringstream ss;

    ss << std::hex << "{opecode: " << opecode << ", ";
    ss << std::hex << "operand: " << operand << "}";
    return ss.str();
}

#ifdef TEST
// テスト用コンストラクター
Cpu::Cpu(IRandomAccessMemory* ram, bool timerOn) {
    setRam(ram);

    if (timerOn) {
        std::thread timer_decrementer(
            decrementTimer,
            std::ref(regs_.delay_timer), std::ref(regs_.sound_timer),
            std::ref(dt_mtx_), std::ref(st_mtx_)
        );
        timer_decrementer.detach();
    }
}

Cpu::Cpu(IRandomAccessMemory* ram, IDisplay* display) {
    setRam(ram);
    setDisplay(display);
}
// テスト用コンストラクター終わり
#endif

Cpu::Cpu(IRandomAccessMemory* ram, IDisplay* display, IKeyboard* keyboard, ILogger* logger) {
    setRam(ram);
    setDisplay(display);
    setKeyboard(keyboard);
    setLogger(logger);
}

void Cpu::setRam(IRandomAccessMemory* ram) {
    if (ram == nullptr) {
        throw std::runtime_error("RAM is null");
    } else if (ram->getCapacity() < kRequireRamSize) {
        throw std::runtime_error("RAM size is not enough");
    }

    ram_ = ram;
}

void Cpu::setDisplay(IDisplay* display) {
    if (display == nullptr) {
        throw std::runtime_error("Display is null");
    }
    size_t x = 0, y = 0;
    display->getResolution(x, y);
    if (x < kRequireDisplayWidth || y < kRequireDisplayHeight) {
        throw std::runtime_error("Display is not satisfy the requirement");
    }

    display_ = display;
}

void Cpu::setKeyboard(IKeyboard* keyboard) {
    if (keyboard == nullptr) {
        throw std::runtime_error("keyboard is null");
    }

    keyboard_ = keyboard;
}

void Cpu::setLogger(ILogger* logger) {
    if (logger == nullptr) {
        throw std::runtime_error("logger is null");
    }

    logger_ = logger;
}

void Cpu::init() {
    importFontset();

    // start timer thread
    std::thread timer_decrementer(
        decrementTimer,
        std::ref(regs_.delay_timer), std::ref(regs_.sound_timer),
        std::ref(dt_mtx_), std::ref(st_mtx_)
    );
    timer_decrementer.detach();
    regs_.pc += 0x200; // FIXME: magic number

    #ifndef TEST
    logger_->log(DEBUG, "Initialization completed.");
    #endif
}

void Cpu::importFontset() {
    const std::vector<unsigned char> kPresets = {
        0xf0, 0x90, 0x90, 0x90, 0xf0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xf0, 0x10, 0xf0, 0x80, 0xf0, // 2
        0xf0, 0x10, 0xf0, 0x10, 0xf0, // 3
        0x90, 0x90, 0xf0, 0x10, 0x10, // 4
        0xf0, 0x80, 0xf0, 0x10, 0xf0, // 5
        0xf0, 0x80, 0xf0, 0x90, 0xf0, // 6
        0xf0, 0x10, 0x20, 0x40, 0x40, // 7
        0xf0, 0x90, 0xf0, 0x90, 0xf0, // 8
        0xf0, 0x90, 0xf0, 0x10, 0xf0, // 9
        0xf0, 0x90, 0xf0, 0x90, 0x90, // a
        0xe0, 0x90, 0xe0, 0x90, 0xe0, // b
        0xf0, 0x80, 0x80, 0x80, 0xf0, // c
        0xe0, 0x90, 0x90, 0x90, 0xe0, // d
        0xf0, 0x80, 0xf0, 0x80, 0xf0, // e
        0xf0, 0x80, 0xf0, 0x80, 0x80  // f
    };

    size_t ram_addr = 0;
    auto sprite_begin = kPresets.begin();
    while (sprite_begin < kPresets.end()) {
        ram_->store(ram_addr, {sprite_begin, sprite_begin + kFontHeight});
        ram_addr += kFontHeight;
        sprite_begin += kFontHeight;
    }
}

void Cpu::run() {
    while (true) {
        consumeClock();
    }
}

void Cpu::consumeClock() {
    uint16_t code = fetch();
    #ifndef TEST
    std::stringstream ss;
    ss << std::hex << code;
    logger_->log(DEBUG, "fetch code: " + ss.str());
    #endif

    auto ope_info = decode(code);
    #ifndef TEST
    logger_->log(DEBUG, "decode: " + ope_info.toString());
    #endif
    
    execute(ope_info);
    #ifndef TEST
    logger_->log(DEBUG, "execution completed.");
    #endif
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

void Cpu::sysAddr(OpeInfo info) {
    regs_.pc = (info.operand & 0x0fff) + 0x200;
}

void Cpu::cls(OpeInfo info) {
    display_->clear();
}

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

void Cpu::seVxByte(OpeInfo info) {
    uint16_t reg_idx = (info.operand >> 8);
    uint16_t imm = (info.operand & 0x00ff);

    if (regs_.v[reg_idx] == imm) {
        regs_.pc += 2;
    }
}

void Cpu::sneVxByte(OpeInfo info) {
    uint16_t reg_idx = (info.operand >> 8);
    uint16_t imm = (info.operand & 0x00ff);

    if (regs_.v[reg_idx] != imm) {
        regs_.pc += 2;
    }
}

void Cpu::seVxVy(OpeInfo info) {
    uint16_t x_idx = (info.operand >> 8);
    uint16_t y_idx = (info.operand >> 4) & 0x000f;

    if (regs_.v[x_idx] == regs_.v[y_idx]) {
        regs_.pc += 2;
    }
}

void Cpu::ldVxByte(OpeInfo info) {
    uint16_t reg_idx = (info.operand & 0x0f00) >> 8;
    uint8_t imm = (info.operand & 0x00ff);

    regs_.v[reg_idx] = imm;
}

void Cpu::addVxByte(OpeInfo info) {
    uint16_t reg_idx = (info.operand >> 8);
    uint16_t imm = (info.operand & 0x00ff);

    regs_.v[reg_idx] += static_cast<uint8_t>(imm);
}

void Cpu::ldVxVy(OpeInfo info) {
    uint16_t x_idx = ((info.operand & 0x0f00) >> 8);
    uint16_t y_idx = ((info.operand & 0x00f0) >> 4);

    regs_.v[x_idx] = regs_.v[y_idx];
}

void Cpu::orVxVy(OpeInfo info) {
    uint16_t x_idx = (info.operand >> 8);
    uint16_t y_idx = (info.operand >> 4) & 0x000f;

    regs_.v[x_idx] |= regs_.v[y_idx];
}

void Cpu::andVxVy(OpeInfo info) {
    uint16_t x_idx = (info.operand >> 8);
    uint16_t y_idx = (info.operand >> 4) & 0x000f;

    regs_.v[x_idx] &= regs_.v[y_idx];
}

void Cpu::xorVxVy(OpeInfo info) {
    uint16_t x_idx = (info.operand >> 8);
    uint16_t y_idx = (info.operand >> 4) & 0x000f;

    regs_.v[x_idx] ^= regs_.v[y_idx];
}

void Cpu::addVxVy(OpeInfo info) {
    uint16_t x_idx = (info.operand >> 8);
    uint16_t y_idx = (info.operand >> 4) & 0x000f;

    uint16_t res = regs_.v[x_idx] + regs_.v[y_idx];
    regs_.v[x_idx] = (res & 0x00ff);
    regs_.v[0xf] = (res > UINT8_MAX) ? 1 : 0;
}

void Cpu::subVxVy(OpeInfo info) {
    uint16_t x_idx = (info.operand >> 8);
    uint16_t y_idx = (info.operand >> 4) & 0x000f;

    regs_.v[0xf] = (regs_.v[x_idx] > regs_.v[y_idx]) ? 1 : 0;
    regs_.v[x_idx] -= regs_.v[y_idx];
}

void Cpu::shrVxVy(OpeInfo info) {
    uint16_t x_idx = (info.operand >> 8);
    
    regs_.v[0xf] = ((regs_.v[x_idx] & 1) == 1) ? 1 : 0;
    regs_.v[x_idx] >>= 1;
}

void Cpu::subnVxVy(OpeInfo info) {
    uint16_t x_idx = (info.operand >> 8);
    uint16_t y_idx = (info.operand >> 4) & 0x000f;

    regs_.v[0xf] = (regs_.v[x_idx] < regs_.v[y_idx]) ? 1 : 0;
    regs_.v[x_idx] = regs_.v[y_idx] - regs_.v[x_idx];
}

void Cpu::shlVxVy(OpeInfo info) {
    uint16_t x_idx = (info.operand >> 8);
    
    regs_.v[0xf] = ((regs_.v[x_idx] >> 7) == 1) ? 1 : 0;
    regs_.v[x_idx] <<= 1;
}

void Cpu::sneVxVy(OpeInfo info) {
    uint16_t x_idx = (info.operand >> 8);
    uint16_t y_idx = (info.operand >> 4) & 0x000f;

    if (regs_.v[x_idx] != regs_.v[y_idx]) {
        regs_.pc += 2;
    }
}

void Cpu::ldIAddr(OpeInfo info) {
    uint16_t addr = (info.operand & 0x0fff);
    regs_.i = addr;
}

void Cpu::jpV0Addr(OpeInfo info) {
    uint16_t addr = (info.operand & 0x0fff);
    regs_.pc = regs_.v[0] + addr;
}

void Cpu::rndVxByte(OpeInfo info) {
    uint16_t reg_idx = (info.operand >> 8);
    uint16_t imm = (info.operand & 0x00ff);
    std::random_device rnd;

    regs_.v[reg_idx] = (rnd() & imm);
}

void Cpu::drwVxVyNibble(OpeInfo info) {
    uint16_t x_idx  = (info.operand >> 8);
    uint16_t y_idx  = (info.operand >> 4) & 0x000f;
    uint16_t n_byte = (info.operand & 0x000f);

    auto sprite = ram_->load(regs_.i, n_byte);
    bool collision = display_->renderSprite(
        regs_.v[x_idx], regs_.v[y_idx],
        sprite
    );
    regs_.v[0xf] = (collision) ? 1 : 0;
}

void Cpu::skpVx(OpeInfo info) {
    KeyCode keycode = static_cast<KeyCode>(info.operand >> 8);
    if (keyboard_->isPressing(keycode)) {
        regs_.pc += 2;
    }
}

void Cpu::sknpVx(OpeInfo info) {
    KeyCode keycode = static_cast<KeyCode>(info.operand >> 8);
    if (!keyboard_->isPressing(keycode)) {
        regs_.pc += 2;
    }
}

void Cpu::ldVxDt(OpeInfo info) {
    uint16_t reg_idx = (info.operand >> 8);
    regs_.v[reg_idx] = regs_.delay_timer;
}

void Cpu::ldVxK(OpeInfo info) {
    uint16_t reg_idx = (info.operand >> 8);
    KeyCode keycode = KEY_INVALID;

    while (keycode == KEY_INVALID) {
        keycode = keyboard_->acquireKey();
    }
    regs_.v[reg_idx] = static_cast<uint8_t>(keycode);
}

void Cpu::ldDtVx(OpeInfo info) {
    uint16_t reg_idx = (info.operand >> 8);
    setDelayTimer(regs_.v[reg_idx]);
}

void Cpu::ldStVx(OpeInfo info) {
    uint16_t reg_idx = (info.operand >> 8);
    regs_.sound_timer = regs_.v[reg_idx];
}

void Cpu::addIVx(OpeInfo info) {
    uint16_t reg_idx = (info.operand >> 8);
    regs_.i = regs_.v[reg_idx];
}

void Cpu::ldFVx(OpeInfo info) {
    uint16_t reg_idx = (info.operand >> 8);
    uint8_t sprite_num = regs_.v[reg_idx];
    std::vector<unsigned char> font = ram_->load(sprite_num * kFontHeight, kFontHeight);
    ram_->store(regs_.i, font);
}

void Cpu::ldBVx(OpeInfo info) {
    unsigned char value = regs_.v[info.operand >> 8];
    unsigned char hundreds = value / 100;
    unsigned char tens = (value / 10) % 10;
    unsigned char ones = value % 10;
    std::vector<unsigned char> data = {hundreds, tens, ones};

    ram_->store(regs_.i, data);
}

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

void Cpu::setDelayTimer(uint8_t value) {
    std::lock_guard<std::mutex> lk(dt_mtx_);
    regs_.delay_timer = value;
}

void Cpu::setSoundTimer(uint8_t value) {
    std::lock_guard<std::mutex> lk(st_mtx_);
    regs_.sound_timer = value;
}

uint8_t Cpu::getDelayTimer() {
    std::lock_guard<std::mutex> lk(dt_mtx_);
    return regs_.delay_timer;
}

uint8_t Cpu::getSoundTimer() {
    std::lock_guard<std::mutex> lk(st_mtx_);
    return regs_.sound_timer;
}

void Cpu::decrementTimer(
    uint8_t& delay_timer,
    uint8_t& sound_timer,
    std::mutex& dt_mtx,
    std::mutex& st_mtx)
{
    while (true) {
        // 1秒間待機
        // NOTE: ホスト側の初期化とデータ競合しないため、まず待機
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        {
            // delay timerのdecrement
            std::lock_guard<std::mutex> lk(dt_mtx);
            if (delay_timer != 0) {
                --delay_timer;
            }
        }
        {
            // sound timerのdecrement
            std::lock_guard<std::mutex> lk(st_mtx);
            if (sound_timer != 0) {
                --sound_timer;
            }
        }
    }
}

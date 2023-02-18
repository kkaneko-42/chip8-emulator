#include "Logger.hpp"

using namespace chip8;

const std::string Logger::kLogfilePath = "/var/log/chip8.log";

Logger::Logger() {
    logfile_.open(kLogfilePath, std::ios::out);
    if (!logfile_) {
        throw std::runtime_error("failed to open logfile");
    }
}

void Logger::log(Cpu::LogLevel level, const std::string& msg) {

}

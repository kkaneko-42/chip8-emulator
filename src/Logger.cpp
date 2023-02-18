#include "Logger.hpp"

using namespace chip8;

const std::string Logger::kLogfilePath = "./chip8.log";

Logger::Logger() {
    logfile_.open(kLogfilePath, std::ios::app);
    if (!logfile_) {
        throw std::runtime_error("failed to open logfile");
    }
}

void Logger::log(Cpu::LogLevel level, const std::string& msg) {
    logfile_ << msg << "\n";
}

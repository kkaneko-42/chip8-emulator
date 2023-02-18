#pragma once

#include "Cpu.hpp"
#include <fstream>

namespace chip8 {
    class Logger : public Cpu::ILogger {
        public:
            Logger();
            void log(Cpu::LogLevel level, const std::string& msg) override;

        private:
            static const std::string kLogfilePath;

            std::ofstream logfile_;
    };
}

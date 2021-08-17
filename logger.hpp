#pragma once
#include <string>
namespace logger {
    enum class LOG_LEVEL {
        NONE = 0b10000,
        ERROR = 0b01000,
        WARNING = 0b00100,
        INFO = 0b00010,
        DEBUG = 0b00001,
    };
    enum class SETTINGS {
        NEWLINE = 0b0001,
        TYPE = 0b0010,
        NONE = 0b0,
    };
    LOG_LEVEL operator|(LOG_LEVEL lhs, LOG_LEVEL rhs);
    LOG_LEVEL operator&(LOG_LEVEL lhs, LOG_LEVEL rhs);
    SETTINGS operator|(SETTINGS lhs, SETTINGS rhs);
    SETTINGS operator&(SETTINGS lhs, SETTINGS rhs);
    class logger {
        public:
        LOG_LEVEL level;
        std::string log_level_text(LOG_LEVEL lvl);
        void log(LOG_LEVEL level, std::string msg, SETTINGS settings = SETTINGS::NEWLINE | SETTINGS::TYPE);
        logger(LOG_LEVEL l);
    };
};  // namespace logger
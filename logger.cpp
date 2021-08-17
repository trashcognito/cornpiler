#include "logger.hpp"
#include <iostream>
namespace logger {
    LOG_LEVEL operator|(LOG_LEVEL lhs, LOG_LEVEL rhs){
        return (LOG_LEVEL)((int)lhs | (int)rhs);
    }
    LOG_LEVEL operator&(LOG_LEVEL lhs, LOG_LEVEL rhs){
        return (LOG_LEVEL)((int)lhs & (int)rhs);
    }
    SETTINGS operator|(SETTINGS lhs, SETTINGS rhs){
        return (SETTINGS)((int)lhs | (int)rhs);
    }
    SETTINGS operator&(SETTINGS lhs, SETTINGS rhs){
        return (SETTINGS)((int)lhs & (int)rhs);
    }
    std::string logger::log_level_text(LOG_LEVEL lvl) {
        std::string retval = "";
        if ((int)lvl & (int)LOG_LEVEL::NONE) {
            retval += "| NONE ";
        }
        if ((int)lvl & (int)LOG_LEVEL::ERROR) {
            retval += "| ERROR ";
        }
        if ((int)lvl & (int)LOG_LEVEL::WARNING) {
            retval += "| WARNING ";
        }
        if ((int)lvl & (int)LOG_LEVEL::INFO) {
            retval += "| INFO ";
        }
        if ((int)lvl & (int)LOG_LEVEL::DEBUG) {
            retval += "| DEBUG ";
        }
        retval.erase(0, 1);
        return retval;
    }
    void logger::log(LOG_LEVEL level, std::string msg, SETTINGS settings) {
        if ((int)(level & logger::level) == 0) return;
        if((int)(settings & SETTINGS::TYPE) != 0) std::cout << "[" << log_level_text(level) << "] ";
        std::cout << msg;
        if((int)(settings & SETTINGS::NEWLINE) != 0) std::cout << std::endl;
    }
    logger::logger(LOG_LEVEL l) {
        level = l;
    }
};  // namespace logger
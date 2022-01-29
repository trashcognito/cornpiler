#include <string>
#include <iostream>
#include "logger.hpp"

logger::LOG_LEVEL logger::operator|(LOG_LEVEL lhs, LOG_LEVEL rhs) {
  return (LOG_LEVEL)((int)lhs | (int)rhs);
}
logger::LOG_LEVEL logger::operator&(LOG_LEVEL lhs, LOG_LEVEL rhs) {
  return (LOG_LEVEL)((int)lhs & (int)rhs);
}
logger::SETTINGS logger::operator|(SETTINGS lhs, SETTINGS rhs) {
  return (SETTINGS)((int)lhs | (int)rhs);
}
logger::SETTINGS logger::operator&(SETTINGS lhs, SETTINGS rhs) {
  return (SETTINGS)((int)lhs & (int)rhs);
}
std::string logger::logger::log_level_text(LOG_LEVEL lvl) {
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
void logger::logger::log(LOG_LEVEL level, std::string msg, SETTINGS settings) {
  if ((int)(level & logger::level) == 0)
    return;
  if ((int)(settings & SETTINGS::TYPE) != 0)
    std::cout << "[" << log_level_text(level) << "] " << std::flush;
  std::cout << msg << std::flush;
  if ((int)(settings & SETTINGS::NEWLINE) != 0)
    std::cout << std::endl;
}

logger::logger::logger(LOG_LEVEL log) { level = log; }
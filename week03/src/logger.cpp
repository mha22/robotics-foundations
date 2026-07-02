#include "logger.hpp"

#include <iostream>

namespace robot_sim {

void Logger::info(const std::string& msg) {
    std::cout << "\033[32m[INFO]\033[0m " << 
    msg << std::endl;
}

void Logger::warn(const std::string& msg) {
    std::cout << "\033[33m[WARN]\033[0m " <<
    msg << std::endl;
}

void Logger::error(const std::string& msg) {
    std::cerr << "\033[31m[ERROR]\033[0m " << msg << std::endl;
}

}

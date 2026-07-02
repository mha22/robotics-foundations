#pragma once
#include <string>

namespace robot_sim {

class Logger {
public:
    static void info(const std::string& msg);
    static void warn(const std::string& msg);
    static void error(const std::string& msg);
};

}

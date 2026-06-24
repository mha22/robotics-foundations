#include "robot.hpp"

#include <iostream>
#include <string>



int main(int argc, char* argv[]) {
    SimulationConfig config{0.5, 0.2, 0.1, 100};

    if (argc == 5) {
        config.v = std::stod(argv[1]);
        config.w = std::stod(argv[2]);
        config.dt = std::stod(argv[3]);
        config.steps = std::stoi(argv[4]);
    }
    else if (argc != 1) {
        std::cerr << "Usage: " << argv[0] << " <v> <w> <dt> <steps>" << std::endl;
        return 1;
    }

    MobileRobot robot(0.0, 0.0, 0.0);
    runSimulation(robot, config);

    return 0;
}

#include "mobile_robot.hpp"

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    double v = 0.5;
    double w = 0.2;
    double dt = 0.1;
    int steps = 100;

    if (argc == 5) {
        v = std::stod(argv[1]);
        w = std::stod(argv[2]);
        dt = std::stod(argv[3]);
        steps = std::stoi(argv[4]);
    }
    else if (argc != 1) {
        std::cerr << "Usage: " << argv[0] << " <v> <w> <dt> <steps>" << std::endl;
        return 1;
    }

    MobileRobot robot(0.0, 0.0, 0.0);

    for (int i = 0; i < steps; i++) {
        robot.move(v, w, dt);
    }

    std::cout << "Final ";
    robot.printPose();
    std::cout << "Total distance traveled: " <<
    robot.compute_total_distance() <<
    " m" << std::endl;

    robot.saveToCsv();
    
    return 0;
}

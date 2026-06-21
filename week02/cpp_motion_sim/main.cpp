#include "mobile_robot.hpp"

#include <iostream>

int main() {
    double v = 0.5;
    double w = 0.2;
    double dt = 0.1;
    int steps = 100;
    MobileRobot robot(0.0, 0.0, 0.0);


    for (int i = 0; i < steps; i++) {
        robot.move(v, w, dt);
    }

    std::cout << "Final ";
    robot.printPose();
    
    return 0;
}


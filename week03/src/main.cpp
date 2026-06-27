#include "robot.hpp"

#include <iostream>
#include <string>


int main(int argc, char* argv[]) {

    if (argc == 1) {
        std::cout << "===== Test 1: omega_left = omega_right (straight) ====="  << std::endl;
        MobileRobot robot1(0.0, 0.0, 0.0);
        runWheelSimulation(robot1, {{10.0, 10.0}, 0.1, 100}, "path_wheel_straight.csv");

        std::cout << "\n==== Test 2: omega_left = -omega_right (rotate in place) ====="  << std::endl;
        MobileRobot robot2(0.0, 0.0, 0.0);
        runWheelSimulation(robot2, {{-5.0, 5.0}, 0.1, 100}, "path_wheel_rotate.csv");     

        std::cout << "\n===== Test 3: omega_right > omega_left (arc) ====="  << std::endl;
        MobileRobot robot3 (0.0, 0.0, 0.0);
        runWheelSimulation(robot3, {{5.0, 10.0}, 0.1, 100}, "path_wheel_arc.csv");
    }
    else if (argc == 5) {
        SimulationConfig config{
        std::stod(argv[1]),
        std::stod(argv[2]),
        std::stod(argv[3]),
        std::stoi(argv[4])
        };
        MobileRobot robot(0.0, 0.0, 0.0);
        runSimulation(robot, config);

    }
    else if (argc == 7) {
        WheelSimulationConfig config{
        {std::stod(argv[1]), std::stod(argv[2])},
        std::stod(argv[3]),
        std::stoi(argv[4])
        };

        RobotGeometry geometry{
            std::stod(argv[5]),    
            std::stod(argv[6])
        };

        MobileRobot robot(0.0, 0.0, 0.0, geometry);
        runWheelSimulation(robot, config, "path_wheel.csv");
    }

    else {
        std::cerr << "Usage:" << std::endl;
        std::cerr << "  " << argv[0] << " (run default tests)" << std::endl;
        std::cerr << "  " << argv[0] << " <v> <w> <dt> <steps> (unicycle mode)" << std::endl;
        std::cerr << "  " << argv[0] << " <omega_l> <omega_r> <dt> <steps> <wheel_r> <wheel_base> (differential drive)" << std::endl;
        return 1;
    }

    return 0;
}

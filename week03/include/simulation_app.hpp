#pragma once

#include "robot.hpp"

#include <memory>
#include <string>
#include <vector>

namespace robot_sim {

class SimulationApp {
public:
    SimulationApp();
    void setup();
    void setup(int argc, char* argv[]);
    void execute();

private:
    enum class Mode {
        DefaultTests,
        Unicycle,
        DifferentialDrive
    };

    void run_default_tests();
    void run_unicycle_simulation();
    void run_differential_drive_simulation();
    void print_usage(const std::string& program_name) const;
    void print_simulation_summary() const;

private:
    std::shared_ptr<MobileRobot> robot_;
    Mode mode_;

    SimulationConfig sim_config_;
    WheelSimulationConfig wheel_config_;
    RobotGeometry geometry_;

    std::string csv_filename_;
    std::string program_name_;
};

}

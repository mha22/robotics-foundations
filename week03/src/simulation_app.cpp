#include "simulation_app.hpp"
#include "logger.hpp"
#include "path_analyzer.hpp"

#include <sstream>
#include <stdexcept>
#include <string>

namespace robot_sim {
SimulationApp::SimulationApp()
    : robot_(nullptr),
    mode_(Mode::DefaultTests),
    sim_config_{0.0, 0.0, 0.0, 0},
    wheel_config_{{0.0, 0.0}, 0.0, 0},
    geometry_{0.05, 0.3},
    csv_filename_("path.csv"),
    program_name_("motion_sim") 
{
}

void SimulationApp::setup() {
    mode_ = Mode::DefaultTests;
}

void SimulationApp::setup(int argc, char* argv[]) {
    if (argc > 0) {
        program_name_ = argv[0];
    }

    if (argc == 1) {
        mode_ = Mode::DefaultTests;
    }
    else if (argc == 5) {
        mode_ = Mode::Unicycle;
        sim_config_ = {
        std::stod(argv[1]),
        std::stod(argv[2]),
        std::stod(argv[3]),
        std::stoi(argv[4])
        };

        csv_filename_ = "path.csv";
    }
    else if (argc == 7) {
        mode_ = Mode::DifferentialDrive;
        wheel_config_ = {
        {std::stod(argv[1]), std::stod(argv[2])},
        std::stod(argv[3]),
        std::stoi(argv[4])
        };

        geometry_ = {
            std::stod(argv[5]),    
            std::stod(argv[6])
        };

        csv_filename_ = "path_wheel.csv";
    }
    else {
        Logger::error("Invalid arguments");
        print_usage(program_name_);
        throw std::runtime_error("invalid arguments");
    }
}

void SimulationApp::execute() {
    switch (mode_)
    {
    case Mode::DefaultTests:
        run_default_tests();
        break;
    case Mode::Unicycle:
        run_unicycle_simulation();
        break;
    case Mode::DifferentialDrive:
        run_differential_drive_simulation();
        break;
    }
}

void SimulationApp::run_default_tests() {
        Logger::info("===== Test 1: omega_left = omega_right (straight) =====");
        robot_ = std::make_shared<MobileRobot>(0.0, 0.0, 0.0);
        wheel_config_ = {{10.0, 10.0}, 0.1, 100};
        csv_filename_ = "path_wheel_straight.csv";
        run_differential_drive_simulation();

        Logger::info("\n==== Test 2: omega_left = -omega_right (rotate in place) =====");
        robot_ = std::make_shared<MobileRobot>(0.0, 0.0, 0.0);
        wheel_config_ = {{-5.0, 5.0}, 0.1, 100};
        csv_filename_ = "path_wheel_rotate.csv";
        run_differential_drive_simulation();



        Logger::info("\n===== Test 3: omega_right > omega_left (arc) =====");
        robot_ = std::make_shared<MobileRobot>(0.0, 0.0, 0.0);
        wheel_config_ = {{5.0, 10.0}, 0.1, 100};
        csv_filename_ = "path_wheel_arc.csv";
        run_differential_drive_simulation();
}

void SimulationApp::run_unicycle_simulation() {
    if (!robot_) {
        robot_ = std::make_shared<MobileRobot>(0.0, 0.0, 0.0);
    }

    for (int i = 0; i < sim_config_.steps; i++) {
        robot_->move(sim_config_.v, sim_config_.w, sim_config_.dt);
    }

    print_simulation_summary();
    robot_->save_to_csv(csv_filename_);
}

void SimulationApp::run_differential_drive_simulation() {
    if (!robot_) {
        robot_ = std::make_shared<MobileRobot>(0.0, 0.0, 0.0, geometry_);
    }

    for (int i = 0; i < wheel_config_.steps; i++) {
        robot_->move_from_wheel_speeds(wheel_config_.command, wheel_config_.dt);
    }

    print_simulation_summary();
    robot_->save_to_csv(csv_filename_);
}

void SimulationApp::print_usage(const std::string& program_name) const {
    Logger::info("Usage:");
    Logger::info("  " + program_name + " (run default tests)");
    Logger::info("  " + program_name + " <v> <w> <dt> <steps> (unicycle mode)");
    Logger::info("  " + program_name + " <omega_l> <omega_r> <dt> <steps> <wheel_r> <wheel_base> (differential drive)");
}

void SimulationApp::print_simulation_summary() const {
    const auto& path = robot_->get_path();

    std::ostringstream oss;
    oss << "Final " << robot_->get_pose_string() << "\n"
        << "Total distance traveled: " 
        << PathAnalyzer::compute_total_distance(path)<< " m\n"

        << "Net displacement: " 
        << PathAnalyzer::compute_net_displacement(path) << " m\n"

        << "Final heading in degrees: " 
        << PathAnalyzer::compute_final_heading_deg(path) << " deg\n"

        << "Maximum distance from origin: " 
        << PathAnalyzer::compute_max_distance_from_origin(path) << " m";

    Logger::info(oss.str());
}
    
}

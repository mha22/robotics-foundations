#include "simulation_app.hpp"
#include "logger.hpp"
#include "path_analyzer.hpp"

#include <sstream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

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
    else if (argc == 2) {
        load_config_file(argv[1]);
    }
    else if (argc == 3 && std::string(argv[1]) == "--config") {
        load_config_file(argv[2]);
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
    Logger::info("  " + program_name + " config.txt");
    Logger::info("  " + program_name + " --config config.txt");
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

void SimulationApp::load_config_file(const std::string& filename) {
    auto config = parse_config_file(filename);

    std::string mode = get_required(config, "mode");

    if (mode == "unicycle") {
        mode_ = Mode::Unicycle;

        sim_config_ = {
            std::stod(get_required(config, "v")),
            std::stod(get_required(config, "w")),
            std::stod(get_required(config, "dt")),
            std::stoi(get_required(config, "steps"))
        };

        csv_filename_ = get_optional(config, "csv", "path.csv");

        Logger::info("Loaded unicycle config from: " + filename);
    }
    else if (mode == "differential") {
        mode_ = Mode::DifferentialDrive;

        wheel_config_ = {
            {
                std::stod(get_required(config, "omega_left")),
                std::stod(get_required(config, "omega_right"))
            },
            std::stod(get_required(config, "dt")),
            std::stoi(get_required(config, "steps"))
        };

        geometry_ = {
            std::stod(get_optional(config, "wheel_radius", "0.05")),
            std::stod(get_optional(config, "wheel_base", "0.3"))
        };

        csv_filename_ = get_optional(config, "csv", "path_wheel.csv");

        Logger::info("Loaded differential drive config from: " + filename);
    }
    else {
        throw std::runtime_error("Unknown simulation mode in config file: " + mode);
    }
}

std::unordered_map<std::string, std::string>
SimulationApp::parse_config_file(const std::string& filename) const {
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open config file: " + filename);
    }

    std::unordered_map<std::string, std::string> config;

    std::string line;
    int line_number = 0;

    while (std::getline(file, line)) {
        line_number++;

        std::string trimmed_line = trim(line);

        if (trimmed_line.empty()) {
            continue;
        }

        if (trimmed_line[0] == '#') {
            continue;
        }

        std::size_t equal_pos = trimmed_line.find('=');

        if (equal_pos == std::string::npos) {
            throw std::runtime_error(
                "Invalid config line " + std::to_string(line_number) +
                ": expected key=value"
            );
        }

        std::string key = trim(trimmed_line.substr(0, equal_pos));
        std::string value = trim(trimmed_line.substr(equal_pos + 1));

        if (key.empty()) {
            throw std::runtime_error(
                "Invalid config line " + std::to_string(line_number) +
                ": empty key"
            );
        }

        config[key] = value;
    }

    return config;
}

std::string SimulationApp::get_required(
    const std::unordered_map<std::string, std::string>& config,
    const std::string& key
) const {
    auto it = config.find(key);
    
    if (it == config.end()) {
        throw std::runtime_error("Missing required config key: " + key);
    }

    return it->second;
}

std::string SimulationApp::get_optional(
    const std::unordered_map<std::string, std::string>& config,
    const std::string& key,
    const std::string& default_value
) const {
    auto it = config.find(key);

    if (it == config.end()) {
        return default_value;
    }

    return it->second;
}

std::string SimulationApp::trim(const std::string& text) const {
    const std::string whitespace = " \t\r\n";

    const auto start = text.find_first_not_of(whitespace);
    if (start == std::string::npos) {
        return "";
    }

    const auto end = text.find_last_not_of(whitespace);
    return text.substr(start, end - start + 1);
}

}

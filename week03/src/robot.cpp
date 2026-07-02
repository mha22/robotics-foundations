#include "logger.hpp"
#include "robot.hpp"
#include "path_analyzer.hpp"

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>

namespace robot_sim {

MobileRobot::MobileRobot(double x, double y, double theta) 
    : x_(x),
    y_(y),
    theta_(theta),
    geometry_{0.05, 0.3}
{
    path_.push_back({x_, y_, theta_});
}

MobileRobot::MobileRobot(double x, double y, double theta, const RobotGeometry& geometry) 
    : x_(x),
    y_(y),
    theta_(theta),
    geometry_(geometry)
{    
    if (geometry_.wheel_base <= 1e-9) {
        throw std::invalid_argument("wheel_base must be positive");
    }
    path_.push_back({x_, y_, theta_});
}

void MobileRobot::move(double v, double w, double dt) {
    x_ = x_ + v * std::cos(theta_) * dt;
    y_ = y_ + v * std::sin(theta_) * dt;
    theta_ = theta_ + w * dt;

    path_.push_back({x_, y_, theta_});
}

void MobileRobot::move_from_wheel_speeds(const WheelCommand& cmd, double dt) {
    double v = geometry_.wheel_radius * (cmd.omega_right + cmd.omega_left) / 2.0;
    double w = geometry_.wheel_radius * (cmd.omega_right - cmd.omega_left) / geometry_.wheel_base;

    move(v, w, dt);
}

std::string MobileRobot::get_pose_string() const {
    constexpr double rad_to_deg = 180.0 / 3.14159265358979323846;
    std::ostringstream oss;
    oss << "pose: x=" << x_
          << ", y=" << y_
          << ", theta=" << theta_
          << " rad"
          << " (" << theta_ * rad_to_deg << " deg)";
        
    return oss.str();
}

Pose MobileRobot::get_pose() const {
    return {x_, y_, theta_};
}

const std::vector<Pose>& MobileRobot::get_path() const {
    return path_;
}

void MobileRobot::save_to_csv(const std::string& filename) const {
    std::ofstream file(filename);

    if (!file.is_open()) {

        Logger::error("Could not open file " + filename + " for writing");
        return;
    }

    file << "step,x,y,theta\n";

    for (size_t i=0; i < path_.size(); i++) {
        file << i << ","
        << path_[i].x << ","
        << path_[i].y << ","
        << path_[i].theta << "\n";
    }

    file.close();
}


void runSimulation(MobileRobot& robot, const SimulationConfig& config) {
    for (int i = 0; i < config.steps; i++) {
        robot.move(config.v, config.w, config.dt);
    }

    print_simulation_summary(robot);
    robot.save_to_csv("path.csv");
}


void runWheelSimulation(MobileRobot& robot, const WheelSimulationConfig& config, const std::string& filename) {
    for (int i = 0; i < config.steps; i++) {
        robot.move_from_wheel_speeds(config.command, config.dt);
    }

    print_simulation_summary(robot);
    robot.save_to_csv(filename);
}


void print_simulation_summary(const MobileRobot& robot) {
    const auto& path = robot.get_path();



    std::ostringstream oss;
    oss << "Final " << robot.get_pose_string() << "\n"
    << "Total distance traveled: " <<
    PathAnalyzer::compute_total_distance(path)
    << " m\n"

    << "Net displacement: " <<
    PathAnalyzer::compute_net_displacement(path) << 
    " m\n"

    << "Final heading in degrees: " <<
    PathAnalyzer::compute_final_heading_deg(path) << 
    " deg\n"

    << "Maximum distance from origin: " <<
    PathAnalyzer::compute_max_distance_from_origin(path) <<
    " m";

    Logger::info(oss.str());
}

}

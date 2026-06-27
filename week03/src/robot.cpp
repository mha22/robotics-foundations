#include "robot.hpp"
#include "path_analyzer.hpp"

#include <cmath>
#include <iostream>
#include <fstream>

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

void MobileRobot::print_pose() const {
    constexpr double rad_to_deg = 180.0 / 3.14159265358979323846;
    std::cout << "pose: x=" << x_
          << ", y=" << y_
          << ", theta=" << theta_
          << " rad"
          << " (" << theta_ * rad_to_deg << " deg)"
          << std::endl;
}

Pose MobileRobot::get_pose() const {
    return {x_, y_, theta_};
}

const std::vector<Pose>& MobileRobot::get_path() const {
    return path_;
}

double MobileRobot::compute_total_distance() const {
    return PathAnalyzer::compute_total_distance(path_);
}

void MobileRobot::save_to_csv(const std::string& filename) const {
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
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

    std::cout << "Final ";
    robot.print_pose();
    std::cout << "Total distance traveled: " <<
    robot.compute_total_distance() <<
    " m" << std::endl;

    robot.save_to_csv("path.csv");
}


void runWheelSimulation(MobileRobot& robot, const WheelSimulationConfig& config, const std::string& filename) {
    for (int i = 0; i < config.steps; i++) {
        robot.move_from_wheel_speeds(config.command, config.dt);
    }

    std::cout << "Final ";
    robot.print_pose();
    std::cout << "Total distance traveled: " <<
    robot.compute_total_distance() <<
    " m" << std::endl;

    robot.save_to_csv(filename);
}

#include "robot_motion_sim/robot.hpp"

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace robot_sim {

namespace {
constexpr double kDefaultWheelRadius = 0.05;
constexpr double kDefaultWheelBase = 0.3;
constexpr double kEpsilon = 1e-9;
}

MobileRobot::MobileRobot(double x, double y, double theta) 
    : MobileRobot(x, y, theta, RobotGeometry{kDefaultWheelRadius, kDefaultWheelBase})
{
    
}

MobileRobot::MobileRobot(double x, double y, double theta, const RobotGeometry& geometry)
    : x_(x),
      y_(y),
      theta_(theta),
      geometry_(geometry)
{    
    if (geometry_.wheel_radius <= kEpsilon) {
        throw std::invalid_argument("wheel_radius must be positive");
    }
    
    if (geometry_.wheel_base <= kEpsilon) {
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
        throw std::runtime_error(
                "Could not open file " + filename + " for writing");
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

}

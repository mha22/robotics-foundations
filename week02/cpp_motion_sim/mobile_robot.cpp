#include "mobile_robot.hpp"

#include <cmath>
#include <iostream>

MobileRobot::MobileRobot(double x, double y, double theta) 
    : x_(x),
    y_(y),
    theta_(theta) 
    {path_.push_back({x_, y_, theta_});
}

void MobileRobot::move(double v, double w, double dt) {
    x_ = x_ + v * std::cos(theta_) * dt;
    y_ = y_ + v * std::sin(theta_) * dt;
    theta_ = theta_ + w * dt;

    path_.push_back({x_, y_, theta_});
}

void MobileRobot::printPose() const {
    std::cout << "pose: x=" << x_
    << ", y=" << y_ << ", theta=" << theta_
    << std::endl;
}

Pose MobileRobot::getPose() const {
    return {x_, y_, theta_};
}

const std::vector<Pose>& MobileRobot::getPath() const {
    return path_;
}



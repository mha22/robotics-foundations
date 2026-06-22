#include "mobile_robot.hpp"

#include <cmath>
#include <iostream>
#include <fstream>

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

double MobileRobot::compute_total_distance() const {
    double total = 0;
    double x0, y0, x1, y1, dx, dy;
    for (size_t i=0; i < path_.size() - 1; i++) {
        x0 = path_[i].x;
        y0 = path_[i].y;

        x1 = path_[i + 1].x;
        y1 = path_[i + 1].y;

        dx = x1 - x0;
        dy = y1 - y0;

        total += std::sqrt(dx * dx + dy * dy);
    }
    return total;
}

void MobileRobot::saveToCsv() const {
    std::ofstream file("path.csv");

    file << "step,x,y,theta\n";

    for (size_t i=0; i < path_.size(); i++) {
        file << i << ","
        << path_[i].x << ","
        << path_[i].y << ","
        << path_[i].theta << "\n";
    }

    file.close();
}

#include <cmath>
#include <iostream>
#include <vector>

struct Pose {
    double x;
    double y;
    double theta;
};

Pose robotPosition(Pose pose, double v, double w, double dt) {
    pose.x = pose.x + v * std::cos(pose.theta) * dt;
    pose.y = pose.y + v * std::sin(pose.theta) * dt;
    pose.theta = pose.theta + w * dt;
    return pose;
}

int main() {
    Pose pose{0.0, 0.0, 0.0};
    double v = 0.5;
    double w = 0.2;
    double dt = 0.1;
    int steps = 100;

    std::vector<Pose> path;
    path.push_back(pose);

    for (int i = 0; i < steps; i++) {
        pose = robotPosition(pose, v, w, dt);
        path.push_back(pose);
    }

    std::cout << "Final pose: x=" << pose.x
              << ", y=" << pose.y
              << ", theta=" << pose.theta << std::endl;
    
    return 0;
}
#pragma once

#include <vector>
#include <string>

struct WheelCommand {
    double omega_left;
    double omega_right;
};

struct Pose {
    double x;
    double y;
    double theta;
};

struct RobotGeometry {
    double wheel_radius;
    double wheel_base;
};

struct SimulationConfig  {
    double v;
    double w;
    double dt;
    int steps;
};

struct WheelSimulationConfig {
    WheelCommand command;
    double dt;
    int steps;
};


class MobileRobot {
public:
    MobileRobot(double x, double y, double theta);
    MobileRobot(double x, double y, double theta, const RobotGeometry& geometry);

    void move(double v, double w, double dt);

    void move_from_wheel_speeds(const WheelCommand& cmd,  double dt);

    void print_pose() const;

    Pose get_pose() const;
        
    const std::vector<Pose>& get_path() const;

    double compute_total_distance() const;

    void save_to_csv(const std::string& filename) const;


private:
    double x_;
    double y_;
    double theta_;
    RobotGeometry geometry_;
    std::vector<Pose> path_; 
};


void runSimulation(MobileRobot& robot, const SimulationConfig& config);
void runWheelSimulation(MobileRobot& robot,
                        const WheelSimulationConfig& config,
                        const std::string& filename);

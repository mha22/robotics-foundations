#pragma once

#include <vector>
#include <string>

struct Pose {
    double x;
    double y;
    double theta;
};


class MobileRobot {
public:
    MobileRobot(double x, double y, double theta);

    void move(double v, double w, double dt);

    void print_pose() const;
    

    Pose get_pose() const;
        
    const std::vector<Pose>& get_path() const;

    double compute_total_distance() const;

    void save_to_csv(const std::string& filename) const;


private:
    double x_;
    double y_;
    double theta_;
    std::vector<Pose> path_; 
};

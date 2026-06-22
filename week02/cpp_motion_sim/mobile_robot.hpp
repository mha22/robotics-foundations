#pragma once

#include <vector>

struct Pose {
    double x;
    double y;
    double theta;
};


class MobileRobot {
public:
    MobileRobot(double x, double y, double theta);

    void move(double v, double w, double dt);

    void printPose() const;
    

    Pose getPose() const;
        
    const std::vector<Pose>& getPath() const;

    double compute_total_distance() const;

    void saveToCsv() const;


private:
    double x_;
    double y_;
    double theta_;
    std::vector<Pose> path_; 
};

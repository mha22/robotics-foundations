#pragma once

#include "robot.hpp"
#include <vector>

class PathAnalyzer {
public:
    static double normalize_deg_signed(double deg);

    static double compute_total_distance(const std::vector<Pose>& path);
    
    static double compute_net_displacement(const std::vector<Pose>& path);

    static double compute_final_heading_deg(const std::vector<Pose>& path);

    static double compute_max_distance_from_origin(const std::vector<Pose>& path);
};

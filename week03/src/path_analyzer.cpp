#include "path_analyzer.hpp"

#include <cmath>
#include <stdexcept>

namespace robot_sim {

double PathAnalyzer::normalize_deg_signed(double deg) {
    double r = std::fmod(deg + 180.0, 360.0);
    if (r < 0.0) {
        r += 360.0;
    }
    return r - 180.0;
}

double PathAnalyzer::compute_total_distance(const std::vector<Pose>& path) {
    if (path.size() < 2) {
        return 0.0;
    }

    double total = 0.0;
    double x0, y0, x1, y1, dx, dy;
    for (size_t i = 0; i < path.size() - 1; i++) {
        x0 = path[i].x;
        y0 = path[i].y;

        x1 = path[i + 1].x;
        y1 = path[i + 1].y;

        dx = x1 - x0;
        dy = y1 - y0;

        total += std::sqrt(dx * dx + dy * dy);
    }
    return total;
}

double PathAnalyzer::compute_net_displacement(const std::vector<Pose>& path) {
    if (path.size() < 2) {
        return 0.0;
    }

    Pose start = path.front();
    Pose end = path.back();

    double dx = end.x - start.x;
    double dy = end.y - start.y;
    return std::sqrt(dx * dx + dy * dy);
}

double PathAnalyzer::compute_final_heading_deg(const std::vector<Pose>& path) {
    if (path.empty()) {
        return 0.0;
    }
    constexpr double rad_to_deg = 180.0 / 3.14159265358979323846;
    double deg = path.back().theta * rad_to_deg;
    return normalize_deg_signed(deg);
}

double PathAnalyzer::compute_max_distance_from_origin(const std::vector<Pose>& path) {
    if (path.empty()) {
        return 0.0;
    }
    
    double max_d = 0.0;
    for (const Pose& p : path) {
        double d = std::sqrt(p.x * p.x + p.y * p.y);
        if (d > max_d) {
            max_d = d;
        }
    }
    return max_d;
}

}

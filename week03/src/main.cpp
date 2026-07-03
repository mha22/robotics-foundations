#include "simulation_app.hpp"
#include "logger.hpp"

#include <memory>

using namespace robot_sim;

int main(int argc, char* argv[]) {
    try {
        auto app = std::make_unique<SimulationApp>();
        app->setup(argc, argv);
        app->execute();
    }
    catch (const std::exception& e) {
        Logger::error(e.what());
        return 1;
    }

    return 0;
}

cmake_minimum_required(VERSION 3.16)

project(
    MobileRobotProject
    VERSION 1.0
    DESCRIPTION "Simple mobile robot simulation"
    LANGUAGES CXX
)

add_library(robot_core
            src/robot.cpp
            src/path_analyzer.cpp
            src/logger.cpp
            src/simulation_app.cpp
)

target_include_directories(robot_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(robot_core PUBLIC cxx_std_17)

target_compile_options(robot_core PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_executable(motion_sim src/main.cpp)

target_compile_options(motion_sim PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

target_link_libraries(motion_sim PRIVATE robot_core)

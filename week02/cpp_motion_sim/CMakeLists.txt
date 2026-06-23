cmake_minimum_required(VERSION 3.16)

project(
    MobileRobotProject
    VERSION 1.0
    DESCRIPTION "Simple mobile robot simulation"
    LANGUAGES CXX
)


add_executable(motion_sim main.cpp robot.cpp)

target_compile_features(motion_sim PRIVATE cxx_std_17)

target_include_directories(motion_sim PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(motion_sim PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

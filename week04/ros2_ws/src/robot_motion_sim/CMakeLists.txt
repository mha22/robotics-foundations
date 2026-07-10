cmake_minimum_required(VERSION 3.8)
project(robot_motion_sim)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)

add_executable(simple_timer_node src/simple_timer_node.cpp)
ament_target_dependencies(simple_timer_node rclcpp)

add_executable(pose_publisher_node src/pose_publisher_node.cpp)
ament_target_dependencies(
  pose_publisher_node
  rclcpp
  geometry_msgs
)

add_executable(velocity_subscriber_node src/velocity_subscriber_node.cpp)
ament_target_dependencies(
  velocity_subscriber_node
  rclcpp
  geometry_msgs
)

install(TARGETS
  simple_timer_node
  pose_publisher_node
  velocity_subscriber_node
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()



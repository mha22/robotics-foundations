# robot_motion_sim

## Overview

`robot_motion_sim` is a ROS 2 C++ package that simulates the motion of a mobile robot.

The simulator receives velocity commands from `/cmd_vel`, updates the robot state in a timer-based simulation loop, and publishes the current 2D robot pose on `/robot_pose`.

This project demonstrates core ROS 2 concepts such as publishers, subscribers, timers, parameters, safety timeout logic, and reusing a C++ `MobileRobot` model inside a ROS 2 node.

## Features

- Subscribe to `/cmd_vel`
- Publish `/robot_pose`
- Timer-based simulation loop
- Safety timeout for stale velocity commands
- ROS 2 parameters for simulator configuration
- Reuses the C++ `MobileRobot` model

## Topics

### Subscribed

- `/cmd_vel`: `geometry_msgs/msg/Twist`

Velocity command input for the robot.

Used fields:

- `linear.x`: forward/backward velocity
- `angular.z`: yaw angular velocity

### Published

- `/robot_pose`: `geometry_msgs/msg/Pose2D`

Current simulated robot pose.

Published fields:

- `x`: robot position on the x-axis
- `y`: robot position on the y-axis
- `theta`: robot heading angle

## Parameters

- `update_rate_hz`

  Simulation update frequency in Hz.

- `command_timeout`

  Maximum allowed time without receiving a new `/cmd_vel` message before the robot is stopped.

- `wheel_radius`

  Radius of the robot wheels.

- `wheel_base`

  Distance between the left and right wheels.

Example:
```bash
ros2 run robot_motion_sim robot_simulator_node --ros-args \
  -p update_rate_hz:=20.0 \
  -p command_timeout:=0.5 \
```
## Build

From the root of your ROS 2 workspace:

```bash
colcon build --packages-select robot_motion_sim
source install/setup.bash
```

## Run

Run the robot simulator node:

```bash
ros2 run robot_motion_sim robot_simulator_node
```

## Test

Open three terminals and source the workspace in each one:

```bash
source install/setup.bash
```

### Terminal 1: run the simulator

```bash
ros2 run robot_motion_sim robot_simulator_node
```

### Terminal 2: publish velocity commands

```bash
ros2 topic pub --rate 10 /cmd_vel geometry_msgs/msg/Twist \
"{linear: {x: 0.5, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 0.3}}"
```

### Terminal 3: echo the robot pose

```bash
ros2 topic echo /robot_pose
```

### Check publishing rate

```bash
ros2 topic hz /robot_pose
```

## Demo

ROS 2 graph:

```text
/cmd_vel publisher -> robot_simulator_node -> /robot_pose echo
```
![ROS 2 graph](docs/rqt_graph.png)

To generate the graph screenshot:

```bash
ros2 run rqt_graph rqt_graph
```
Save the screenshot as:

```text
docs/rqt_graph.png
```

## Expected Behavior

When velocity commands are published to `/cmd_vel`, the simulator updates the robot pose and publishes it on `/robot_pose`.

If no new command is received before `command_timeout`, the robot stops automatically for safety.

## Package Structure

```text
robot_motion_sim/
├── CMakeLists.txt
├── package.xml
├── README.md
├── docs/
│   └── rqt_graph.png
├── include/
│   └── ...
└── src/
└── ...
```

## Notes

This project is intended as a small ROS 2 mobile robot simulation exercise. It focuses on clean ROS 2 node structure, parameter usage, topic communication, and separating robot motion logic from ROS-specific code.
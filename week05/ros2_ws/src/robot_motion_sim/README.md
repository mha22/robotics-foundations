# robot_motion_sim

## Overview

`robot_motion_sim` is a ROS 2 C++ package that simulates the planar motion of a mobile robot.

The simulator receives velocity commands from `/cmd_vel`, updates the robot state in a timer-based simulation loop, and publishes the robot state as a standard `nav_msgs/msg/Odometry` message on `/odom`.

The robot position is represented in the `odom` frame, while `base_link` is used as the robot's child frame.

This project demonstrates core ROS 2 concepts such as:

- Publishers and subscribers
- Standard ROS 2 message types
- Timer-based simulation
- ROS 2 parameters
- Velocity command timeout logic
- Yaw-to-quaternion conversion
- Reusing a standalone C++ `MobileRobot` model inside a ROS 2 node

## Features

- Subscribes to velocity commands on `/cmd_vel`
- Publishes standard odometry messages on `/odom`
- Simulates planar mobile robot motion
- Converts the robot yaw angle to a quaternion
- Uses `odom` and `base_link` frame identifiers
- Runs the simulation using a configurable timer
- Stops the robot when velocity commands become stale
- Reuses the C++ `MobileRobot` model

## Topics

### Subscribed Topics

#### `/cmd_vel`

Message type:
```text
geometry_msgs/msg/Twist
```

Provides velocity commands for the simulated robot.

Used fields:

- `linear.x`: forward or backward linear velocity
- `angular.z`: yaw angular velocity

Other fields in the message are currently ignored.

### Published Topics

#### `/odom`

Message type:

```text
nav_msgs/msg/Odometry
```

Contains the current simulated robot pose and velocity.

Frame information:

- `header.frame_id`: `odom`
- `child_frame_id`: `base_link`

Published pose fields:

- `pose.pose.position.x`: robot position on the x-axis
- `pose.pose.position.y`: robot position on the y-axis
- `pose.pose.position.z`: always `0.0`
- `pose.pose.orientation`: robot yaw represented as a quaternion

Published twist fields:

- `twist.twist.linear.x`: current forward or backward velocity
- `twist.twist.angular.z`: current yaw angular velocity

## Coordinate Frames

The odometry message uses the following frame relationship:

```text
odom -> base_link
```

The robot position and orientation are expressed relative to the `odom` frame.

The node currently sets the frame identifiers in the odometry message, but it does not publish a TF transform between `odom` and `base_link`.

TF broadcasting may be added in a future version.

## Parameters

### `update_rate_hz`

Simulation update frequency in hertz.

Default value:

```text
20.0
```

If a non-positive value is provided, the node prints a warning and falls back to `20.0` Hz.

### `command_timeout`

Maximum time in seconds that the simulator waits for a new `/cmd_vel` message.

If no new velocity command is received within this duration, the linear and angular velocities are set to zero.

Default value:

```text
0.5
```

### Parameter Example

```bash
ros2 run robot_motion_sim robot_simulator_node --ros-args \
  -p update_rate_hz:=20.0 \
  -p command_timeout:=0.5
```

## Dependencies

The package uses the following ROS 2 dependencies:

- `rclcpp`
- `geometry_msgs`
- `nav_msgs`
- `tf2`

`tf2` is used to convert the robot yaw angle into a quaternion for the odometry orientation.

## Build

From the root of the ROS 2 workspace:

```bash
colcon build --packages-select robot_motion_sim
```

Source the workspace after building:

```bash
source install/setup.bash
```

## Run

Run the robot simulator node:

```bash
ros2 run robot_motion_sim robot_simulator_node
```

## Test

Open three terminals and source the workspace in each terminal:

```bash
source install/setup.bash
```

### Terminal 1: Run the simulator

```bash
ros2 run robot_motion_sim robot_simulator_node
```

### Terminal 2: Publish velocity commands

```bash
ros2 topic pub --rate 10 /cmd_vel geometry_msgs/msg/Twist \
"{linear: {x: 0.5}, angular: {z: 0.3}}"
```

This command requests:

- A linear velocity of `0.5`
- An angular velocity of `0.3`

### Terminal 3: Inspect odometry

```bash
ros2 topic echo /odom
```

The output should contain:

- An increasing or changing robot position
- A quaternion representing the robot orientation
- The commanded linear velocity
- The commanded angular velocity
- `odom` as the parent frame
- `base_link` as the child frame

## Additional Inspection Commands

### Display topic information

```bash
ros2 topic info /odom
```

### Display the Odometry message definition

```bash
ros2 interface show nav_msgs/msg/Odometry
```

### Check the odometry publishing rate

```bash
ros2 topic hz /odom
```

The measured rate should be close to the configured `update_rate_hz`.

### List active topics

```bash
ros2 topic list
```

### Display node information

```bash
ros2 node info /robot_simulator_node
```

## Demo

ROS 2 communication graph:

```text
/cmd_vel publisher
|
v
robot_simulator_node
|
v
/odom
```
![ROS 2 graph](docs/rqt_graph.png)

To inspect the ROS 2 graph:

```bash
ros2 run rqt_graph rqt_graph
```

Save the graph screenshot as:

```text
docs/rqt_graph.png
```

If the existing screenshot still shows `/robot_pose`, regenerate it after running the updated node.

## Expected Behavior

When velocity commands are published on `/cmd_vel`, the simulator:

1. Reads `linear.x` and `angular.z`.
2. Updates the robot pose at the configured simulation rate.
3. Converts the robot heading angle to a quaternion.
4. Publishes the current pose and velocity on `/odom`.

When velocity commands stop arriving, the robot continues using the last received command until `command_timeout` expires.

After the timeout expires:

- Linear velocity becomes zero.
- Angular velocity becomes zero.
- The simulated robot stops moving.
- Odometry messages continue to be published with zero velocity.

## Odometry Covariance

The `nav_msgs/msg/Odometry` message contains covariance matrices for both pose and twist.

Covariance fields are currently left at their default values for simplicity.

In a real robot or sensor-fusion system, these values should represent the uncertainty of the pose and velocity estimates. Correct covariance values become especially important when using tools such as:

- Extended Kalman filters
- Robot localization
- Sensor fusion
- Navigation systems

## Package Structure

```text
robot_motion_sim/
├── CMakeLists.txt
├── docs
│   └── rqt_graph.png
├── include
│   └── robot_motion_sim
│       └── robot.hpp
├── package.xml
├── README.md
└── src
    ├── robot.cpp
    └── robot_simulator_node.cpp
```

The exact source file names may differ depending on the current project layout.

## Current Limitations

- Motion is simulated only in the 2D plane.
- Only `linear.x` and `angular.z` velocity commands are used.
- Odometry covariance values are not configured yet.
- The node does not currently publish the `odom` to `base_link` TF transform.
- The simulation uses a fixed time step calculated from `update_rate_hz`.
- The simulator does not model wheel slip, sensor noise, or other physical effects.

## Notes

This project is intended as a small ROS 2 mobile robot simulation exercise.

It focuses on:

- Clean ROS 2 node structure
- Standard ROS 2 odometry messages
- Parameter usage
- Topic communication
- Safe handling of stale commands
- Separation of robot motion logic from ROS-specific code
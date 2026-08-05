# `robot_motion_sim`

A ROS 2 C++ package for simulating a planar differential-drive robot with odometry, TF, wheel joint states, a Xacro robot model, a simple ray-cast 2D LiDAR sensor, and Nav2-based autonomous navigation.

The simulator subscribes to velocity commands on `/cmd_vel`, updates the robot state in a timer-based loop, and publishes odometry, transforms, joint states, laser scans, and world visualization markers. The package also provides an integrated Nav2 bringup launch file for goal-based navigation in RViz.

---

## Features

- Differential-drive robot motion simulation
- `/cmd_vel` velocity command input
- `/odom` odometry publishing
- Dynamic TF publishing: `odom -> base_link`
- Static map-to-odom transform for Nav2 simulation bringup
- Xacro robot model loaded by `robot_state_publisher`
- Wheel joint state publishing on `/joint_states`
- Simulated 2D LiDAR on `/scan`
- Simple 2D ray-casting against line-segment worlds
- RViz world visualization using `MarkerArray` on `/world_map`
- YAML-based simulator and Nav2 configuration
- Integrated Nav2 launch support
- Goal navigation through RViz `2D Goal Pose`

---

## Package Structure

```text
robot_motion_sim/
├── CMakeLists.txt
├── package.xml
├── README.md
├── config/
│   ├── sim_params.yaml
│   └── nav2_params.yaml
├── launch/
│   ├── sim.launch.py
│   └── bringup_nav.launch.py
├── maps/
│   ├── map.yaml
│   └── map.pgm
├── rviz/
│   └── sim.rviz
├── src/
│   └── robot_simulator_node.cpp
└── urdf/
    └── robot.urdf.xacro
```

---

## Build

From the root of your ROS 2 workspace:

```bash
colcon build --packages-select robot_motion_sim
source install/setup.bash
```

---

## Launch Simulator Only

Run the simulator with RViz:

```bash
ros2 launch robot_motion_sim sim.launch.py use_rviz:=true
```

Run without RViz:

```bash
ros2 launch robot_motion_sim sim.launch.py use_rviz:=false
```

Use a custom simulator parameter file:

```bash
ros2 launch robot_motion_sim sim.launch.py \
  use_rviz:=true \
  params_file:=/path/to/custom_params.yaml
```

---

## Launch Nav2 Navigation

Run the simulator together with Nav2:

```bash
ros2 launch robot_motion_sim bringup_nav.launch.py
```

This launch file starts the simulator, robot model publishing, map server, planner, controller, smoother, behavior server, BT navigator, velocity smoother, and lifecycle manager.

After launch, set a navigation goal in RViz using `2D Goal Pose`.

---

## Nav2 Nodes

Expected lifecycle-managed Nav2 nodes:

```text
/map_server
/controller_server
/planner_server
/smoother_server
/behavior_server
/bt_navigator
/velocity_smoother
```

Check lifecycle states:

```bash
ros2 lifecycle get /map_server
ros2 lifecycle get /controller_server
ros2 lifecycle get /planner_server
ros2 lifecycle get /bt_navigator
```

Expected result:

```text
active [3]
```

---

## Topics

### Subscribed Topics

| Topic | Type | Description |
|---|---|---|
| `/cmd_vel` | `geometry_msgs/msg/Twist` | Final robot velocity command |

### Published Topics

| Topic | Type | Description |
|---|---|---|
| `/odom` | `nav_msgs/msg/Odometry` | Robot odometry |
| `/tf` | `tf2_msgs/msg/TFMessage` | Dynamic transforms |
| `/joint_states` | `sensor_msgs/msg/JointState` | Wheel joint states |
| `/scan` | `sensor_msgs/msg/LaserScan` | Simulated 2D LiDAR |
| `/world_map` | `visualization_msgs/msg/MarkerArray` | World wall visualization |

Nav2-related velocity topics may include:

```text
/cmd_vel
/cmd_vel_nav
/cmd_vel_teleop
```

In the current working setup, the simulator receives final velocity commands on `/cmd_vel`.

---

## TF Tree

The simulator publishes:

```text
odom -> base_link
```

The Nav2 bringup also provides:

```text
map -> odom
```

The robot model defines:

```text
base_link
├── left_wheel_link
├── right_wheel_link
├── caster_link
└── laser_link
```

Complete TF tree for Nav2:

```text
map
└── odom
└── base_link
├── left_wheel_link
├── right_wheel_link
├── caster_link
└── laser_link
```

Inspect TF:

```bash
ros2 run tf2_ros tf2_echo map base_link
ros2 run tf2_ros tf2_echo odom base_link
ros2 run tf2_ros tf2_echo base_link laser_link
```

Generate a TF graph:

```bash
ros2 run tf2_tools view_frames
```

---

## Configuration

Simulator parameters are stored in:

```text
config/sim_params.yaml
```

Nav2 parameters are stored in:

```text
config/nav2_params.yaml
```

Important simulator parameters:

| Parameter | Description | Example |
|---|---|---|
| `update_rate_hz` | Main simulation update rate | `20.0` |
| `command_timeout` | Stop robot if command is stale | `0.5` |
| `odom_frame_id` | Odometry frame | `"odom"` |
| `base_frame_id` | Robot base frame | `"base_link"` |
| `wheel_radius` | Wheel radius in meters | `0.05` |
| `wheel_base` | Distance between wheels | `0.28` |
| `scan_rate_hz` | Laser scan publishing rate | `10.0` |
| `scan_frame_id` | Laser frame ID | `"laser_link"` |
| `world_name` | Simulated world name | `"simple_room"` |

Important Nav2 parameters:

| Parameter | Description | Current value |
|---|---|---|
| `global_frame` | Navigation global frame | `map` |
| `robot_base_frame` | Robot base frame | `base_link` |
| `odom_topic` | Odometry topic | `/odom` |
| `controller_frequency` | Controller update frequency | `10.0` |
| `FollowPath.model_dt` | MPPI model timestep | `0.1` |

For MPPI, `controller_frequency: 10.0` and `model_dt: 0.1` are intentionally matched.

---

## Simulated LiDAR

The simulator publishes a 2D laser scan on:

```text
/scan
```

Type:

```text
sensor_msgs/msg/LaserScan
```

Default frame:

```text
laser_link
```

Check the scan:

```bash
ros2 topic echo /scan --once
ros2 topic hz /scan
```

---

## RViz Visualization

Recommended RViz displays:

- `Map`
- `TF`
- `RobotModel`
- `Odometry`
- `LaserScan` on `/scan`
- `MarkerArray` on `/world_map`
- `Global Costmap`
- `Local Costmap`
- `Path`

For simulator-only visualization, use fixed frame:

```text
odom
```

For Nav2 visualization, use fixed frame:

```text
map
```

---

## Testing Simulator Motion

Start the simulator:

```bash
ros2 launch robot_motion_sim sim.launch.py use_rviz:=true
```

Send a forward command:

```bash
ros2 topic pub --rate 10 /cmd_vel geometry_msgs/msg/Twist \
"{linear: {x: 0.3}, angular: {z: 0.0}}"
```

Rotate in place:

```bash
ros2 topic pub --rate 10 /cmd_vel geometry_msgs/msg/Twist \
"{linear: {x: 0.0}, angular: {z: 0.8}}"
```

Check odometry and joint states:

```bash
ros2 topic echo /odom
ros2 topic echo /joint_states
```

---

## Testing Nav2

Launch Nav2:

```bash
ros2 launch robot_motion_sim bringup_nav.launch.py
```

Confirm Nav2 lifecycle nodes are active:

```bash
ros2 lifecycle get /map_server
ros2 lifecycle get /controller_server
ros2 lifecycle get /planner_server
ros2 lifecycle get /bt_navigator
```

Send a goal using RViz `2D Goal Pose`.

Check velocity output:

```bash
ros2 topic echo /cmd_vel
```

Inspect topic connections:

```bash
ros2 topic info /cmd_vel -v
```

The robot should move toward the clicked goal.

---

## Dependencies

Main ROS 2 dependencies:

- `rclcpp`
- `geometry_msgs`
- `nav_msgs`
- `sensor_msgs`
- `visualization_msgs`
- `tf2`
- `tf2_ros`
- `tf2_msgs`
- `robot_state_publisher`
- `xacro`
- `rviz2`
- `nav2_bringup`
- `nav2_map_server`
- `nav2_controller`
- `nav2_planner`
- `nav2_bt_navigator`
- `nav2_velocity_smoother`
- `launch`
- `launch_ros`

---

## Notes

- The simulator is 2D only.
- Only `linear.x` and `angular.z` from `/cmd_vel` are used.
- LiDAR ray-casting is based on simple line-segment intersection.
- The scan frame is `laser_link`.
- The world marker frame is `odom`.
- Nav2 uses `map` as the global frame and `base_link` as the robot base frame.
- The current Nav2 setup uses a static `map -> odom` transform for simulation.
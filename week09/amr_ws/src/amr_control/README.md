# AMR LiDAR Simulation

ROS 2 Jazzy Gazebo simulation for a differential-drive Autonomous Mobile Robot (AMR) equipped with a GPU LiDAR sensor.

## Features

- Gazebo Sim world with GPU LiDAR sensor support
- Custom simulation world with physics, lighting, and ground plane
- `gz_ros2_control` integration
- Differential-drive controllers
- ROS-Gazebo bridges for `/clock` and `/scan`
- LiDAR data published on `/scan` using the `lidar_link` frame

## Build

```bash
cd ~/amr_ws
source /opt/ros/jazzy/setup.zsh
colcon build --symlink-install
source install/setup.zsh
```

## Run

```bash
ros2 launch amr_control simulation.launch.py
```

## Verify

```bash
ros2 control list_controllers
ros2 topic echo /scan --once
gz topic -e -t /scan
```

The GPU LiDAR requires the Gazebo Sensors system plugin in the simulation world.

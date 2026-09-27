# AMR Description

This package contains the URDF/Xacro description of a differential-drive AMR robot for ROS 2 Jazzy.

## Contents

- Robot links, joints, visuals, collisions, and inertial properties
- Left and right wheel joints
- `ros2_control` interfaces
- `robot_state_publisher` launch file
- RViz visualization support

## Build
```bash
cd ~/amr_ws
colcon build --symlink-install
source install/setup.bash
```

## Launch

```bash
ros2 launch amr_description description.launch.py
```

To use the simulation configuration:

```bash
ros2 launch amr_description description.launch.py use_sim:=true
```

## Validate the Xacro Model

```bash
ros2 run xacro xacro \
  src/amr_description/urdf/amr.urdf.xacro \
  use_sim:=true \
  > /tmp/amr.urdf

check_urdf /tmp/amr.urdf
```

Controller configuration and Gazebo integration will be added soon.

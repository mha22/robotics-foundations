# AMR Control

ROS 2 Jazzy control package for a differential-drive AMR using `ros2_control`.

## Features

- Mock hardware with `mock_components/GenericSystem`
- `joint_state_broadcaster`
- `diff_drive_controller`
- Velocity command input through `/cmd_vel`
- Joint state publishing on `/joint_states`
- Odometry publishing on `/odom`
- `odom -> base_link` TF publishing
- Controller configuration and launch files

## Build
```bash
cd ~/amr_ws
source /opt/ros/jazzy/setup.bash
colcon build --symlink-install
source install/setup.bash
```

## Launch

```bash
ros2 launch amr_control control.launch.py
```

The default configuration uses mock hardware:

```bash
ros2 launch amr_control control.launch.py use_sim:=false
```

## Verify Controllers

```bash
ros2 control list_controllers
ros2 control list_hardware_interfaces
```

## Test Topics

```bash
ros2 topic echo /joint_states
ros2 topic echo /odom
ros2 run tf2_ros tf2_echo odom base_link
```

To send a velocity command:

```bash
ros2 topic pub /diff_drive_controller/cmd_vel geometry_msgs/msg TwistStamped \
"{header: {stamp: {sec: 0, nanosec: 0}, frame_id: ''}, twist: {linear: {x: 0.2, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 0.0}}}" -r 10
```

Gazebo integration will be added in the next lesson.

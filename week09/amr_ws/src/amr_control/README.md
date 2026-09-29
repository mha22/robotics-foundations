# AMR Gazebo Simulation

ROS 2 Jazzy simulation setup for a differential-drive Autonomous Mobile Robot (AMR). This lesson connects the robot model to Gazebo Sim using `gz_ros2_control` and runs the wheel controllers inside the simulator.

## Features

- Xacro-based robot description
- Gazebo Sim integration through `gz_ros2_control`
- `joint_state_broadcaster` and `diff_drive_controller`
- Separate launch files for mock control and Gazebo simulation

## Build
```bash
cd ~/amr_ws
source /opt/ros/jazzy/setup.zsh
colcon build --symlink-install
source install/setup.zsh
```

## Run the Simulation

```bash
ros2 launch amr_control simulation.launch.py
```

## Verify

```bash
ros2 control list_controllers
ros2 control list_hardware_interfaces
ros2 topic echo /joint_states
ros2 topic echo /diff_drive_controller/odom
```

Send a forward velocity command:

```bash
 ros2 topic pub -r 20 /diff_drive_controller/cmd_vel geometry_msgs/msg/TwistStamped \
"{header: {stamp: {sec: 0, nanosec: 0}, frame_id: ''}, twist: {linear: {x: 0.0, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 0.6}}}"
```

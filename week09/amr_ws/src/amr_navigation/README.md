# AMR SLAM and Navigation

ROS 2 Jazzy navigation setup for an AMR using Gazebo, SLAM Toolbox, Nav2, LiDAR, and differential-drive control.

## Features

- SLAM Toolbox mapping with `/scan`
- Nav2 navigation with global and local costmaps
- Collision monitor and velocity smoother
- TF verification between `map`, `odom`, and `base_link`
- LiDAR and odometry integration
- Map saving for navigation after restart

## Run Nav2
```bash
ros2 launch nav2_bringup navigation_launch.py \
  params_file:=src/amr_navigation/config/nav2_params.yaml \
  use_sim_time:=true \
  autostart:=true \
  use_composition:=False
```

## Verify

```bash
ros2 lifecycle get /controller_server
ros2 lifecycle get /planner_server
ros2 lifecycle get /bt_navigator
ros2 action list | grep navigate
```
The main Nav2 nodes should be in the `active [3]` state and `/navigate_to_pose` should be available.

## Save Map

```bash
mkdir -p ~/amr_ws/maps
ros2 run nav2_map_server map_saver_cli -f maps/amr_map_v1
```
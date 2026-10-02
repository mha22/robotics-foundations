# AMR LiDAR Visualization

ROS 2 Jazzy Gazebo simulation for validating the LiDAR TF chain and visualizing `/scan` data in RViz.

## Verification

Check the transform between `base_link` and `lidar_link`:
```bash
ros2 run tf2_ros tf2_echo base_link lidar_link
```

Expected result:

```text
Translation: [0.000, 0.000, 0.285]
Rotation: [0.000, 0.000, 0.000, 1.000]
```
Launch RViz:

```bash
rviz2
```

Set the `Fixed Frame` to `base_link` and add a `LaserScan` display for:

```text
/scan
```

In an empty world, the laser may not be visible because there are no nearby obstacles. After adding the `test_box` obstacle to `simulation.sdf`, the LiDAR scan became visible in RViz as expected.

import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, DeclareLaunchArgument
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import Command, LaunchConfiguration, FindExecutable
from launch_ros.actions import Node


def generate_launch_description():
    pkg_share = get_package_share_directory('robot_motion_sim')
    xacro_file = os.path.join(pkg_share, 'urdf', 'robot.urdf.xacro')

    world = LaunchConfiguration('world')

    declare_world = DeclareLaunchArgument(
        'world',
        default_value='empty.sdf'
    )

    robot_description = Command(['xacro', ' ', xacro_file])

    rsp = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        output='screen',
        parameters=[{
            'robot_description': robot_description,
            'use_sim_time': True
        }]
    )

    gz_sim = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('ros_gz_sim'),
                'launch',
                'gz_sim.launch.py'
            )
        ),
        launch_arguments={'gz_args': ['-r ', world]}.items()
    )

    spawn = Node(
        package='ros_gz_sim',
        executable='create',
        output='screen',
        arguments=[
            '-name', 'simple_diff_drive_robot',
            '-topic', 'robot_description'
        ]
    )

    ros_gz_bridge = Node(
        package='ros_gz_bridge',
        executable='parameter_bridge',
        name='ros_gz_bridge',
        arguments=[
            # Gazebo -> ROS
            '/clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock',
            '/odom@nav_msgs/msg/Odometry[gz.msgs.Odometry',
            '/scan@sensor_msgs/msg/LaserScan[gz.msgs.LaserScan',

            # ROS -> Gazebo
            '/cmd_vel@geometry_msgs/msg/Twist]gz.msgs.Twist',


        ],
        output='screen',
    )



    return LaunchDescription([
        declare_world,
        rsp,
        gz_sim,
        spawn,
        ros_gz_bridge
    ])

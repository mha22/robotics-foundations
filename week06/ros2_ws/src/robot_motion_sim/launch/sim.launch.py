import os

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration, Command

from launch_ros.actions import Node


def generate_launch_description():
    package_share_dir = get_package_share_directory('robot_motion_sim')

    params_file = os.path.join(
        package_share_dir,
        'config',
        'sim_params.yaml'
    )

    rviz_config_file = os.path.join(
        package_share_dir,
        'rviz',
        'sim.rviz'
    )

    xacro_file = os.path.join(
        package_share_dir,
        'urdf',
        'robot.urdf.xacro'
    )

    use_rviz = LaunchConfiguration('use_rviz')

    declare_use_rviz_arg = DeclareLaunchArgument(
        'use_rviz',
        default_value='true',
        description='Whether to launch RViz'
    )

    robot_simulator_node = Node(
        package='robot_motion_sim',
        executable='robot_simulator_node',
        name='robot_simulator_node',
        output='screen',
        parameters=[params_file]
    )

    robot_state_publisher_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        output='screen',
        parameters=[{
            'robot_description': Command(['xacro  ', xacro_file])
        }]
    )


    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        output='screen',
        arguments=['-d', rviz_config_file],
        condition=IfCondition(use_rviz)
    )

    return LaunchDescription([
        declare_use_rviz_arg,
        robot_simulator_node,
        robot_state_publisher_node,
        rviz_node
    ])

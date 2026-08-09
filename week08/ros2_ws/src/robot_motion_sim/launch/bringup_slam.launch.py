import os

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration, Command

from launch_ros.actions import Node


def generate_launch_description():
    package_share_dir = get_package_share_directory('robot_motion_sim')

    default_sim_params_file = os.path.join(
        package_share_dir,
        'config',
        'sim_params.yaml'
    )

    mapper_params_file = os.path.join(
        package_share_dir,
        'config',
        'mapper_params_online_async.yaml'
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
    sim_params_file = LaunchConfiguration('sim_params_file')
    use_sim_time = LaunchConfiguration('use_sim_time')

    declare_use_rviz_arg = DeclareLaunchArgument(
        'use_rviz',
        default_value='true',
        description='Whether to launch RViz'
    )

    declare_sim_params_file_arg = DeclareLaunchArgument(
        'sim_params_file',
        default_value=default_sim_params_file,
        description='Path to the simulator parameters file'
    )

    declare_use_sim_time_arg = DeclareLaunchArgument(
        'use_sim_time',
        default_value='false',
        description='Use simulation time'
    )

    robot_simulator_node = Node(
        package='robot_motion_sim',
        executable='robot_simulator_node',
        name='robot_simulator_node',
        output='screen',
        parameters=[sim_params_file]
    )

    robot_state_publisher_node = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        output='screen',
        parameters=[{
            'use_sim_time': use_sim_time,
            'robot_description': Command(['xacro ', xacro_file])
        }]
    )

    slam_toolbox_node = Node(
        package='slam_toolbox',
        executable='async_slam_toolbox_node',
        name='slam_toolbox',
        output='screen',
        parameters=[
            mapper_params_file,
            {'use_sim_time': use_sim_time}
        ]
    )

    lifecycle_manager_slam_node = Node(
        package='nav2_lifecycle_manager',
        executable='lifecycle_manager',
        name='lifecycle_manager_slam',
        output='screen',
        parameters=[{
            'use_sim_time': use_sim_time,
            'autostart': True,
            'node_names': ['slam_toolbox']
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
        declare_sim_params_file_arg,
        declare_use_sim_time_arg,

        robot_simulator_node,
        robot_state_publisher_node,
        slam_toolbox_node,
        lifecycle_manager_slam_node,

        rviz_node,
    ])

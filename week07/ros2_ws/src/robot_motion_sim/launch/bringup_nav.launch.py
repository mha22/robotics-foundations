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

    nav2_params_file = os.path.join(
        package_share_dir,
        'config',
        'nav2_params.yaml'
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
        default_value='true',
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

    map_to_odom_tf_node = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        name='map_to_odom_static_tf',
        output='screen',
        arguments=['0', '0', '0', '0', '0', '0', 'map', 'odom']
    )

    map_server_node = Node(
        package='nav2_map_server',
        executable='map_server',
        name='map_server',
        output='screen',
        parameters=[nav2_params_file]
    )

    planner_server_node = Node(
        package='nav2_planner',
        executable='planner_server',
        name='planner_server',
        output='screen',
        parameters=[nav2_params_file]
    )

    controller_server_node = Node(
        package='nav2_controller',
        executable='controller_server',
        name='controller_server',
        output='screen',
        parameters=[nav2_params_file]
    )

    smoother_server_node = Node(
        package='nav2_smoother',
        executable='smoother_server',
        name='smoother_server',
        output='screen',
        parameters=[nav2_params_file]
    )

    behavior_server_node = Node(
        package='nav2_behaviors',
        executable='behavior_server',
        name='behavior_server',
        output='screen',
        parameters=[nav2_params_file]
    )

    bt_navigator_node = Node(
        package='nav2_bt_navigator',
        executable='bt_navigator',
        name='bt_navigator',
        output='screen',
        parameters=[nav2_params_file]
    )

    velocity_smoother_node = Node(
        package='nav2_velocity_smoother',
        executable='velocity_smoother',
        name='velocity_smoother',
        output='screen',
        parameters=[nav2_params_file],
        remappings=[
            ('cmd_vel', 'cmd_vel_nav'),
            ('cmd_vel_smoothed', 'cmd_vel'),
        ]
    )

    lifecycle_manager_node = Node(
        package='nav2_lifecycle_manager',
        executable='lifecycle_manager',
        name='lifecycle_manager_navigation',
        output='screen',
        parameters=[nav2_params_file]
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

        map_to_odom_tf_node,

        map_server_node,
        planner_server_node,
        controller_server_node,
        smoother_server_node,
        behavior_server_node,
        bt_navigator_node,
        velocity_smoother_node,
        lifecycle_manager_node,

        rviz_node,
    ])

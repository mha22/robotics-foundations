import os

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    package_share_dir = get_package_share_directory('robot_motion_sim')

    params_file = os.path.join(
        package_share_dir,
        'config',
        'sim_params.yaml'
    )

    robot_simulator_node = Node(
        package='robot_motion_sim',
        executable='robot_simulator_node',
        name='robot_simulator_node',
        output='screen',
        parameters=[params_file]
    )

    return LaunchDescription([
        robot_simulator_node
    ])

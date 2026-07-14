from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    robot_simulator_node = Node(
        package='robot_motion_sim',
        executable='robot_simulator_node',
        name='robot_simulator_node',
        output='screen',
        parameters=[
            {
                'update_rate_hz': 1.0,
                'command_timeout': 0.5,
                'odom_frame_id': 'odom',
                'base_frame_id': 'base_link'
            }
        ]
    )

    return LaunchDescription([robot_simulator_node])
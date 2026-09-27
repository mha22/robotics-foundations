from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import Command, LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    use_sim = LaunchConfiguration('use_sim')

    robot_description_xacro = PathJoinSubstitution([
        FindPackageShare('amr_description'),
        'urdf',
        'amr.urdf.xacro'
    ])

    robot_description = {
        'robot_description': Command([
            'xacro ',
            robot_description_xacro,
            ' use_sim:=',
            use_sim
        ])
    }

    return LaunchDescription([
        DeclareLaunchArgument(
            'use_sim',
            default_value='true',
            description='Use Gazebo Sim compatible ros2_control plugin'
        ),

        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            name='robot_state_publisher',
            output='screen',
            parameters=[robot_description]
        )
    ])

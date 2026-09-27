from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, RegisterEventHandler
from launch.conditions import IfCondition
from launch.event_handlers import OnProcessStart
from launch.substitutions import Command, LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    use_sim = LaunchConfiguration("use_sim")
    use_rviz_like_rsp = LaunchConfiguration("use_robot_state_publisher")

    description_pkg = FindPackageShare("amr_description")
    control_pkg = FindPackageShare("amr_control")

    robot_description_file = PathJoinSubstitution([
        description_pkg,
        "urdf",
        "amr.urdf.xacro"
    ])

    controllers_file = PathJoinSubstitution([
        control_pkg,
        "config",
        "controllers.yaml"
    ])

    robot_description_content = Command([
        "xacro",
        " ",
        robot_description_file,
        " ",
        "use_sim:=",
        use_sim
    ])

    robot_description = {
        "robot_description": robot_description_content
    }

    robot_state_publisher_node = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        name="robot_state_publisher",
        output="screen",
        parameters=[robot_description],
        condition=IfCondition(use_rviz_like_rsp),
    )

    ros2_control_node = Node(
        package="controller_manager",
        executable="ros2_control_node",
        parameters=[robot_description, controllers_file],
        output="screen",
    )

    joint_state_broadcaster_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=[
            "joint_state_broadcaster",
            "--controller-manager",
            "/controller_manager",
        ],
        output="screen",
    )

    diff_drive_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=[
            "diff_drive_controller",
            "--controller-manager",
            "/controller_manager",
        ],
        output="screen",
    )

    delay_joint_state_broadcaster_after_control_node = RegisterEventHandler(
        OnProcessStart(
            target_action=ros2_control_node,
            on_start=[joint_state_broadcaster_spawner],
        )
    )

    delay_diff_drive_controller_after_control_node = RegisterEventHandler(
        OnProcessStart(
            target_action=ros2_control_node,
            on_start=[diff_drive_controller_spawner],
        )
    )

    return LaunchDescription([
        DeclareLaunchArgument(
            "use_sim",
            default_value="false",
            description="Use simulator-oriented robot description",
        ),
        DeclareLaunchArgument(
            "use_robot_state_publisher",
            default_value="true",
            description="Launch robot_state_publisher",
        ),
        robot_state_publisher_node,
        ros2_control_node,
        delay_joint_state_broadcaster_after_control_node,
        delay_diff_drive_controller_after_control_node,
    ])

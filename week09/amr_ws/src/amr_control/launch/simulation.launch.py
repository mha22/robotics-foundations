from launch import LaunchDescription
from launch.actions import (
    DeclareLaunchArgument,
    IncludeLaunchDescription,
    RegisterEventHandler,
    TimerAction,
)
from launch.event_handlers import OnProcessExit
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import (
    Command,
    LaunchConfiguration,
    PathJoinSubstitution,
)
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    use_sim_time = LaunchConfiguration("use_sim_time")

    description_pkg = FindPackageShare("amr_description")
    control_pkg = FindPackageShare("amr_control")
    ros_gz_sim_pkg = FindPackageShare("ros_gz_sim")

    robot_description_file = PathJoinSubstitution([
        description_pkg,
        "urdf",
        "amr.urdf.xacro",
    ])

    controllers_file = PathJoinSubstitution([
        control_pkg,
        "config",
        "controllers.yaml",
    ])

    gz_sim_launch_file = PathJoinSubstitution([
        ros_gz_sim_pkg,
        "launch",
        "gz_sim.launch.py",
    ])

    robot_description_content = Command([
        "xacro",
        " ",
        robot_description_file,
        " ",
        "use_sim:=true",
    ])

    robot_description = {
        "robot_description": robot_description_content,
        "use_sim_time": use_sim_time,
    }

    robot_state_publisher_node = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        name="robot_state_publisher",
        output="screen",
        parameters=[robot_description],
    )

    gazebo_node = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(gz_sim_launch_file),
        launch_arguments={
            "gz_args": "-r empty.sdf",
        }.items(),
    )

    spawn_robot_node = Node(
        package="ros_gz_sim",
        executable="create",
        arguments=[
            "-name",
            "amr01",
            "-topic",
            "robot_description",
            "-z",
            "0.20",
        ],
        output="screen",
    )

    joint_state_broadcaster_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=[
            "joint_state_broadcaster",
            "--controller-manager",
            "/controller_manager",
            "--controller-manager-timeout",
            "60",
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
            "--controller-manager-timeout",
            "60",
        ],
        output="screen",
    )

    spawn_robot_then_start_controllers = RegisterEventHandler(
        OnProcessExit(
            target_action=spawn_robot_node,
            on_exit=[
                TimerAction(
                    period=3.0,
                    actions=[
                        joint_state_broadcaster_spawner,
                        diff_drive_controller_spawner,
                    ],
                )
            ],
        )
    )

    clock_bridge_node = Node(
        package="ros_gz_bridge",
        executable="parameter_bridge",
        arguments=[
            "/clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock",
        ],
        output="screen",
    )

    return LaunchDescription([
        DeclareLaunchArgument(
            "use_sim_time",
            default_value="true",
            description="Use Gazebo simulation time",
        ),

        gazebo_node,
        clock_bridge_node,
        robot_state_publisher_node,

        TimerAction(
            period=2.0,
            actions=[spawn_robot_node],
        ),

        spawn_robot_then_start_controllers,
    ])

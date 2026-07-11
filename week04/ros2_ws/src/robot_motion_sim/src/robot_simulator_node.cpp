#include <robot_motion_sim/robot.hpp>

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/pose2_d.hpp>
#include <geometry_msgs/msg/twist.hpp>

#include <cmath>
#include <chrono>
#include <functional>

class RobotSimulatorNode : public rclcpp::Node {
public:
    RobotSimulatorNode() 
        : Node("robot_simulator_node"),
        robot_(0.0, 0.0, 0.0),
        linear_velocity_(0.0),
        angular_velocity_(0.0),
        update_rate_hz_(20.0),
        last_command_time_(now()),
        command_timeout_(rclcpp::Duration::from_seconds(0.5))
        {
        cmd_subscription_ = create_subscription<geometry_msgs::msg::Twist>(
            "cmd_vel",
            10,
            std::bind(&RobotSimulatorNode::command_callback, this, std::placeholders::_1)
        );

        pose_publisher_ = create_publisher<geometry_msgs::msg::Pose2D>("robot_pose", 10);


        
        const double timeout_seconds = declare_parameter<double>("command_timeout", 0.5);
        command_timeout_ = rclcpp::Duration::from_seconds(timeout_seconds);

        update_rate_hz_ = declare_parameter<double>("update_rate_hz", 20.0);
        if (update_rate_hz_ <= 0.0) {
            RCLCPP_WARN(
                get_logger(),
                "update_rate_hz must be positive. Falling back to 20.0 Hz."
            );
            update_rate_hz_ = 20.0;
        }
        
        const auto timer_period = 
            std::chrono::duration<double>(1.0 / update_rate_hz_);

        timer_ = create_wall_timer(
            timer_period,
            std::bind(&RobotSimulatorNode::update, this)
        );
    }

private:
    void command_callback(
        const geometry_msgs::msg::Twist::SharedPtr message
    )
    {
        linear_velocity_ = message->linear.x;
        angular_velocity_ = message->angular.z;

        last_command_time_ = now();
    }

    void update()
    {
        if ((now() - last_command_time_) > command_timeout_) {
            linear_velocity_ = 0.0;
            angular_velocity_ = 0.0;
        }
        
        const double dt =  1.0 / update_rate_hz_;
        robot_.move(linear_velocity_, angular_velocity_, dt);

        const robot_sim::Pose robot_pose = robot_.get_pose();
        
        geometry_msgs::msg::Pose2D message;
        message.x = robot_pose.x;
        message.y = robot_pose.y;
        message.theta = robot_pose.theta;

        pose_publisher_->publish(message);
    }

private:
    robot_sim::MobileRobot robot_;
    double linear_velocity_;
    double angular_velocity_;
    double update_rate_hz_;

    rclcpp::Time last_command_time_;
    rclcpp::Duration command_timeout_;

    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<geometry_msgs::msg::Pose2D>::SharedPtr pose_publisher_;
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_subscription_;
};


int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);

    auto node = std::make_shared<RobotSimulatorNode>();
    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}

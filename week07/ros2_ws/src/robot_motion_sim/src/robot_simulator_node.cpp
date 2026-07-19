#include <robot_motion_sim/robot.hpp>

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <sensor_msgs/msg/joint_state.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>

#include <chrono>
#include <functional>
#include <string>
#include <memory>
#include <cmath>

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

        odom_frame_id_ = declare_parameter<std::string>("odom_frame_id", "odom");
        base_frame_id_ = declare_parameter<std::string>("base_frame_id", "base_link");

        cmd_subscription_ = create_subscription<geometry_msgs::msg::Twist>(
            "cmd_vel",
            10,
            std::bind(&RobotSimulatorNode::command_callback, this, std::placeholders::_1)
        );

        odom_publisher_ = create_publisher<nav_msgs::msg::Odometry>("odom", 10);

        tf_broadcaster_ = std::make_unique<tf2_ros::TransformBroadcaster>(*this);

        joint_state_publisher_ = create_publisher<sensor_msgs::msg::JointState>("joint_states", 10);

        scan_publisher_ = create_publisher<sensor_msgs::msg::LaserScan>("scan", 10);
        
        left_wheel_joint_name_ = declare_parameter<std::string>("left_wheel_joint_name", "left_wheel_joint");

        right_wheel_joint_name_ = declare_parameter<std::string>("right_wheel_joint_name", "right_wheel_joint");

        wheel_radius_ = declare_parameter<double>("wheel_radius", 0.05);
        if (wheel_radius_ <= 0.0) {
            RCLCPP_WARN(
                get_logger(),
                "Invalid wheel_radius %.3f. Resetting to default 0.05",
                wheel_radius_
            );
            wheel_radius_ = 0.05;
        }

        wheel_base_ = declare_parameter<double>("wheel_base", 0.28);
        if (wheel_base_ <= 0.0) {
            RCLCPP_WARN(
                get_logger(),
                "Invalid wheel_base %.3f. Resetting to default 0.28",
                wheel_base_
            );
            wheel_base_ = 0.28;
        }

        scan_rate_hz_ = declare_parameter<double>("scan_rate_hz", 10.0);

        scan_angle_min_ = declare_parameter<double>("scan_angle_min", -1.5708);
        scan_angle_max_ = declare_parameter<double>("scan_angle_max", 1.5708);
        scan_angle_increment_ = declare_parameter<double>("scan_angle_increment", 0.0174533);

        scan_range_min_ = declare_parameter<double>("scan_range_min", 0.05);
        scan_range_max_ = declare_parameter<double>("scan_range_max", 5.0);

        scan_frame_id_ = declare_parameter<std::string>("scan_frame_id", "laser_link");

        laser_x_offset_ = declare_parameter<double>("laser_x_offset", 0.18);
        laser_y_offset_ = declare_parameter<double>("laser_y_offset", 0.0);
        laser_yaw_offset_ = declare_parameter<double>("laser_yaw_offset", 0.0);

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

    void publish_odometry(const rclcpp::Time& stamp) {
        const robot_sim::Pose pose = robot_.get_pose();
        tf2::Quaternion q;
        q.setRPY(0.0, 0.0, pose.theta);

        nav_msgs::msg::Odometry odom;
        odom.header.stamp = stamp;
        odom.header.frame_id = odom_frame_id_;
        odom.child_frame_id = base_frame_id_;

        odom.pose.pose.position.x = pose.x;
        odom.pose.pose.position.y = pose.y;
        odom.pose.pose.position.z = 0.0;

        odom.pose.pose.orientation.x = q.x();
        odom.pose.pose.orientation.y = q.y();
        odom.pose.pose.orientation.z = q.z();
        odom.pose.pose.orientation.w = q.w();

        odom.twist.twist.linear.x = linear_velocity_;
        odom.twist.twist.angular.z = angular_velocity_;
    
        odom_publisher_->publish(odom);
    }
    
    void broadcast_transform(const rclcpp::Time& stamp) {
        const robot_sim::Pose pose = robot_.get_pose();
        tf2::Quaternion q;
        q.setRPY(0.0, 0.0, pose.theta);

        geometry_msgs::msg::TransformStamped transform;
        transform.header.stamp = stamp;
        transform.header.frame_id = odom_frame_id_;
        transform.child_frame_id = base_frame_id_;

        transform.transform.translation.x = pose.x;
        transform.transform.translation.y = pose.y;
        transform.transform.translation.z = 0.0;
        

        transform.transform.rotation.x = q.x();
        transform.transform.rotation.y = q.y();
        transform.transform.rotation.z = q.z();
        transform.transform.rotation.w = q.w();

        tf_broadcaster_->sendTransform(transform);
    }

    void publish_joint_states(const rclcpp::Time& stamp) {
        sensor_msgs::msg::JointState joint_state;
        joint_state.header.stamp = stamp;
        
        joint_state.name = {
            left_wheel_joint_name_,
            right_wheel_joint_name_
        };

        joint_state.position = {
            left_wheel_position_rad_,
            right_wheel_position_rad_
        };

        joint_state.velocity = {
            left_wheel_velocity_rad_s_,
            right_wheel_velocity_rad_s_
        };

        joint_state_publisher_->publish(joint_state);

    }

    void publish_laser_scan(const rclcpp::Time& stamp) {
        sensor_msgs::msg::LaserScan scan;

        scan.header.stamp = stamp;
        scan.header.frame_id = scan_frame_id_;

        scan.angle_min = scan_angle_min_;
        scan.angle_max = scan_angle_max_;
        scan.angle_increment = scan_angle_increment_;

        scan.time_increment = 0.0;
        scan.scan_time = 1.0 / scan_rate_hz_;

        scan.range_min = scan_range_min_;
        scan.range_max = scan_range_max_;

        const int beam_count = 
            static_cast<int>(
                std::floor((scan_angle_max_ - scan_angle_min_) / scan_angle_increment_)
            ) + 1;


        scan.ranges.assign(beam_count, scan_range_max_);

        scan_publisher_->publish(scan);
    }

    void apply_command_timeout() {
        if ((now() - last_command_time_) > command_timeout_) {
            linear_velocity_ = 0.0;
            angular_velocity_ = 0.0;
        }
    }

    void update_wheel_states(double dt) {
        const double left_wheel_linear_velocity =
            linear_velocity_ - angular_velocity_ * wheel_base_ / 2.0;
        
        const double right_wheel_linear_velocity = 
            linear_velocity_ + angular_velocity_ * wheel_base_ / 2.0;
        
        left_wheel_velocity_rad_s_ = left_wheel_linear_velocity / wheel_radius_;

        right_wheel_velocity_rad_s_ = right_wheel_linear_velocity / wheel_radius_;
        
        left_wheel_position_rad_ += left_wheel_velocity_rad_s_ * dt;
        right_wheel_position_rad_ += right_wheel_velocity_rad_s_ * dt;
    }

    void update()
    {   
        const double dt =  1.0 / update_rate_hz_;

        apply_command_timeout();

        robot_.move(linear_velocity_, angular_velocity_, dt);

        update_wheel_states(dt);
        
        const auto current_time = now();

        publish_odometry(current_time);
        broadcast_transform(current_time);
        publish_joint_states(current_time);
        publish_laser_scan(current_time);
    }

private:
    robot_sim::MobileRobot robot_;
    double linear_velocity_;
    double angular_velocity_;
    double update_rate_hz_;
    double left_wheel_position_rad_{0.0};
    double right_wheel_position_rad_{0.0};
    double wheel_radius_;
    double wheel_base_;
    double left_wheel_velocity_rad_s_{0.0};
    double right_wheel_velocity_rad_s_{0.0};

    double scan_rate_hz_;
    double scan_angle_min_;
    double scan_angle_max_;
    double scan_angle_increment_;
    double scan_range_min_;
    double scan_range_max_;
    std::string scan_frame_id_;

    double laser_x_offset_;
    double laser_y_offset_;
    double laser_yaw_offset_;

    std::string odom_frame_id_;
    std::string base_frame_id_;
    std::string left_wheel_joint_name_;
    std::string right_wheel_joint_name_;

    rclcpp::Time last_command_time_;
    rclcpp::Duration command_timeout_;

    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_subscription_;
    rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr odom_publisher_;
    std::unique_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr joint_state_publisher_;
    rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr scan_publisher_;
};


int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);

    auto node = std::make_shared<RobotSimulatorNode>();
    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}

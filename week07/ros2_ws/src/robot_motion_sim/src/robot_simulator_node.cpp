#include <robot_motion_sim/robot.hpp>

#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <geometry_msgs/msg/transform_stamped.hpp>
#include <tf2_ros/transform_broadcaster.h>
#include <sensor_msgs/msg/joint_state.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <visualization_msgs/msg/marker_array.hpp>

#include <chrono>
#include <functional>
#include <string>
#include <memory>
#include <cmath>
#include <vector>
#include <random>

struct Point2D
{
    double x;
    double y;
};

struct Segment2D
{
    Point2D p1;
    Point2D p2;
};

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

        validate_lidar_parameters();

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

        marker_publisher_ = create_publisher<visualization_msgs::msg::MarkerArray>("world_map", 1);
        
        world_name_ = declare_parameter<std::string>("world_name", "simple_room");
        initialize_world();

        scan_noise_enabled_ = declare_parameter<bool>("scan_noise_enabled", false);
        scan_noise_stddev_ = declare_parameter<double>("scan_noise_stddev", 0.01);
        scan_noise_distribution_ = std::normal_distribution<double>(0.0, scan_noise_stddev_);
        
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


        scan.ranges.resize(beam_count);
        scan.intensities.resize(beam_count);

        const double laser_world_yaw = compute_laser_world_yaw();
        const Point2D laser_position = compute_laser_world_position();

        for (int i = 0; i < beam_count; ++i) {
            const double angle_in_laser_frame = 
                scan_angle_min_ + i * scan_angle_increment_;

            const double angle_in_world = 
                laser_world_yaw + angle_in_laser_frame;

            double range = cast_ray(angle_in_world, laser_position);
            range = apply_scan_noise(range);
            scan.ranges[i] = range;
            scan.intensities[i] = 1.0;
        }

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

        if (should_publish_scan(current_time)) {
            publish_laser_scan(current_time);
        }

        static int counter = 0;
        if (counter++ % 20 == 0) {
            publish_world_markers(current_time);
        }
    }

    void initialize_world() {
        world_segments_.clear();

        if (world_name_ == "simple_room") {
            initialize_simple_room();
        }
        else if (world_name_ == "corridor") {
            initialize_corridor();
        }
        else {
            RCLCPP_WARN(
                get_logger(),
                "Unknown world_name '%s'. Falling back to simple_room.",
                world_name_.c_str()
            );
            initialize_simple_room();
        }

        create_world_markers();
    }

    void create_world_markers() {
        world_markers_.markers.clear();
        for (size_t i = 0; i < world_segments_.size(); ++i) {
            visualization_msgs::msg::Marker marker;
            marker.header.frame_id = odom_frame_id_;
            marker.header.stamp = now();
            marker.ns = "walls";
            marker.id = i;
            marker.type = visualization_msgs::msg::Marker::LINE_LIST;
            marker.action = visualization_msgs::msg::Marker::ADD;
            marker.pose.orientation.w = 1.0;

            marker.color.r = 0.0; marker.color.g = 1.0; marker.color.b = 0.0; marker.color.a = 1.0;
            marker.scale.x = 0.05;

            geometry_msgs::msg::Point p1, p2;
            p1.x = world_segments_[i].p1.x; p1.y = world_segments_[i].p1.y;
            p2.x = world_segments_[i].p2.x; p2.y = world_segments_[i].p2.y;

            marker.points.push_back(p1);
            marker.points.push_back(p2);

            world_markers_.markers.push_back(marker);
        }
    }

    void publish_world_markers(const rclcpp::Time& stamp) {
        for (auto& marker : world_markers_.markers) {
            marker.header.stamp = stamp;
        }

        marker_publisher_->publish(world_markers_);
    }

    void initialize_simple_room() {
        world_segments_.clear();
        
        world_segments_.push_back({{-3.0, -2.0}, { 3.0, -2.0}});
        world_segments_.push_back({{ 3.0, -2.0}, { 3.0,  2.0}});
        world_segments_.push_back({{ 3.0,  2.0}, {-3.0,  2.0}});
        world_segments_.push_back({{-3.0,  2.0}, {-3.0, -2.0}});

        world_segments_.push_back({{0.8, -0.8}, {0.8, 0.8}});
        world_segments_.push_back({{-1.2, 0.5}, {-0.3, 0.5}});
    }

    void initialize_corridor() {
        world_segments_.clear();

        world_segments_.push_back({{-4.0, -0.8}, {4.0, -0.8}});
        world_segments_.push_back({{-4.0,  0.8}, {4.0,  0.8}});

        world_segments_.push_back({{-4.0, -0.8}, {-4.0, 0.8}});
        world_segments_.push_back({{ 4.0, -0.8}, { 4.0, 0.8}});

        world_segments_.push_back({{1.0, -0.2}, {1.0, 0.4}});
    }


    Point2D compute_laser_world_position() const {
        const robot_sim::Pose pose = robot_.get_pose();

        const double robot_x = pose.x;
        const double robot_y = pose.y;
        const double robot_theta = pose.theta;

        Point2D laser;

        laser.x = 
            robot_x + 
            std::cos(robot_theta) * laser_x_offset_ -
            std::sin(robot_theta) * laser_y_offset_;

        laser.y = 
            robot_y +
            std::sin(robot_theta) * laser_x_offset_ +
            std::cos(robot_theta) * laser_y_offset_;
        
        return laser;
    }

    double compute_laser_world_yaw() const {
        const robot_sim::Pose pose = robot_.get_pose();
        
        return pose.theta + laser_yaw_offset_;
    }

    bool intersect_ray_segment(
        const Point2D& ray_origin,
        double ray_angle,
        const Segment2D& segment,
        double& distance)
    {
        const double dx = std::cos(ray_angle);
        const double dy = std::sin(ray_angle);

        const double x1 = ray_origin.x;
        const double y1 = ray_origin.y;

        const double x2 = segment.p1.x;
        const double y2 = segment.p1.y;
        const double x3 = segment.p2.x;
        const double y3 = segment.p2.y;

        const double sx = x3 - x2;
        const double sy = y3 - y2;

        const double denominator = dx * sy - dy * sx;

        if (std::fabs(denominator) < 1e-9) {
            return false;
        }

        const double qpx = x2 - x1;
        const double qpy = y2 - y1;

        const double t = (qpx * sy - qpy * sx) / denominator;
        const double u = (qpx * dy - qpy * dx) / denominator;

        if (t >= 0.0 && u >= 0.0 && u <= 1.0) {
            distance = t;
            return true;
        }

        return false;
    }

    double cast_ray(double ray_angle, const Point2D& laser_position)
    {
        

        double closest_distance = scan_range_max_;

        for (const auto& segment : world_segments_) {
            double distance = 0.0;

            if (intersect_ray_segment(laser_position, ray_angle, segment, distance)) {
                if (distance >= scan_range_min_ && distance < closest_distance) {
                    closest_distance = distance;
                }
            }
        }

        return closest_distance;
    }

    bool should_publish_scan(const rclcpp::Time& stamp) {
        if (!has_last_scan_time_) {
            last_scan_time_ = stamp;
            has_last_scan_time_ = true;
            return true;
        }

        const auto scan_period = rclcpp::Duration::from_seconds(1.0 / scan_rate_hz_);
        

        if (stamp >= last_scan_time_ + scan_period) {
            last_scan_time_ = last_scan_time_ + scan_period;
            return true;
        }

        return false;
    }

    void validate_lidar_parameters()
    {
        if (scan_rate_hz_ <= 0.0) {
            RCLCPP_WARN(
                get_logger(),
                "Invalid scan_rate_hz %.3f. Resetting to 10.0",
                scan_rate_hz_
            );
            scan_rate_hz_ = 10.0;
        }

        if (scan_range_min_ < 0.0) {
            RCLCPP_WARN(
                get_logger(),
                "Invalid scan_range_min %.3f. Resetting to 0.05",
                scan_range_min_
            );
            scan_range_min_ = 0.05;
        }

        if (scan_range_max_ <= scan_range_min_) {
            RCLCPP_WARN(
                get_logger(),
                "Invalid scan_range_max %.3f. Resetting to 5.0",
                scan_range_max_
            );
            scan_range_max_ = 5.0;
        }

        if (scan_angle_increment_ <= 0.0) {
            RCLCPP_WARN(
                get_logger(),
                "Invalid scan_angle_increment %.6f. Resetting to 1 degree",
                scan_angle_increment_
            );
            scan_angle_increment_ = 0.0174533;
        }

        if (scan_angle_max_ <= scan_angle_min_) {
            RCLCPP_WARN(
                get_logger(),
                "Invalid scan angle limits. Resetting to [-pi/2, pi/2]"
            );
            scan_angle_min_ = -1.5708;
            scan_angle_max_ = 1.5708;
        }
    }

    double apply_scan_noise(double range) {
        if (!scan_noise_enabled_) {
            return range;
        }

        if (range >= scan_range_max_) {
            return range;
        }

        const double noisy_range = range + scan_noise_distribution_(random_engine_);

        if (noisy_range < scan_range_min_) {
            return scan_range_min_;
        }

        if (noisy_range > scan_range_max_) {
            return scan_range_max_;
        }

        return noisy_range;
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
    rclcpp::Time last_scan_time_;
    bool has_last_scan_time_{false};

    double laser_x_offset_;
    double laser_y_offset_;
    double laser_yaw_offset_;

    std::string odom_frame_id_;
    std::string base_frame_id_;
    std::string left_wheel_joint_name_;
    std::string right_wheel_joint_name_;

    rclcpp::Time last_command_time_;
    rclcpp::Duration command_timeout_;

    std::vector<Segment2D> world_segments_;
    std::string world_name_;
    rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr marker_publisher_;
    visualization_msgs::msg::MarkerArray world_markers_;

    bool scan_noise_enabled_;
    double scan_noise_stddev_;
    std::default_random_engine random_engine_;
    std::normal_distribution<double> scan_noise_distribution_;

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

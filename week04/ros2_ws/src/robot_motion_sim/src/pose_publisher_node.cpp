#include <geometry_msgs/msg/pose2_d.hpp>
#include <rclcpp/rclcpp.hpp>

#include <chrono>
#include <memory>

using namespace std::chrono_literals;

class PosePublisherNode: public rclcpp::Node {
public:
    PosePublisherNode() : Node("pose_publisher_node"), x_(0.0) {
        timer_ = this->create_wall_timer(
            500ms,
            std::bind(&PosePublisherNode::publish_pose, this)

        );

        publisher_ = create_publisher<geometry_msgs::msg::Pose2D>("robot_pose", 10);

        RCLCPP_INFO(this->get_logger(), "PosePublisherNode started");
    }

private:
    void publish_pose() {
        geometry_msgs::msg::Pose2D message;
        message.x = x_;
        message.y = 0.0;
        message.theta = 0.0;

        publisher_->publish(message);
        RCLCPP_INFO(this->get_logger(), "Published x=%.2f", message.x);
        x_ += 0.1;
    }

private:
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<geometry_msgs::msg::Pose2D>::SharedPtr publisher_;
    double x_;
};


int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);

    auto node = std::make_shared<PosePublisherNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}

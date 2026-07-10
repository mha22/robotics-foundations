#include <geometry_msgs/msg/twist.hpp>
#include <rclcpp/rclcpp.hpp>

#include <functional>
#include <memory>

class VelocitySubscriberNode : public rclcpp::Node {
public:
    VelocitySubscriberNode() : Node("velocity_subscriber_node") {
        subscription_  = create_subscription<geometry_msgs::msg::Twist>(
            "cmd_vel",
            10,
            std::bind(&VelocitySubscriberNode::velocity_callback, this, std::placeholders::_1)
        );
    }

private:
    void velocity_callback(const geometry_msgs::msg::Twist::SharedPtr message) {

        RCLCPP_INFO(
            get_logger(),
            "Received v=%.3f , w=%.3f",
            message->linear.x,
            message->angular.z
        );
    }

    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr subscription_;

};

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);

    auto node = std::make_shared<VelocitySubscriberNode>();
    rclcpp::spin(node);

    rclcpp::shutdown();

    return 0;
}
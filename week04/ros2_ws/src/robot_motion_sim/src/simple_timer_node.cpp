#include <rclcpp/rclcpp.hpp>

#include <chrono>

using namespace std::chrono_literals;

class SimpleTimerNode : public rclcpp::Node {
public:
    SimpleTimerNode() : Node("simple_timer_node"), counter_(0) {
        timer_ = this->create_wall_timer(
            1s,
            std::bind(&SimpleTimerNode::on_timer, this)
        );

        RCLCPP_INFO(this->get_logger(), "SimpleTimerNode started");
    }

private:
    void on_timer() {

        RCLCPP_INFO(this->get_logger(), "Time tick: %d", counter_);
        counter_++;
    }

private:
    rclcpp::TimerBase::SharedPtr timer_;
    int counter_;
};

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);

    auto node = std::make_shared<SimpleTimerNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}

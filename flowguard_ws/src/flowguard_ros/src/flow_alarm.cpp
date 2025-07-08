#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

class FlowAlarm : public rclcpp::Node {
public:
    FlowAlarm() : Node("flow_alarm") {
        sub_ = this->create_subscription<std_msgs::msg::String>(
            "/flow_status", 10,
            std::bind(&FlowAlarm::callback, this, std::placeholders::_1)
        );
    }

private:
    void callback(const std_msgs::msg::String::SharedPtr msg) {
        if (msg->data == "inconsistent") {
            RCLCPP_WARN(this->get_logger(), "🚨 INCONSISTENT FLOW DETECTED!");
            std::cout << "\a" << std::flush;  // Sesli uyarı
        }
    }

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr sub_;
};

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<FlowAlarm>());
    rclcpp::shutdown();
    return 0;
}
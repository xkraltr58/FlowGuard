#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float32.hpp"
#include "std_msgs/msg/string.hpp"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

class FlowPublisher : public rclcpp::Node {
public:
    FlowPublisher() : Node("flow_publisher") {
        flow_pub_ = this->create_publisher<std_msgs::msg::Float32>("/flow_value", 10);
        status_pub_ = this->create_publisher<std_msgs::msg::String>("/flow_status", 10);
        timer_ = this->create_wall_timer(
            std::chrono::seconds(1),
            std::bind(&FlowPublisher::publish_flow, this)
        );
    }

private:
    std::string previous_status_ = "consistent";

    std::string applyHysteresis(float value, const std::string& prev_status) {
        if (value > 41.0f) return "consistent";
        if (value < 39.0f) return "inconsistent";
        return prev_status;
    }

    std::string get_last_flow_value() {
        std::ifstream file("/home/naci/FlowGuard/ml/data/flow_log.csv");
        std::string line, last_line;
        while (std::getline(file, line)) {
            if (!line.empty()) last_line = line;
        }

        size_t comma_pos = last_line.find(',');
        if (comma_pos != std::string::npos)
            return last_line.substr(0, comma_pos);
        return "0.0";
    }

    void publish_flow() {
        std::string flow_value_str = get_last_flow_value();
        std::string command = "python3 /home/naci/FlowGuard/ml/ml_model.py " + flow_value_str;
        FILE* pipe = popen(command.c_str(), "r");
        if (!pipe) {
            RCLCPP_ERROR(this->get_logger(), "Failed to run ML model.");
            return;
        }

        char buffer[128];
        std::string result;
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        pclose(pipe);

        size_t delim_pos = result.find('|');
        if (delim_pos == std::string::npos) {
            RCLCPP_WARN(this->get_logger(), "Unexpected result format: %s", result.c_str());
            return;
        }

        std::string raw_status = result.substr(0, delim_pos);
        std::string score_str = result.substr(delim_pos + 1);
        float score = std::stof(score_str);
        float flow_value = std::stof(flow_value_str);

        std::string filtered_status = applyHysteresis(flow_value, previous_status_);
        previous_status_ = filtered_status;

        auto flow_msg = std_msgs::msg::Float32();
        flow_msg.data = score;
        flow_pub_->publish(flow_msg);

        auto status_msg = std_msgs::msg::String();
        status_msg.data = filtered_status;
        status_pub_->publish(status_msg);

        RCLCPP_INFO(this->get_logger(), "Published score: %.4f, status: %s", score, filtered_status.c_str());
    }

    rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr flow_pub_;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr status_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char* argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<FlowPublisher>());
    rclcpp::shutdown();
    return 0;
}
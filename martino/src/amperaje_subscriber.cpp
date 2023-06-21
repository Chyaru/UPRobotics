#include <memory>
#include <chrono>
#include <thread>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float32.hpp"

using std::placeholders::_1;

void topic_callback(const std_msgs::msg::Float32::SharedPtr msg)
{
  RCLCPP_INFO(rclcpp::get_logger("amperaje_subscriber"), "Amperaje: %f", msg->data);
}

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("amperaje_subscriber");
  auto subscription = node->create_subscription<std_msgs::msg::Float32>(
    "amperaje", 10, topic_callback);
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}

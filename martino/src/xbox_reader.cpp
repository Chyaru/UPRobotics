#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include "structures/srv/movement.hpp"

void joyCallback(const sensor_msgs::msg::Joy::SharedPtr joy)
{
    int direction = 0;
    
    // int d1 = , d2 ... d17 
    // if(d1 && ...) direction = 1 ...

    auto request = std::make_shared<structures::srv::Movement::Request>();
    request->direction = direction;

    client_ = create_client<structures::srv::Movement>("client");
    client_->async_send_request(request);
    auto future = client_->async_send_request(request);

    /* auto responseReceived = future.wait_for(std::chrono::milliseconds(200));

    if (responseReceived == std::future_status::ready) {
      auto response = future.get();
      RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "succesful");
    } else {
      RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "not succesful");
    }
    */
}

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  auto node = rclcpp::Node::make_shared("joystick_reader");

  auto joy_sub = node->create_subscription<sensor_msgs::msg::Joy>(
    "/joy", 10, joyCallback);

  rclcpp::spin(node);

  rclcpp::shutdown();

  return 0;
}

#include "rclcpp/rclcpp.hpp"
#include "structures/srv/movement.hpp"

#include <chrono>
#include <cstdlib>
#include <memory>

using namespace std::chrono_literals;

int main(int argc, char **argv)
{
  rclcpp::init(argc, argv);

  if (argc != 2) {
      RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "usage: needed the direction X ");
      return 1;
  }

  std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("martino_client");
  rclcpp::Client<structures::srv::Movement>::SharedPtr client =
    node->create_client<structures::srv::Movement>("direcciones");

  auto request = std::make_shared<structures::srv::Movement::Request>();
  request->direction = atoll(argv[1]);

  while (!client->wait_for_service(1s)) {
    if (!rclcpp::ok()) {
      RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Interrupted while waiting for the service. Exiting.");
      return 0;
    }
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "service not available, waiting again...");
  }

  auto result = client->async_send_request(request);
  // Wait for the result.
  if (rclcpp::spin_until_future_complete(node, result) ==
    rclcpp::FutureReturnCode::SUCCESS)
  {
     RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "speeds:" ); //[%d] [%d] [%d] [%d] [%d] [%d] [%d] [%d] [%d]", result.get()->s0, result.get()->s1, result.get()->s2, result.get()->s3, result.get()->s4, result.get()->s5, result.get()->s6, result.get()->s7, result.get()->s8);
  } else {
    RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Failed to call service direcciones");
  }

  rclcpp::shutdown();
  return 0;
}

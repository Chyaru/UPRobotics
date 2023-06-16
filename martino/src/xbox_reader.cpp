#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/Joy.hpp>
#include <service_package/srv/ServiceType.hpp>

class JoystickReaderNode : public rclcpp::Node
{
public:
  JoystickReaderNode() : Node("joystick_reader_node")
  {
    // Create subscriber
    joy_sub_ = create_subscription<sensor_msgs::msg::Joy>(
      "/joy", 10, std::bind(&JoystickReaderNode::joyCallback, this, std::placeholders::_1));

    // Create client
    client_ = create_client<service_package::srv::ServiceType>("/service_name");
  }

private:
  void joyCallback(const sensor_msgs::msg::Joy::SharedPtr joy)
  {
    // Process the received joystick message
    // ...

    // Create a request object
    auto request = std::make_shared<service_package::srv::ServiceType::Request>();

    // Populate the request with relevant data based on the received message
    // ...

    // Send the request to the service server
    auto future = client_->async_send_request(request);

    // Handle the response when it arrives
    auto responseReceived = future.wait_for(std::chrono::seconds(5));

    if (responseReceived == std::future_status::ready) {
      auto response = future.get();
      // Process the response received from the service server
    } else {
      // Handle case when no response is received within the specified time
    }
  }

  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr joy_sub_;
  rclcpp::Client<service_package::srv::ServiceType>::SharedPtr client_;
};

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<JoystickReaderNode>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}

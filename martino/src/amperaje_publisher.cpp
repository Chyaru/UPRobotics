// this will be heald on the raspberry
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float32.hpp"

#include <string>
#include <memory>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <pigpiod_if2.h>
#include <stdio.h>
#include <functional>

using namespace std::chrono_literals;


int pi;
const int amp_pin=21;

float read_amp(){
    float cnt=0.0,average_amp=0.0;
    for(int i = 0; i < 300; i++){
        float x = gpioRead(pi, amp_pin);
        cnt += x;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    average_amp = cnt / 300.0;
    return (2.5 - (average_amp * (5.0 / 1024.0)) )/0.06511;
}

void timer_callback()
{
  static rclcpp::Node::SharedPtr node = rclcpp::Node::make_shared("amperaje_publisher");
  static rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr publisher = node->create_publisher<std_msgs::msg::Float32>("amperaje", 10);

  auto message = std_msgs::msg::Float32();
  message.data = read_amp;
  RCLCPP_INFO(node->get_logger(), "Amperaje: %f", read_amp);
  publisher->publish(message);
}

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::TimerBase::SharedPtr timer = rclcpp::create_timer(rclcpp::Duration(3s), timer_callback);
  rclcpp::spin(timer);
  rclcpp::shutdown();
  return 0;
}
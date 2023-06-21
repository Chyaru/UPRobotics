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
    for(int i = 0; i < 1; i++){
        float x = gpio_read(pi, amp_pin);
        cnt += x;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    average_amp = cnt; /// 300.0;
    return ((average_amp * (5.0 / 1023.0)) - 2.5 )/0.06511;
}

class MinimalPublisher : public rclcpp::Node
{
  public:
    MinimalPublisher()
    : Node("amperaje_publisher")
    {
      publisher_ = this->create_publisher<std_msgs::msg::Float32>("amperaje", 10);
      timer_ = this->create_wall_timer(
      500ms, std::bind(&MinimalPublisher::timer_callback, this));
    }

  private:
    void timer_callback()
    {
        auto message = std_msgs::msg::Float32();
        message.data = read_amp();
        
        RCLCPP_INFO(this->get_logger(), "Publishing: %f", message.data);
        publisher_->publish(message);
    }
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr publisher_;
};


int main(int argc, char * argv[])
{
	printf("Running pigpio program\n");
	pi = pigpio_start(nullptr, nullptr);
    if(pi<0){
        printf("failed");
        return 1;
    }
    set_mode(pi, amp_pin, PI_OUTPUT);
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalPublisher>());
  rclcpp::shutdown();
  return 0;
}

// this will be heald on the raspberry
#include "rclcpp/rclcpp.hpp"
#include "std_srvs/srv/set_bool.hpp"
#include "structures/srv/movement.hpp"

#include <memory>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <stdio.h>
#include <functional>
#include <stdlib.h>

using namespace std::chrono_literals;



class MinimalPublisher : public rclcpp::Node
{
  public:
    MinimalPublisher()
    : Node("check_connection_publisher")
    {
      timer_ = this->create_wall_timer(500ms, std::bind(&MinimalPublisher::timer_callback, this));
      client_ = this->create_client<std_srvs::srv::SetBool>("conexion");
      detencion = this->create_client<structures::srv::Movement>("direcciones");
    }

  private:
    void timer_callback()
    {
        /*
        auto request = std::make_shared<std_srvs::srv::Trigger::Request>();
        request->success = true;
        RCLCPP_INFO(this->get_logger(), "Checking conection");
        auto result = client_->assync_send_request(request);
        */
        RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "comprobando conexion");
        while (!client_->wait_for_service(3s)) {
            if (!rclcpp::ok()) {
                RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Interrupted while waiting for the service. Exiting.");
                exit(1);
            }
            RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "service not available, Stoping martino");
            auto request = std::make_shared<structures::srv::Movement::Request>();
            request->direction = 0;
            auto result = detencion->async_send_request(request);
        }

    }
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Client<std_srvs::srv::SetBool>::SharedPtr client_;
     rclcpp::Client<structures::srv::Movement>::SharedPtr detencion;
};


int main(int argc, char * argv[])
{
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Checking connection started" );  
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MinimalPublisher>());
  rclcpp::shutdown();
  return 0;
}

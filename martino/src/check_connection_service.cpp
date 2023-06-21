#include "rclcpp/rclcpp.hpp"
#include "std_srvs/srv/set_bool.hpp"



void add(const std::shared_ptr<std_srvs::srv::SetBool::Request> request,
          std::shared_ptr<std_srvs::srv::SetBool::Response>      response)
{
  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Connection established");            
 //  response->message="success"; 
}
int main(int argc, char **argv){

    rclcpp::init(argc, argv);
    std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("check_connection_service");
    
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Checking connection started" );  


    rclcpp::Service<std_srvs::srv::SetBool>::SharedPtr service =
    node->create_service<std_srvs::srv::SetBool>("conexion", &add);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
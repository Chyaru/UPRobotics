#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joy.hpp"
#include "structures/srv/movement.hpp"
#include<vector>


void joyCallback(const sensor_msgs::msg::Joy::SharedPtr joy){
  /*  int r = joy->buttons[4];
    if(r!=0){
        std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("reader_client");
        rclcpp::Client<structures::srv::Movement>::SharedPtr client =
        node->create_client<structures::srv::Movement>("direcciones");

        auto request = std::make_shared<structures::srv::Movement::Request>();
        request->direction = 1;


        auto result = client->async_send_request(request);
        if (rclcpp::spin_until_future_complete(node, result) ==
            rclcpp::FutureReturnCode::SUCCESS)
        {
            RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "speeds:"); // %s", result.get()->speeds);
        } else {
            RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Failed to call service direcciones");
        }
    }
*/

/*

        rclcpp::Client<structures::srv::Movement>::SharedPtr client_;
        client_ = rclcpp::create_client<structures::srv::Movement>("martino/server"); 
        auto future = client_->async_send_request(request);   
*/

        /* auto responseReceived = future.wait_for(std::chrono::milliseconds(200));
            if (responseReceived == std::future_status::ready) {
            auto response = future.get();
            RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "succesful");
            } else {
            RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "not succesful");
            }
        */
   

    std::vector<double> states(20);
   for(int i = 0; i < 8; i++){
        states[i]=joy->axes[i];
        if(i!=2 && i!=5 && states[i]!=0.0)
            RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "button %d is %f", i, states[i]);
    }
    
    /*
        axis 0 joystick de arriba horizontal
        axis 1 joystick de arriba vertical
        axis 2 RT
        axis 3 joystick de abajo horizontal
        axis 4 joystick de abajo vertical
        axis 5 LT
        axis 6 flechita horizontal
        axis 7 flechita vertical
        

        boton 0 es A
        boton 1 es B
        boton 2 es X
        boton 3 es Y
        boton 4 es LB
        boton 5 es RB
        boton 6 es doble cuadrito
        boton 7 es 3 lineas
        boton 8 es xbox
        boton 9 es presionar ruedita de arriba
        boton 10 es presionar ruedita de abajo
        boton 11 es captura

        */

    
    return;

}

int main(int argc, char ** argv){
    rclcpp::init(argc, argv);
    auto node = rclcpp::Node::make_shared("joystick_reader");
    auto joy_sub = node->create_subscription<sensor_msgs::msg::Joy>("/joy",10,joyCallback);

    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}

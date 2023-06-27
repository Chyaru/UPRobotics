#include "rclcpp/rclcpp.hpp"

#include "sensor_msgs/msg/joy.hpp"

#include "structures/srv/movement.hpp"

#include <vector>

#include <stdlib.h>



const double death_zone=10;

int last_r=-1;

int last_ac=-1;



std::shared_ptr<rclcpp::Node> node;

rclcpp::Client<structures::srv::Movement>::SharedPtr client;



void joyCallback(const sensor_msgs::msg::Joy::SharedPtr joy){

    std::vector<double> axis(8);

    std::vector<bool> botons(12);

    for(int i = 0; i < 8; i++) axis[i] = joy->axes[i];

    for(int i = 0; i < 12; i++) botons[i]=joy->buttons[i]; 

    /*

        axis 0 joystick de arriba horizontal

        axis 1 joystick de arriba vertical

        axis 2 LT

        axis 3 joystick de abajo horizontal

        axis 4 joystick de abajo vertical

        axis 5 RT

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



    int r=0;



    if(axis[0]<=1.0 && axis[0]>0.5) r = 4;

    else if(axis[0]>=-1.0 && axis[0]<-0.5) r = 3;

    else if(axis[1]<=1.0 && axis[1]>0.5) r = 1;

    else if(axis[1]>=-1.0 && axis[1]<-0.5) r = 2;

    else if(axis[3]==-1.0) r = 18; 

    else if(axis[3]==1.0) r = 17;

    else if(axis[4]==-1.0) r = 6;

    else if(axis[4]==1.0) r = 5;

    else if(axis[2]==-1.0) r = 14;

    else if(axis[6]==-1.0) r = 10;

    else if(axis[6]==1.0) r = 9;

    else if(axis[7]==-1.0) r = 8;

    else if(axis[7]==1.0) r = 7;

    else if(botons[0]) r = 16;

    else if(botons[1]) r = 0;

    else if(botons[2]) r = 13;

    else if(botons[3]) r = 15;

    else if(botons[4]) r = 12;

    else if(botons[5]) r = 11;

    else if(botons[6]) r = 0;

    else if(botons[7]) r = 0;

    else if(botons[8]) r = -1;

    else if(botons[9]) r = 0;

    else if(botons[10]) r = 0;

    else if(botons[11]) r = 0;

    

    

    if(last_r!=r) {

	    

		auto request = std::make_shared<structures::srv::Movement::Request>();

	 	request->direction = r;



	    

	    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "sending: [%d]", r);

	    

	    

	    auto result = client->async_send_request(request);

	    if (rclcpp::spin_until_future_complete(node, result) ==

		rclcpp::FutureReturnCode::SUCCESS)

	    {

		RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "speeds: %d", r); // %s", result.get()->speeds);

	    } else {

		RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Failed to call service direcciones");

	    }

	    last_r = r;

    }

    int ac = -(axis[5]-1.0)*20.0;

    if(last_ac != ac && ac>death_zone && r>=1 && r<=4){



	auto request = std::make_shared<structures::srv::Movement::Request>();

 	RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "sending: [%d]", 100 + ac);

        request->direction = 100 + ac;



        auto result = client->async_send_request(request);

        

        if (rclcpp::spin_until_future_complete(node, result) ==

            rclcpp::FutureReturnCode::SUCCESS)

        {

            RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "speeds:"); // %s", result.get()->speeds);

        } else {

            RCLCPP_ERROR(rclcpp::get_logger("rclcpp"), "Failed to call service direcciones");

        }

        last_ac = ac;

    }   





    

    return;



}



int main(int argc, char ** argv){

    rclcpp::init(argc, argv);

    auto nodes = rclcpp::Node::make_shared("joystick_reader");

    auto joy_sub = nodes->create_subscription<sensor_msgs::msg::Joy>("/joy",10,joyCallback);

	node = rclcpp::Node::make_shared("reader_client");

	client = node->create_client<structures::srv::Movement>("direcciones");	

    rclcpp::spin(nodes);

    rclcpp::shutdown();

    return 0;

}

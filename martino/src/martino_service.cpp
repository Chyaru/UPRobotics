#include "rclcpp/rclcpp.hpp"
#include "structures/srv/movement.hpp"
#include <string>
#include <memory>



#include <unistd.h>
#include <pigpiod_if2.h>
#include <algorithm>
#include <stdio.h>

int pi;
// CHANGE THIS!!!
const int motors_pins[4]={2,1,3,4}; // *
const int max_speed_forward = 53; // *
const int max_speed_backwards = 35; // *
const int speed_jump=1; // *
const int starting_velocity_forward = 45; // *
const int starting_velocity_backward = 37; // *


const int to_standar = 90;

int current_direction = 0;/*
   0 not moving
   1 moving forward
   2 moving backward
   3 turning right
   4 turning left
*/
int current_speed[4] = {to_standar,to_standar,to_standar,to_standar}; /* 
    4 motors
    seen from front
    0 index the right motor
    1 index the front motor
    2 index the left motor
    3 index the back motor
*/

int states[5][2]{
    to_standar, to_standar,
    1,  0, // move the robot forward
    0, 1, // move the robot backward
    1,  1, // turning right
    0, 0 // turning left
};


void set_speed(int motor_to_move, int speed_to_move){
   set_PWM_dutycycle(pi, motors_pins[motor_to_move], speed_to_move);
    current_speed[motor_to_move]=speed_to_move;
    return;
}



void move_forward(int motor_to_move){
    set_speed(motor_to_move, std::min(max_speed_forward, current_speed[motor_to_move] + speed_jump) );
    return;
}

void move_backwards(int motor_to_move){
    set_speed(motor_to_move, std::max(max_speed_backwards, current_speed[motor_to_move] - speed_jump) );
    return;
}


void stop_motors(){
    current_direction = 0;
    for(int i = 0; i < 4; i++) set_speed(i,to_standar);
    return;
}

void moving(int desired_direction){
    if(desired_direction!=current_direction){
        stop_motors();
        if(states[desired_direction][0]) set_speed(0, starting_velocity_forward);
        else set_speed(0, starting_velocity_backward);
        if(states[desired_direction][1]) set_speed(2, starting_velocity_forward);
        else set_speed(2, starting_velocity_backward);
    }else{
        if(states[desired_direction][0]) move_forward(0);
        else move_backwards(0);
        if(states[desired_direction][1]) move_forward(2);
        else move_backwards(2);
    }
    current_direction = desired_direction;
    return;
}


// CHECK THIS, forward or backward !!!
void elevating_front_flipper(){ 
    stop_motors();
    set_speed(1, starting_velocity_forward);
    return;
}

void desending_front_flipper(){
    stop_motors();
    set_speed(1, starting_velocity_backward);
    return;
}
void elevating_back_flipper(){
    stop_motors();
    set_speed(3, starting_velocity_backward);
    return;
}
void desending_back_flipper(){ 
    stop_motors();
    set_speed(3, starting_velocity_forward);
    return;
}

void print_velocities(){
    printf("this is the speed of the right motor: %d\n", current_speed[0]);
    printf("this is the speed of the left motor: %d\n", current_speed[2]);
    printf("this is the speed of the front motor: %d\n", current_speed[1]);
    printf("this is the speed of the back motor: %d\n\n", current_speed[3]);
    return;
}
// CLAW CODE COMING SOON


// this is supossed to be the subscriber function
/*  SUBSCRIBER WILL RECIBE THESE VALUES
    -1 exit
    0 stop  
    1 move forward
    2 move backward
    3 turning right
    4 turning left
    5 elevate front flipper
    6 desend front flipper
    7 elevate back flipper
    8 desend back flipper
*/
void subscriber_function(int instruction){


    if(instruction==-1) return;
    
    
   set_PWM_dutycycle(pi, 2, instruction);
    
    print_velocities();
    
    return;

    if(instruction==0) stop_motors();
    else if(instruction==5) elevating_front_flipper();
    else if(instruction==6) desending_front_flipper();
    else if(instruction==7) elevating_back_flipper();
    else if(instruction==8) desending_back_flipper();
    else moving(instruction);

    return;
}



void add(const std::shared_ptr<structures::srv::Movement::Request> request,
          std::shared_ptr<structures::srv::Movement::Response>      response)
{
  //  stop_motors();
  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Incoming request\ndirection: %ld",
                request->direction);
   subscriber_function(request->direction);             
   
  response->speeds = "a";
                
  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "sending back response: [%s]", response->speeds);
}

int main(int argc, char **argv)
{
	printf("Running pigpio program\n");
	pi = pigpio_start(nullptr, nullptr);
    if(pi<0){
        printf("failed");
        return 1;
    }
 // you MUST initialize the library !
	//for(int i = 0; i < 4; i++) set_mode(pi, motors_pins[i], PI_OUTPUT); // setting motors pins
	set_mode(pi, 2, PI_OUTPUT);
  rclcpp::init(argc, argv);

  std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("martino_server");
    
  rclcpp::Service<structures::srv::Movement>::SharedPtr service =
    node->create_service<structures::srv::Movement>("direcciones", &add);

  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Ready to move the robots");
  
  printf("gpioTerminate()...\n");
  
  rclcpp::spin(node);
  rclcpp::shutdown();
 // pigpio_stop(pi);
}

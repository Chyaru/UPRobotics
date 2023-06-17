#include "rclcpp/rclcpp.hpp"
#include "structures/srv/movement.hpp"
#include <string>
#include <memory>


#include <chrono>
#include <thread>
#include <unistd.h>
#include <pigpiod_if2.h>
#include <algorithm>
#include <stdio.h>

int pi;
// CHANGE THIS!!!
const int motors_pins[7]={2,3,4,17,27,22,10}; /*
    4 motors
    seen from front
    0 index the right motor
    1 index the left motor
    2 index the front motor
    3 index the back motor
    4 index the motor that rotates the arm
    5 index muneca
    6 index clawn
*/
const int max_speed_forward = 51; // *
const int max_speed_backwards = 27; // *
const int speed_jump=1; // *
const int starting_velocity_forward[7] = {42,42,43,43,42,40,40}; // *
const int starting_velocity_backward[7] = {35,35,34,34,36,37,37}; // *
const int to_standar = 90;

int current_speed[7] = {to_standar,to_standar,to_standar,to_standar,to_standar,to_standar,to_standar}; 

int current_direction = 0;/*
   0 not moving
   1 moving forward
   2 moving backward
   3 turning right
   4 turning left
*/
int states[5][2]{
    to_standar, to_standar,
    1,  1, // move the robot forward
    0, 0, // move the robot backward
    1,  0, // turning right
    0, 1 // turning left
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
    for(int i = 0; i < 7; i++) set_speed(i,to_standar);
    return;
}

void moving(int desired_direction){
    if(desired_direction!=current_direction){
        stop_motors();
        set_speed(0, states[desired_direction][0]?starting_velocity_forward[0]:starting_velocity_backward[0]);
        set_speed(1, states[desired_direction][1]?starting_velocity_forward[1]:starting_velocity_backward[1]);
    }else{
        if(states[desired_direction][0]) move_forward(0);
        else move_backwards(0);
        if(states[desired_direction][1]) move_forward(1);
        else move_backwards(1);
    }
    current_direction = desired_direction;
    return;
}


// CHECK THIS, forward or backward !!!
void flipper(int desired_direction){
    stop_motors();
    std::this_thread::sleep_for(std::chrono::milliseconds(40));
    int motor_to_move = (desired_direction/2)+2;
    set_speed(motor_to_move, ((desired_direction&1) ? starting_velocity_forward[motor_to_move]: starting_velocity_backward[motor_to_move]));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    stop_motors();

}

// CLAW CODE COMING SOON
void garra(int desired_direction){
    stop_motors();
    std::this_thread::sleep_for(std::chrono::milliseconds(40));
    int motor_to_move = (desired_direction/2)+4;
    set_speed(motor_to_move, ((desired_direction&1) ? starting_velocity_forward[motor_to_move]: starting_velocity_forward[motor_to_move]));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    stop_motors();
}




const int standar_speed_actuators=200;
const int actuators_pins[2][2]={6,13,19,26};
/*
    0 bottom actuator
    1 above actuator
*/
void actuators(int desired_direction){
    int actuator_i = desired_direction/2;
    RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "%d %d ", actuators_pins[actuator_i][0], actuators_pins[actuator_i][1]);
    gpio_write(pi, actuators_pins[actuator_i][1],(desired_direction&1));
    gpio_write(pi, actuators_pins[actuator_i][2],!(desired_direction&1));
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    gpio_write(pi, actuators_pins[actuator_i][1],0);
    gpio_write(pi, actuators_pins[actuator_i][2],0);
}

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
    9 rotate right arm          0
    10 rotate left arm          1
    11 wrap clawn up            2
    12 wrap clawn down          3 
    13 open clawn               4
    14 close clawn              5
    15 elevate bottom actuator  0
    16 desend bottom actuator   1
    17 elevate above actuator   2
    18 desend bottom actuator   3
*/
void subscriber_function(int instruction){


    if(instruction==-1) {
        printf("gpioTerminate()...\n");
        pigpio_stop(pi);
        return;
    }
    
    if(instruction==0) stop_motors();
    else if(instruction<=4) moving(instruction);
    else if(instruction<=8) flipper(instruction-5);
    else if(instruction<=14) garra(instruction-9);
    else if(instruction<=18) actuators(instruction-15);
    else if(instruction>100) 
    return;
     
    set_PWM_dutycycle(pi, 2, instruction);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    stop_motors();
    
    return;
}


void add(const std::shared_ptr<structures::srv::Movement::Request> request,
          std::shared_ptr<structures::srv::Movement::Response>      response)
{
  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Incoming request\ndirection: %ld",
                request->direction);
   subscriber_function(request->direction);             
   response->s0=current_speed[0]; 
   response->s1=current_speed[1]; 
   response->s2=current_speed[2]; 
   response->s3=current_speed[3]; 
   response->s4=current_speed[4]; 
   response->s5=current_speed[5]; 
   response->s6=current_speed[6]; 
   response->s7=0; 
   response->s8=0; 
  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "sending back response: [%d] [%d] [%d] [%d] [%d] [%d] [%d] [%d] [%d]", response->s0, response->s1, response->s2, response->s3, response->s4, response->s5, response->s6, response->s7, response->s8);
  
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
	for(int i = 0; i < 7; i++) set_mode(pi, motors_pins[i], PI_OUTPUT); // setting motors pins
	for(int i = 0; i < 2; i++) for(int j = 0; j < 2; j++) set_mode(pi, actuators_pins[i][j], PI_OUTPUT);
  rclcpp::init(argc, argv);

  std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("martino_server");
    
  rclcpp::Service<structures::srv::Movement>::SharedPtr service =
    node->create_service<structures::srv::Movement>("direcciones", &add);

  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Ready to move the robots");
  
  
  rclcpp::spin(node);
  rclcpp::shutdown();
}

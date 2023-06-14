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
const int motors_pins[7]={1,2,3,4,5,6,7}; /*
    4 motors
    seen from front
    0 index the right motor
    1 index the front motor
    2 index the left motor
    3 index the back motor
    4 index the motor that rotates the arm
    5 index muneca
    6 index clawn
*/
const int max_speed_forward = 53; // *
const int max_speed_backwards = 35; // *
const int speed_jump=1; // *
const int starting_velocity_forward = 46; // *
const int starting_velocity_backward = 35; // *
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
    for(int i = 0; i < 7; i++) set_speed(i,to_standar);
    return;
}

void moving(int desired_direction){
    if(desired_direction!=current_direction){
        stop_motors();
        set_speed(0, states[desired_direction][0]?starting_velocity_forward:starting_velocity_backward);
        set_speed(2, states[desired_direction][1]?starting_velocity_forward:starting_velocity_backward);
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
void flipper(int desired_direction){
    set_speed((desired_direction/2)+2, ((desired_direction&1) ? starting_velocity_forward: starting_velocity_backward));
}

void garra(int desired_direction){
    set_speed((desired_direction/2)+4, ((desired_direction&1) ? starting_velocity_forward: starting_velocity_forward));
}

void print_velocities(std::string &s){
    s = ""; 
    for(int i = 0; i < 7; i++){
        s += "Velocidad: ";
        if(current_speed[i]!=90) s += "on\n";
        else s += "off\n";
    }
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
    9 rotate right arm
    10 rotate left arm
    11 wrap clawn up
    12 wrap clawn down
    13 close clawn
    14 open clawn
*/
void subscriber_function(int instruction, std::string &velocyties){
    if(instruction==-1) {
        pigpio_stop(pi);
        return;
    }
    if(instruction==0) stop_motors();
    else if(instruction<=4) moving(instruction);
    else if(instruction<=8) flipper(instruction-5);
    else if(instruction<=14) garra(instruction-9);
    print_velocities(velocyties);
    return;
}



void add(const std::shared_ptr<structures::srv::Movement::Request> request,
          std::shared_ptr<structures::srv::Movement::Response>      response)
{
  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Incoming request\ndirection: %ld",
                request->direction);
   subscriber_function(request->direction, response->speeds);             
                
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
}
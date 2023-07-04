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
const int motors_pins[7]={3,4,17,27,22,23,24}; /*
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
const int starting_velocity_forward[7] = {170,170,160,160,188,188,185}; // *
const int starting_velocity_backward[7] = {215,215,220,220,200,200,203}; // *
const int to_standar = 100;

int current_speed[9] = {to_standar,to_standar,to_standar,to_standar,to_standar,to_standar,to_standar, 0, 0}; 
const int actuators_pins[2][3]={13,19,26,16,20,21};

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

void speed_dir(int motor_to_move, int speed_to_move, int &vel){

    if(vel==to_standar) vel = (speed_to_move>=200 ? 198: 192);

    if(vel>=198 && speed_to_move<vel){ // decrementando derecha

 	if(speed_to_move==to_standar) speed_to_move = 198;

    vel -= std::max((vel-198)/2,1);

    vel = std::max(vel, speed_to_move);

    printf("vel: %d\n", vel);

        set_PWM_dutycycle(pi, motors_pins[motor_to_move], vel);

    std::this_thread::sleep_for(std::chrono::milliseconds(delay));

    }else if(vel>=198 && speed_to_move>vel){ // incrementando derecha

	    vel += std::max((vel-198)/2,1);

	    vel = std::min(vel, speed_to_move);

	    printf("vel: %d\n", vel);

    	    set_PWM_dutycycle(pi, motors_pins[motor_to_move], vel);

	    std::this_thread::sleep_for(std::chrono::milliseconds(delay));

    }else if(speed_to_move<vel && speed_to_move!=to_standar){ // incrementando izquierda

	    vel -= std::max((192-vel)/2,1);

	    vel = std::max(vel, speed_to_move);

	    printf("vel: %d\n", vel);

    	    set_PWM_dutycycle(pi, motors_pins[motor_to_move], vel);

	    std::this_thread::sleep_for(std::chrono::milliseconds(delay));


    }else{ // decrementando izquierda

	    if(speed_to_move==to_standar) speed_to_move = 192;

	    vel += std::max((192-vel)/2,1);

	    vel = std::min(vel, speed_to_move);

	    printf("vel: %d\n", vel);

    	    set_PWM_dutycycle(pi, motors_pins[motor_to_move], vel);

	    std::this_thread::sleep_for(std::chrono::milliseconds(delay));

    	}

    }

    

    if(vel==198 || vel==192){

    	vel = to_standar;

	    printf("vel: %d\n", vel);

    	 set_PWM_dutycycle(pi, motors_pins[motor_to_move], vel);

    }

    return;

}

void set_speed(int motor_to_move, int speed_to_move){
    int &vel = current_speed[motor_to_move];
    while(vel!=speed_to_move) {
        speed_dir(motor_to_move, speed_to_move, vel);
    }
    return;
}


void stop_motors(){
    current_direction = 0;
    int &vel = current_speed[0];
    int &vel1 = current_speed[1];
    while(vel!=to_standar || vel1!=to_standar) {
        if(vel!=to_standar) speed_dir(to_standar, vel);
        if(vel1!=to_standar) speed_dir(to_standar, vel1); 
    }
    for(int i = 2; i < 7; i++) set_speed(i,to_standar);
    for(int i = 0; i < 2; i++) for(int j = 0; j < 2; j++){ set_PWM_dutycycle(pi, actuators_pins[i][j],0);  current_speed[i+8]=0;}
    return;
}
void moving(int x){
    int &vel1 = current_speed[0];
    int &vel2 = current_speed[1];
    int desired_speed1 = (states[current_direction][0] ? 200 + x : 188 - x);
    int desired_speed2 = (states[current_direction][1] ? 200 + x : 188 - x);
    while(vel1!=desired_speed1 || vel2!=desired_speed2){
        if(vel1!=desired_speed1) speed_dir(0, desired_speed1, vel1);
        if(vel2!=desired_speed2) speed_dir(1, desired_speed2, vel2);
    }
    return;
}


// CHECK THIS, forward or backward !!!
void flipper(int desired_direction){
    int motor_to_move = (desired_direction/2)+2;
    set_speed(motor_to_move, ((desired_direction&1) ? starting_velocity_forward[motor_to_move]: starting_velocity_backward[motor_to_move]));

}

// CLAW CODE COMING SOON
void garra(int desired_direction){
    int motor_to_move = (desired_direction/2)+4;
    set_speed(motor_to_move, ((desired_direction&1) ? starting_velocity_forward[motor_to_move]: starting_velocity_backward[motor_to_move]));
}




const int standar_speed_actuators=130;
/*
    0 bottom actuator
    1 above actuator
*/
void actuators(int desired_direction){
    int actuator_i = desired_direction/2;
    set_PWM_dutycycle(pi, actuators_pins[actuator_i][0], (desired_direction&1) ? standar_speed_actuators : 0);
    set_PWM_dutycycle(pi, actuators_pins[actuator_i][1], (desired_direction&1) ? 0: standar_speed_actuators);
    current_speed[8+actuator_i]=1;
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
   //     set_PWM_dutycycle(pi, 3, instruction);
    
    
  //  return;
   //  set_speed(0, instruction);

    if(instruction==0) stop_motors();
    else if(instruction<=4) current_direction=instruction;
    else if(instruction<=8) flipper(instruction-5);
    else if(instruction<=14) garra(instruction-9);
    else if(instruction<=18) actuators(instruction-15);
    else if(instruction>100) moving(instruction-100);
    
    
    
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
   response->s7=current_speed[7]; 
   response->s8=current_speed[8]; 
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
	for(int i = 0; i < 7; i++) set_mode(pi, motors_pins[i], PI_OUTPUT);
	for(int i = 0; i < 7; i++) set_PWM_frequency(pi, motors_pins[i], 5000);  // setting motors pins
	for(int i = 0; i < 2; i++) for(int j = 0; j < 3; j++)  set_mode(pi, actuators_pins[i][j], PI_OUTPUT); 
	gpio_write(pi, actuators_pins[0][2], 1);
  rclcpp::init(argc, argv);

  std::shared_ptr<rclcpp::Node> node = rclcpp::Node::make_shared("martino_server");
    
  rclcpp::Service<structures::srv::Movement>::SharedPtr service =
    node->create_service<structures::srv::Movement>("direcciones", &add);

  RCLCPP_INFO(rclcpp::get_logger("rclcpp"), "Ready to move the robots");
  
  
  rclcpp::spin(node);
  rclcpp::shutdown();
}

# UPRobotis
ROS UPRobotics
We're a mexican team which is developing a rescue robot, in order to participate in the Robocup Rescue League. 

# RASPBERRY PI 4 AND ROS2 ACCESS TO PINS ON UBUNTU MATE 22.04

- Download pigpio library in ros2_ws/src: http://abyz.me.uk/rpi/pigpio/
- Make sure your CMakeList contains the target_link_libraries(<name> pigpiod_if2) 
- sudo pigpiod before running your code

 My code is very simple, the importants parts you may want to look at is in the main and in the set_speed function

  #include <pigpiod_if2.h>
  
  pi = pigpio_start(nullptr, nullptr);
  set_mode(pi, PIN_YOU_WANT, PI_OUTPUT);
  set_PWM_dutycycle(pi, PIN_YOU_WANT, speed_to_move);

# UPRobotis
ROS UPRobotics
We're a mexican team which is developing a rescue robot, in order to participate in the Robocup Rescue League. 

Let me define certain thigs, there are 2 projects, The one in the raspberry pi, and the other one in the Virtual Machine, CMakeList doesn't need to difeer, but there are 2 in the martino's carpet. If you want to test it, then keep the one with the VM, erase VM in the name.
I love C++ so I'm trying to code everything in this languaje. 

# RASPBERRY PI 4 AND ROS2 HUMBLE ACCESS TO PINS ON UBUNTU MATE 22.04 C++

- Download pigpio library in ros2_ws/src: http://abyz.me.uk/rpi/pigpio/
- Make sure your CMakeList contains the target_link_libraries(<name> pigpiod_if2) 
- $ sudo pigpiod before running your code

 My code is very simple, the importants parts you may want to look at is in the main and in the set_speed function

  #include <pigpiod_if2.h>
  
  pi = pigpio_start(nullptr, nullptr);
 
  set_mode(pi, PIN_YOU_WANT, PI_OUTPUT);
 
  set_PWM_dutycycle(pi, PIN_YOU_WANT, speed_to_move);

 
 # READ XBOX CONTROLLER INPUT ON C++ AND SEND IT TO THE SERVER C++
 
 
  I was doing the client in a single code. But it's better to make a subscriber which calls to the server. In this case I followed these steps.
 
 $ apt-get install ros-humble-teleop-twist-joy
 
 $ ls /dev/input/ 
 
 In order to identify your X (in jsX) use this $ sudo jstest /dev/input/jsX test and identify
 
 $ sudo chmod a+rw /dev/input/jsX
 
 You will have to run this every time you want to use the xbox controller
 
 $ ros2 run joy joy_node
 
 try if it's working correctly
 
 $ ros2 topic echo joy
 
 The values will change when you press the buttons.
 
 
 Let's create a subscriber 
 
 auto joy_sub = node->create_subscription<sensor_msgs::msg::Joy>("/joy",10,joyCallback);
 
 /joy is the package mentioned before
 
 void joyCallback(const sensor_msgs::msg::Joy::SharedPtr joy)
 
 don't forget to run $ ros2 run joy joy_node
 
 Now we'll want to fuse the pin's access and the xbox controller input. You will read that in xbox_reader. I define a call to the service once I read the input. 
 
 
 
 
 

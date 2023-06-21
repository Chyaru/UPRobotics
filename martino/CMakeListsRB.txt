cmake_minimum_required(VERSION 3.8)
project(martino)


# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(structures REQUIRED)
find_package(std_srvs REQUIRED)

add_executable(server src/martino_service.cpp)
target_link_libraries(server pigpiod_if2)
ament_target_dependencies(server rclcpp structures)

add_executable(client src/martino_client.cpp)
ament_target_dependencies(client rclcpp structures)


add_executable(amp_pub src/amperaje_publisher.cpp)
target_link_libraries(amp_pub pigpiod_if2)
ament_target_dependencies(amp_pub rclcpp std_msgs)

add_executable(amp_sub src/amperaje_subscriber.cpp)
ament_target_dependencies(amp_sub rclcpp std_msgs)

add_executable(con_pub src/check_connection_publisher.cpp)
ament_target_dependencies(con_pub rclcpp std_srvs)
ament_target_dependencies(con_pub rclcpp structures)


add_executable(con_ser src/check_connection_service.cpp)
ament_target_dependencies(con_ser rclcpp std_srvs)  

install(TARGETS
    server
    client
    amp_pub
    amp_sub
    con_ser
    con_pub
  DESTINATION lib/${PROJECT_NAME})


ament_package()

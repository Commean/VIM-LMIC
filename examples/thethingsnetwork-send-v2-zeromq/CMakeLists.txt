cmake_minimum_required(VERSION 3.15)

project(thethingsnetwork-send-v2-zeromq)
add_definitions(-DARDUINO_LMIC_PROJECT_CONFIG_H=${CMAKE_CURRENT_SOURCE_DIR}/my_project_config.h -DVIM3=1)

find_package(cppzmq)
add_executable(${CMAKE_PROJECT_NAME} thethingsnetwork-send-v2-zeromq.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} lmic wiringPi pthread rt m crypt cppzmq)

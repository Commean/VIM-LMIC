cmake_minimum_required(VERSION 3.15)

project(thethingsnetwork-send-v1)
add_definitions(-DARDUINO_LMIC_PROJECT_CONFIG_H=${CMAKE_CURRENT_SOURCE_DIR}/my_project_config.h -DVIM3=1)

add_executable(${CMAKE_PROJECT_NAME} thethingsnetwork-send-v1.cpp)
target_link_libraries(${CMAKE_PROJECT_NAME} lmic wiringPi pthread rt m crypt)

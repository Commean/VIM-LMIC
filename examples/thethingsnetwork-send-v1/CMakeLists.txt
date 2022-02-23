cmake_minimum_required(VERSION 3.15)

project(myapp)
add_definitions(-DARDUINO_LMIC_PROJECT_CONFIG_H=${CMAKE_CURRENT_SOURCE_DIR}/my_project_config.h -DVIM3=1)

add_executable(myexample thethingsnetwork-send-v1.cpp)
target_link_libraries(myexample lmic wiringPi pthread rt m crypt)

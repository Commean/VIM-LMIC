cmake_minimum_required(VERSION 3.15)

project(myapp)
add_definitions (-DARDUINO_LMIC_PROJECT_CONFIG_H=${CMAKE_CURRENT_SOURCE_DIR}/my_project_config.h -DVIM3=1)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lwiringPi -lpthread -lrt -lm -lcrypt")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D ARDUINO_LMIC_PROJECT_CONFIG_H_SUPPRESS -D LMIC_DEBUG_LEVEL=1 -D CFG_eu868=1 -D CFG_sx1276_radio=1")

#add_library(lmic SHARED IMPORTED) # or STATIC instead of SHARED

add_executable(myexample thethingsnetwork-send-v1.cpp)
target_link_libraries(myexample lmic wiringPi pthread rt m crypt) # also adds the required include path

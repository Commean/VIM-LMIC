cmake_minimum_required(VERSION 3.15)

project(lmic VERSION 1.0.0)
add_definitions(-DARDUINO_LMIC_PROJECT_CONFIG_H=../project_config/lmic_project_config.h -DVIM3=1)

add_library(lmic SHARED
    arduino_lmic.h
    arduino_lmic_hal_boards.h
    arduino_lmic_hal_configuration.h
    arduino_lmic_lorawan_compliance.h
    arduino_lmic_user_configuration.h
    lmic.h

    aes/ideetron/AES-128_V10.cpp
    aes/lmic.c
    aes/other.c

    hal/getpinmap_catena4420.cpp
    hal/getpinmap_catena4551.cpp
    hal/getpinmap_catena4610.cpp
    hal/getpinmap_catena4611.cpp
    hal/getpinmap_catena4612.cpp
    hal/getpinmap_catena4617.cpp
    hal/getpinmap_catena4618.cpp
    hal/getpinmap_catena4630.cpp
    hal/getpinmap_catena4801.cpp
    hal/getpinmap_catena4802.cpp
    hal/getpinmap_disco_l072cs_lrwan1.cpp
    hal/getpinmap_feather32u4lora.cpp
    hal/getpinmap_featherm0lora.cpp
    hal/getpinmap_heltec_lora32.cpp
    hal/getpinmap_thisboard.cpp
    hal/getpinmap_ttgo_lora32_v1.cpp
    hal/getpinmap_ttgo_lora32_v2.1.cpp
    hal/hal.cpp
    hal/hal.h

    lmic/config.h
    lmic/hal.h
    lmic/lmic_as923.c
    lmic/lmic_au915.c
    lmic/lmic_bandplan_as923.h
    lmic/lmic_bandplan_au915.h
    lmic/lmic_bandplan_eu868.h
    lmic/lmic_bandplan.h
    lmic/lmic_bandplan_in866.h
    lmic/lmic_bandplan_kr920.h
    lmic/lmic_bandplan_us915.h
    lmic/lmic.c
    lmic/lmic_channelshuffle.c
    lmic/lmic_compat.h
    lmic/lmic_compliance.c
    lmic/lmic_compliance.h
    lmic/lmic_config_preconditions.h
    lmic/lmic_env.h
    lmic/lmic_eu868.c
    lmic/lmic_eu_like.c
    lmic/lmic_eu_like.h
    lmic/lmic.h
    lmic/lmic_in866.c
    lmic/lmic_kr920.c
    lmic/lmic_us915.c
    lmic/lmic_us_like.c
    lmic/lmic_us_like.h
    lmic/lmic_util.c
    lmic/lmic_util.h
    lmic/lorabase_as923.h
    lmic/lorabase_au915.h
    lmic/lorabase_eu868.h
    lmic/lorabase.h
    lmic/lorabase_in866.h
    lmic/lorabase_kr920.h
    lmic/lorabase_us915.h
    lmic/lorawan_spec_compliance.h
    lmic/oslmic.c
    lmic/oslmic.h
    lmic/oslmic_types.h
    lmic/radio.c
)

include_directories(aes hal lmic ../project_config)
target_link_libraries(lmic wiringPi pthread rt m crypt) # also adds the required include path
set_target_properties(lmic PROPERTIES VERSION ${PROJECT_VERSION})

install(TARGETS lmic DESTINATION lib)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/" # source directory
    DESTINATION "include" # target directory
    FILES_MATCHING # install only matched files
    PATTERN "*.h" # select header files
)

project(InterfaceLEDStrip)
cmake_minimum_required(VERSION 2.8)
#aux_source_directory(. SRC_LIST)
#add_executable(${PROJECT_NAME} ${SRC_LIST})

include_directories(.)

add_executable(arduino_led_interface
        arduino.cpp
        main.cpp
        RS232.cpp
)

add_executable(test_arduino_led_interface
        arduino.cpp
        test_interface_main.cpp
        RS232.cpp
)

target_link_libraries(arduino_led_interface
        kogmo_rtdb
        rt
        ArduinoData
)

target_link_libraries(test_arduino_led_interface
        kogmo_rtdb
        rt
        ArduinoData
)

project(PatternGenerator)
cmake_minimum_required(VERSION 2.8)
#aux_source_directory(. SRC_LIST)
#add_executable(${PROJECT_NAME} ${SRC_LIST})

include_directories(.)

add_executable(pattern_generator
        PatternGenerator.cpp
        main.cpp
)

target_link_libraries(pattern_generator
        kogmo_rtdb
        rt
        ArduinoData
)

add_executable(LedBatteryStatePublisher
        PatternGenerator.cpp
        main_battery_state.cpp
)

target_link_libraries(LedBatteryStatePublisher
        kogmo_rtdb
        rt
        ArduinoData
        MultiPlusData
)

add_executable(LedMovementStatePublisher
        PatternGenerator.cpp
        main_movement_state.cpp
)

target_link_libraries(LedMovementStatePublisher
        kogmo_rtdb
        rt
        ArduinoData
)

add_executable(LedLaserPublisher
        PatternGenerator.cpp
        main_laser.cpp
)

target_link_libraries(LedLaserPublisher
        kogmo_rtdb
        rt
        ArduinoData
        Sensor
        ticpp
        KinematicTypes
)

add_executable(LedEmergencyStatePublisher
        PatternGenerator.cpp
        main_emergency_state.cpp
)

target_link_libraries(LedEmergencyStatePublisher
        kogmo_rtdb
        rt
        ArduinoData
)

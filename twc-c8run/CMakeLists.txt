find_package(SDL2 REQUIRED)

include_directories(twc-c8run
        ${SDL2_INCLUDE_DIRS})

add_executable(twc-c8run
        src/audio.cpp
        src/display.cpp
        src/event_manager.cpp
        src/main.cpp
        src/timer.cpp)

target_link_libraries(twc-c8run
        ${SDL2_LIBRARIES}
        libtwc-chip8)
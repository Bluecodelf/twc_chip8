add_executable(twc-c8asm
        src/main.cpp)

target_link_libraries(twc-c8asm
        libtwc-chip8)

install(TARGETS twc-c8asm DESTINATION bin)
add_library(libtwc-chip8
        src/cpu.cpp
        src/instruction.cpp)

target_include_directories(libtwc-chip8 PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)
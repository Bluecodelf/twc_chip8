cmake_minimum_required(VERSION 3.7)
project(twc_chip8)

find_package(Boost 1.54.0 REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(libtwc-chip8)
add_subdirectory(twc-c8run)
add_subdirectory(twc-c8asm)
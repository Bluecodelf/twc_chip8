add_library(libtwc-chip8
        src/constants.cpp
        src/cpu.cpp
        src/instruction.cpp
        src/program.cpp
        src/parsing/lexer.cpp
        src/parsing/parser.cpp
        src/parsing/tokenizer.cpp)

target_include_directories(libtwc-chip8 PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE src)
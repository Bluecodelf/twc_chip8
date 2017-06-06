add_executable(twc-c8asm
        src/main.cpp
        src/parser.cpp
        src/lexer.cpp
        src/tokenizer.cpp)

target_link_libraries(twc-c8asm
        libtwc-chip8)
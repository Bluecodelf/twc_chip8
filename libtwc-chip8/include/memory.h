//
// Created by Laurent Fourrier on 2017-05-16.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#ifndef TWC_CHIP8_MEMORY_H
#define TWC_CHIP8_MEMORY_H

#include <cstdint>

#define C8_REGISTER_COUNT 16
#define C8_STACK_SIZE 16
#define C8_MEMORY_SIZE 0x1000

namespace c8 {
    union memory {
        struct interpreter {
            std::uint8_t v[C8_REGISTER_COUNT];
            std::uint8_t dt, st, sp;
            std::uint16_t i, pc;
            std::uint16_t stack[C8_STACK_SIZE];
            std::uint8_t font[80];
        } interpreter;
        std::uint8_t memory[C8_MEMORY_SIZE];
    };
}

#endif //TWC_CHIP8_MEMORY_H

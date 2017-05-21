//
// Created by Laurent Fourrier on 2017-05-16.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#ifndef TWC_CHIP8_MEMORY_H
#define TWC_CHIP8_MEMORY_H

#include <cstdint>

#define REGISTER_COUNT 16
#define STACK_SIZE 16
#define MEMORY_SIZE 0x1000

namespace Chip8
{
    union Memory
    {
        struct Interpreter
        {
            std::uint8_t v[REGISTER_COUNT];
            std::uint8_t dt, st, sp;
            std::uint16_t i, pc;
            std::uint16_t stack[STACK_SIZE];
            std::uint8_t font[80];
        } interpreter;
        std::uint8_t memory[MEMORY_SIZE];
    };
}

#endif //TWC_CHIP8_MEMORY_H

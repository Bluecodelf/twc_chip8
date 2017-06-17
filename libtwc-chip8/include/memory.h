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
    using vm_address = std::uint16_t;
    using vm_register = std::uint8_t;
    using vm_byte = std::uint8_t;

    union memory {
        struct interpreter {
            vm_register v[C8_REGISTER_COUNT];
            vm_register dt, st, sp;
            vm_address i, pc;
            vm_address stack[C8_STACK_SIZE];
            vm_byte font[80];
        } interpreter;
        vm_byte memory[C8_MEMORY_SIZE];
    };
}

#endif //TWC_CHIP8_MEMORY_H

//
// Created by Laurent Fourrier on 2017-06-16.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#ifndef TWC_CHIP8_CONST_H
#define TWC_CHIP8_CONST_H

#define C8_INSTRUCTION_COUNT 19
#define C8_STATIC_ARGUMENT_COUNT 7

namespace c8 {
    namespace detail {
        extern char const *INSTRUCTION_CODE_STRINGS[C8_INSTRUCTION_COUNT];
        extern char const *STATIC_ARGUMENT_STRINGS[C8_STATIC_ARGUMENT_COUNT];
    }
}

#endif //TWC_CHIP8_CONST_H

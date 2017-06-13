//
// Created by Laurent Fourrier on 2017-05-21.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#ifndef TWC_CHIP8_INSTRUCTION_H
#define TWC_CHIP8_INSTRUCTION_H

#include <string>

#define C8_MAX_OPERANDS 3

namespace c8 {
    enum opcode {
        CLS,
        RET,
        JP,
        CALL,
        SE,
        SNE,
        LD,
        ADD,
        OR,
        AND,
        XOR,
        SUB,
        SHR,
        SUBN,
        SHL,
        RND,
        DRW,
        SKP,
        SKNP
    };

    enum operand_type {
        NONE,
        ADDRESS,
        REGISTER,
        WORD,
        POINTER,
        KEY,
        DELAY_TIMER,
        SOUND_TIMER,
        FONT_POINTER,
        BCD_POINTER,
        ARRAY_POINTER
    };

    struct operand {
        operand_type type;
        uint16_t value;

        std::string to_string() const;
    };

    struct instruction {
        opcode code;
        operand operands[C8_MAX_OPERANDS];

        std::string to_string() const;
    };
}

#endif //TWC_CHIP8_INSTRUCTION_H

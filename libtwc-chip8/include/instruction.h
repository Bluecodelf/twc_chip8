//
// Created by Laurent Fourrier on 2017-05-21.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#ifndef TWC_CHIP8_INSTRUCTION_H
#define TWC_CHIP8_INSTRUCTION_H

#include <string>
#include <boost/blank.hpp>
#include <boost/variant.hpp>
#include "memory.h"

#define C8_MAX_ARGUMENTS 3

namespace c8 {
    enum instruction_code {
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

    enum argument_type {
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

    struct argument {
        argument_type type;
        union {
            vm_byte byte;
            vm_register reg;
            vm_address addr;
        } value;

        std::string to_string() const;
    };

    struct label_argument {
        std::string label_identifier;

        std::string to_string() const;
    };

    struct instruction {
        using dyn_argument = boost::variant<boost::blank, argument, label_argument>;

        instruction_code code;
        dyn_argument arguments[C8_MAX_ARGUMENTS];

        std::string to_string() const;
    };
}

#endif //TWC_CHIP8_INSTRUCTION_H

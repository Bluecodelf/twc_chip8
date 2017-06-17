//
// Created by Laurent Fourrier on 2017-06-16.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#include "constants.h"

char const *c8::detail::INSTRUCTION_CODE_STRINGS[19] = {
        "CLS",
        "RET",
        "JP",
        "CALL",
        "SE",
        "SNE",
        "LD",
        "ADD",
        "OR",
        "AND",
        "XOR",
        "SUB",
        "SHR",
        "SUBN",
        "SHL",
        "RND",
        "DRW",
        "SKP",
        "SKNP"
};

char const *c8::detail::STATIC_ARGUMENT_STRINGS[7] = {
        "I",
        "K",
        "DT",
        "ST",
        "F",
        "B",
        "[I]"
};
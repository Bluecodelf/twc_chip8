//
// Created by Laurent Fourrier on 2017-05-22.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#include "instruction.h"

#include <sstream>

using namespace c8;

namespace {
    char const *OPCODE_STRINGS[19] = {
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

    char const *STATIC_OPERAND_STRINGS[7] = {
            "I",
            "K",
            "DT",
            "ST",
            "F",
            "B",
            "[I]"
    };
}

std::string operand::to_string() const {
    std::stringstream ss;
    switch (type) {
        case NONE:
            break;
        case ADDRESS:
            ss << "0x" << std::hex << value;
            break;
        case REGISTER:
            ss << "V" << std::hex << value;
            break;
        case WORD:
            ss << value;
            break;
        default:
            ss << STATIC_OPERAND_STRINGS[type - 4];
    }
    return ss.str();
}

std::string instruction::to_string() const {
    std::stringstream ss;
    ss << OPCODE_STRINGS[code];
    for (int it = 0; it < C8_MAX_OPERANDS; it++) {
        if (operands[it].type == NONE) {
            break;
        }
        ss << ((it == 0) ? " " : ", ") << operands[it].to_string();
    }
    ss << std::endl;
    return ss.str();
}
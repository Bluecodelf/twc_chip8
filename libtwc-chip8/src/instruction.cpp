//
// Created by Laurent Fourrier on 2017-05-22.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#include "instruction.h"
#include "constants.h"

#include <sstream>

using namespace c8;

namespace {
    class to_string_visitor : public boost::static_visitor<std::string> {
    public:
        std::string operator()(boost::blank const &blank) const {
            return "";
        }

        std::string operator()(argument const &arg) const {
            return arg.to_string();
        }

        std::string operator()(label_argument const &arg) const {
            return arg.to_string();
        }
    };
}

std::string argument::to_string() const {
    std::stringstream ss;
    switch (type) {
        case NONE:
            break;
        case ADDRESS:
            ss << "0x" << std::hex << +value.addr;
            break;
        case REGISTER:
            ss << "V" << std::hex << +value.reg;
            break;
        case WORD:
            ss << +value.byte;
            break;
        default:
            ss << detail::STATIC_ARGUMENT_STRINGS[type - 4];
    }
    return ss.str();
}

std::string label_argument::to_string() const {
    std::stringstream ss;
    ss << "%" << label_identifier;
    return ss.str();
}

std::string instruction::to_string() const {
    std::stringstream ss;
    ss << detail::INSTRUCTION_CODE_STRINGS[code];
    for (int it = 0; it < C8_MAX_ARGUMENTS; it++) {
        std::string argument = boost::apply_visitor(to_string_visitor(), arguments[it]);
        if (argument.empty()) {
            break;
        }
        ss << ((it == 0) ? " " : ", ") << argument;
    }
    ss << std::endl;
    return ss.str();
}
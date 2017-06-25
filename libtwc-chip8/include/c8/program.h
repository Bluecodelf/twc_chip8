//
// Created by Laurent Fourrier on 2017-06-14.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#ifndef TWC_CHIP8_PROGRAM_H
#define TWC_CHIP8_PROGRAM_H

#include <string>
#include <map>
#include <vector>

#include "memory.h"
#include "instruction.h"

namespace c8 {
    // A program represent a set of instruction (along with its labels, if any)
    struct program {
        std::map<std::string, vm_address> labels{};
        std::vector<instruction> instructions{};
    };

    class parsing_exception : public std::exception {
    public:
        explicit parsing_exception(std::string const &message);

        virtual ~parsing_exception() {}

        virtual char const *what() const noexcept;
    protected:
        explicit parsing_exception() {}

        std::string _message;
    };

    program parse_assembly(std::string const &input);
}

#endif //TWC_CHIP8_PROGRAM_H

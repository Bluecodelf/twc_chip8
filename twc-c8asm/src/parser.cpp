//
// Created by Laurent Fourrier on 2017-06-05.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#include "parser.h"

#include <sstream>

parsing_exception::parsing_exception(std::string const &error, unsigned int line_number) {
    if (line_number == 0) {
        _message = error;
    } else {
        std::stringstream ss;
        ss << "Error at line " << line_number << ": " << error;
        _message = ss.str();
    }
}

char const *parsing_exception::what() const noexcept {
    return &_message[0];
}
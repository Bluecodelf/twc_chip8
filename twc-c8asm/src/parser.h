//
// Created by Laurent Fourrier on 2017-06-05.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#ifndef TWC_CHIP8_PARSER_H
#define TWC_CHIP8_PARSER_H

#include <exception>
#include <istream>
#include <string>
#include <cctype>

class parsing_exception : public std::exception {
public:
    explicit parsing_exception(std::string const &error, unsigned int line_number = 0);
    virtual ~parsing_exception() {}

    virtual char const *what() const noexcept;

private:
    std::string _message;
};

#endif //TWC_CHIP8_PARSER_H

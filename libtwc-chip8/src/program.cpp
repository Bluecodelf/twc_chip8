//
// Created by Laurent Fourrier on 2017-06-16.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#include "program.h"
#include "parsing/lexer.h"
#include "parsing/parser.h"

#define C8_DEFINITION_GET(type, value) boost::get<type, \
boost::blank, c8::instruction, detail::label_definition>(value)

using namespace c8;

parsing_exception::parsing_exception(std::string const &message) :
        _message(message) {}

char const *parsing_exception::what() const noexcept {
    return &_message[0];
}

program c8::parse_assembly(std::string const &input) {
    program prog;
    detail::lexer lexer{input};
    std::vector<detail::token> tokens = lexer.lex_all();
    detail::parser parser{tokens};
    while (parser.has_next()) {
        detail::parser::definition definition = parser.parse_next();
        if (definition.type() == typeid(c8::instruction)) {
            prog.instructions.push_back(C8_DEFINITION_GET(c8::instruction, definition));
        } else if (definition.type() == typeid(detail::label_definition)) {
            detail::label_definition label_definition = C8_DEFINITION_GET(
                    detail::label_definition, definition);
            prog.labels[label_definition.label] = label_definition.addr;
        }
    }
    return prog;
}
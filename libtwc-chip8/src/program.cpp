//
// Created by Laurent Fourrier on 2017-06-16.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#include "c8/program.h"
#include "parsing/lexer.h"
#include "parsing/parser.h"

#include <sstream>

#define _DEFINITION_GET(type, value) boost::get<type,\
boost::blank, c8::instruction, detail::label_definition>(value)

#define _ARG_GET_LABEL(value)  boost::get<label_argument,\
boost::blank, argument, label_argument>(value)

using namespace c8;

parsing_exception::parsing_exception(std::string const &message) :
        _message(message) {}

char const *parsing_exception::what() const noexcept {
    return &_message[0];
}

program c8::parse_assembly(std::string const &input) {
    program prog{};
    detail::lexer lexer{input};
    std::vector<detail::token> tokens = lexer.lex_all();
    detail::parser parser{tokens};

    // Parsing and lexing phase. We also check the arguments there.
    while (parser.has_next()) {
        detail::parser::definition definition = parser.parse_next();
        if (definition.type() == typeid(c8::instruction)) {
            instruction inst = _DEFINITION_GET(c8::instruction, definition);
            if (!inst.is_valid()) {
                std::stringstream ss;
                ss << "[LOGIC] Invalid arguments for instruction at line " << parser.get_line();
                throw parsing_exception(ss.str());
            }
            prog.instructions.push_back(inst);
        } else if (definition.type() == typeid(detail::label_definition)) {
            detail::label_definition label_definition = _DEFINITION_GET(
                    detail::label_definition, definition);
            prog.labels[label_definition.label] = label_definition.addr;
        }
    }

    // "Label" phase ; we go through all the instructions again to replace the labels
    // with their referenced addresses.
    for (auto it = prog.instructions.begin(); it != prog.instructions.end(); it++) {
        for (int arg_it = 0; arg_it < C8_MAX_ARGUMENTS; arg_it++) {
            if (it->arguments[arg_it].type() == typeid(label_argument)) {
                label_argument label = _ARG_GET_LABEL(it->arguments[arg_it]);
                try {
                    vm_address label_addr = prog.labels.at(label.label_identifier);
                    argument resolved_argument;
                    resolved_argument.type = ADDRESS;
                    resolved_argument.value.addr = label_addr;
                    it->arguments[arg_it] = instruction::dyn_argument(resolved_argument);
                } catch (std::out_of_range const &) {
                    std::stringstream ss;
                    ss << "[LOGIC] Undefined reference to label " << label.label_identifier;
                    throw parsing_exception(ss.str());
                }
            }
        }
    }

    return prog;
}
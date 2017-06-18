//
// Created by Laurent Fourrier on 2017-06-16.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#include "parser.h"
#include "constants.h"

#include <sstream>

using namespace c8::detail;

namespace {
    class parser_exception : public c8::parsing_exception {
    public:
        parser_exception(const std::string &error, const token &tok) {
            std::stringstream ss;
            ss << "[TOKEN] Error at (" << tok.line << ":" << tok.column << "): " << error;
            _message = ss.str();
        }
    };

    c8::instruction_code parse_instruction_code(token const &tok) {
        std::string instruction_code_str = tok.content.substr(0, tok.content.length());
        for (int it = 0; it < C8_INSTRUCTION_COUNT; it++) {
            if (instruction_code_str.compare(INSTRUCTION_CODE_STRINGS[it]) == 0) {
                return static_cast<c8::instruction_code>(it);
            }
        }
        throw parser_exception("Unknown instruction", tok);
    }

    inline bool is_argument(token const &tok) {
        switch (tok.type) {
            case TOKEN_ADDRESS:
            case TOKEN_REGISTER:
            case TOKEN_NUMBER:
            case TOKEN_ARG_KEYWORD:
            case TOKEN_LABEL_REF:
                return true;
            default:
                return false;
        }
    }

    inline bool is_line_end(token const &tok) {
        return (tok.type == TOKEN_LINE_SEPARATOR || tok.type == TOKEN_END);
    }

    char const *ADDRESS_OUT_OF_RANGE = "Address must be between 0x0 and 0xFFFF (inclusive)";
    char const *NUMBER_OUT_OF_RANGE = "Number must be between 0 and 255 (inclusive)";
    char const *UNEXPECTED_TOKEN = "Unexpected token";

    c8::vm_address parse_address_argument(token const &tok) {
        try {
            int addr = std::stoi(tok.content.substr(2), nullptr, 16);
            if (addr < 0 || addr > 0xFFFF) {
                throw parser_exception(ADDRESS_OUT_OF_RANGE, tok);
            }
            return static_cast<c8::vm_address>(addr);
        } catch (std::out_of_range const &e) {
            throw parser_exception(ADDRESS_OUT_OF_RANGE, tok);
        }
    }

    c8::vm_register parse_register_argument(token const &tok) {
        int reg = std::toupper(tok.content[1]);
        if (reg >= '0' && reg <= '9') {
            return static_cast<c8::vm_register>(reg - '0');
        } else if (reg >= 'A' && reg <= 'F') {
            return static_cast<c8::vm_register>(reg - 'A' + 10);
        } else {
            throw parser_exception("Invalid register", tok);
        }
    }

    c8::vm_byte parse_number_argument(token const &tok) {
        int nbr = std::stoi(tok.content);
        if (nbr < 0 || nbr > 255) {
            throw parser_exception(NUMBER_OUT_OF_RANGE, tok);
        }
        return static_cast<c8::vm_byte>(nbr);
    }

    c8::argument_type parse_keyword_argument_type(token const &tok) {
        for (int it = 0; it < C8_STATIC_ARGUMENT_COUNT; it++) {
            if (tok.content.compare(STATIC_ARGUMENT_STRINGS[it]) == 0) {
                return static_cast<c8::argument_type>(it + 4);
            }
        }
        throw parser_exception("Invalid argument", tok);
    }

    c8::argument parse_raw_argument(token const &tok) {
        c8::argument arg;
        if (tok.type == TOKEN_ADDRESS) {
            arg.type = c8::ADDRESS;
            arg.value.addr = parse_address_argument(tok);
        } else if (tok.type == TOKEN_REGISTER) {
            arg.type = c8::REGISTER;
            arg.value.reg = parse_register_argument(tok);
        } else if (tok.type == TOKEN_NUMBER) {
            arg.type = c8::WORD;
            arg.value.reg = parse_number_argument(tok);
        } else {
            arg.type = parse_keyword_argument_type(tok);
        }
        return arg;
    }

    c8::label_argument parse_label_argument(token const &tok) {
        return c8::label_argument{tok.content.substr(1)};
    }

    c8::instruction::dyn_argument parse_argument(token const &tok) {
        if (tok.type == TOKEN_LABEL_REF) {
            return c8::instruction::dyn_argument(parse_label_argument(tok));
        } else {
            return c8::instruction::dyn_argument(parse_raw_argument(tok));
        }
    }

    label_definition parse_label(token const &tok, c8::vm_address current_addr) {
        std::string label_name = tok.content.substr(0, tok.content.length() - 1);
        return label_definition{label_name, current_addr};
    }
}

parser::parser(std::vector<token> const &tokens):
        _line(1),
        _current_addr(0x200),
        _tokens(tokens),
        _token_it(tokens.cbegin()) {}

parser::definition parser::parse_next() {
    // Skip unneeded end of lines
    while (_token_it->type == TOKEN_LINE_SEPARATOR) {
        _token_it++;
    }

    _line = _token_it->line;
    if (_token_it->type == TOKEN_INSTRUCTION) {
        // Parse the instruction and its arguments
        c8::instruction inst;
        inst.code = parse_instruction_code(*_token_it++);
        for (int it = 0; it < C8_MAX_ARGUMENTS; it++) {
            if (it == 0 && is_argument(*_token_it)) {
                inst.arguments[it] = parse_argument(*_token_it++);
            } else if (it > 0 && _token_it->type == TOKEN_ARG_SEPARATOR) {
                if (!is_argument(*(++_token_it))) {
                    throw parser_exception(UNEXPECTED_TOKEN, *_token_it);
                }
                inst.arguments[it] = parse_argument(*_token_it++);
            } else if (is_line_end(*_token_it)) {
                inst.arguments[it] = c8::instruction::dyn_argument(boost::blank{});
            } else {
                throw parser_exception(UNEXPECTED_TOKEN, *_token_it);
            }
        }

        // Check if the instruction is followed by a line end (or end of file) and consume it
        if (_token_it->type == TOKEN_LINE_SEPARATOR) {
            _token_it++;
        } else if (_token_it->type != TOKEN_END) {
            throw parser_exception(UNEXPECTED_TOKEN, *_token_it);
        }

        _current_addr += 2;
        return parser::definition(inst);
    } else if (_token_it->type == TOKEN_LABEL) {
        return parser::definition(parse_label(*_token_it++, _current_addr));
    } else if (_token_it->type == TOKEN_END) {
        return parser::definition(boost::blank{});
    } else {
        throw parser_exception(UNEXPECTED_TOKEN, *_token_it);
    }
}

bool parser::has_next() const {
    return (_token_it->type != TOKEN_END);
}

int parser::get_line() const {
    return _line;
}
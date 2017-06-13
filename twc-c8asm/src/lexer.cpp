//
// Created by Laurent Fourrier on 2017-06-06.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#include "lexer.h"

#include <regex>

namespace {
    unsigned int const INSTRUCTION_COUNT = 19;

    char const *INSTRUCTION_STRINGS[INSTRUCTION_COUNT] = {
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

    unsigned int const ARG_KEYWORD_COUNT = 7;

    char const *ARG_KEYWORD_STRINGS[ARG_KEYWORD_COUNT] = {
            "I",
            "K",
            "DT",
            "ST",
            "F",
            "B",
            "[I]"
    };

    class keyword_matcher : public matcher_base {
    public:
        keyword_matcher(std::string const &keyword, bool distinct = false) :
                _distinct(distinct),
                _keyword(keyword) {}

        virtual bool matches(tokenizer &input) const {
            input.skip_separators();
            input.begin_record();
            for (int it = 0; it < _keyword.size(); it++) {
                if (input.next() != _keyword[it]) {
                    return false;
                }
            }
            input.end_record();
            return !(_distinct && input.has_next() && !isspace(input.peek()));
        }

    private:
        bool _distinct;
        std::string _keyword;
    };

    class regex_matcher : public matcher_base {
    public:
        regex_matcher(std::regex const &regex) :
                _regex(regex) {}

        virtual bool matches(tokenizer &input) const {
            input.skip_separators();
            input.begin_record();

            auto it = input.begin();
            std::smatch res;
            if (std::regex_search(it, input.end(), res, _regex) && res[0].first == it) {
                // Start record from first marked sub-expresssion if the regex contains one
                if (res.size() > 1) {
                    while (input.next() && ++it != res[1].first);
                    input.begin_record();
                    while (input.next() && ++it != res[1].second);
                    input.end_record();
                    while (input.next() && ++it != res[0].second);
                } else {
                    while (input.next() && ++it != res[0].second);
                    input.end_record();
                }
                return true;
            } else {
                return false;
            }
        }

    private:
        std::regex _regex;
    };
}

lexer_exception::lexer_exception(std::string const &error, tokenizer const &input) {
    std::stringstream ss;
    tokenizer_cursor cursor = input.get_cursor();
    ss << "[LEXER] Error at (" << cursor.line
       << ":" << cursor.column << "): " << error;
    _message = ss.str();
}

char const *lexer_exception::what() const noexcept {
    return &_message[0];
}

lexer::lexer(std::string const &input) :
        _input{input} {
    // Instructions (JP, ADD, CALL...)
    for (int it = 0; it < INSTRUCTION_COUNT; it++) {
        add_match_rule(match_rule{
                token_type::INSTRUCTION,
                std::make_unique<keyword_matcher>(INSTRUCTION_STRINGS[it], true)
        });
    }

    // Addresses (for example 0x042A)
    add_match_rule(match_rule{
            token_type::ADDRESS,
            std::make_unique<regex_matcher>(std::regex("^0x[0-9a-fA-F]+"))
    });

    // Registers, of format Vn
    add_match_rule(match_rule{
            token_type::REGISTER,
            std::make_unique<regex_matcher>(std::regex("^V[0-9a-fA-F]"))
    });

    // Number values
    add_match_rule(match_rule{
            token_type::NUMBER,
            std::make_unique<regex_matcher>(std::regex("^[0-9]+"))
    });

    // "Static" argument keywords (I, DT, ST...)
    for (int it = 0; it < ARG_KEYWORD_COUNT; it++) {
        add_match_rule(match_rule{
                token_type::ARG_KEYWORD,
                std::make_unique<keyword_matcher>(ARG_KEYWORD_STRINGS[it], false)
        });
    }

    // Simple argument separator (',')
    add_match_rule(match_rule{
            token_type::ARG_SEPARATOR,
            std::make_unique<keyword_matcher>(",", false)
    });

    // Line separator (\n)
    add_match_rule(match_rule{
            token_type::LINE_SEPARATOR,
            std::make_unique<keyword_matcher>("\n", false)
    });

    // Label (of format 'label:')
    add_match_rule(match_rule{
            token_type::LABEL,
            std::make_unique<regex_matcher>(std::regex("^[0-9a-zA-Z\\-_]+:"))
    });

    // Label reference (of format '%label', for use in arguments)
    add_match_rule(match_rule{
            token_type::LABEL_REF,
            std::make_unique<regex_matcher>(std::regex("^%[0-9a-zA-Z\\-_]+"))
    });
}

void lexer::add_match_rule(match_rule rule) {
    _match_rules.push_back(std::move(rule));
}

token lexer::lex_next() {
    _input.skip_separators();
    for (auto it = _match_rules.cbegin(); it != _match_rules.cend(); it++) {
        _input.save_position();
        if (it->matcher->matches(_input)) {
            tokenizer_cursor cursor = _input.get_saved_cursor();
            return token{
                    it->type,
                    _input.get_record(),
                    cursor.line,
                    cursor.column
            };
        }
        _input.reset_position();
    }
    throw lexer_exception{"unrecognized token", _input};
}

std::vector<token> lexer::lex_all() {
    std::vector<token> tokens;
    while (has_next()) {
        tokens.push_back(lex_next());
    }
    return tokens;
}

bool lexer::has_next() {
    _input.skip_separators();
    return _input.has_next();
}
//
// Created by Laurent Fourrier on 2017-06-15.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#include "lexer.h"
#include "constants.h"
#include "program.h"

#include <regex>

using namespace c8::detail;

namespace {
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

    class lexer_exception : public c8::parsing_exception {
    public:
        lexer_exception(std::string const &error, tokenizer const &input) {
            std::stringstream ss;
            tokenizer_cursor cursor = input.get_cursor();
            ss << "[LEXER] Error at (" << cursor.line << ":" << cursor.column << "): " << error;
            _message = ss.str();
        }
    };
}

lexer::lexer(std::string const &input) :
        _input{input} {
    // Instructions (JP, ADD, CALL...)
    for (int it = 0; it < C8_INSTRUCTION_COUNT; it++) {
        add_match_rule(match_rule{
                token_type::TOKEN_INSTRUCTION,
                std::make_unique<keyword_matcher>(detail::INSTRUCTION_CODE_STRINGS[it], true)
        });
    }

    // Addresses (for example 0x042A)
    add_match_rule(match_rule{
            token_type::TOKEN_ADDRESS,
            std::make_unique<regex_matcher>(std::regex("^0x[0-9a-fA-F]+"))
    });

    // Registers, of format Vn
    add_match_rule(match_rule{
            token_type::TOKEN_REGISTER,
            std::make_unique<regex_matcher>(std::regex("^V[0-9a-fA-F]"))
    });

    // Number values
    add_match_rule(match_rule{
            token_type::TOKEN_NUMBER,
            std::make_unique<regex_matcher>(std::regex("^[0-9]+"))
    });

    // "Static" argument keywords (I, DT, ST...)
    for (int it = 0; it < C8_STATIC_ARGUMENT_COUNT; it++) {
        add_match_rule(match_rule{
                token_type::TOKEN_ARG_KEYWORD,
                std::make_unique<keyword_matcher>(detail::STATIC_ARGUMENT_STRINGS[it], false)
        });
    }

    // Simple argument separator (',')
    add_match_rule(match_rule{
            token_type::TOKEN_ARG_SEPARATOR,
            std::make_unique<keyword_matcher>(",", false)
    });

    // Line separator (\n)
    add_match_rule(match_rule{
            token_type::TOKEN_LINE_SEPARATOR,
            std::make_unique<keyword_matcher>("\n", false)
    });

    // Label (of format 'label:')
    add_match_rule(match_rule{
            token_type::TOKEN_LABEL,
            std::make_unique<regex_matcher>(std::regex("^[0-9a-zA-Z\\-_]+:"))
    });

    // Label reference (of format '%label', for use in arguments)
    add_match_rule(match_rule{
            token_type::TOKEN_LABEL_REF,
            std::make_unique<regex_matcher>(std::regex("^%[0-9a-zA-Z\\-_]+"))
    });
}

void lexer::add_match_rule(match_rule rule) {
    _match_rules.push_back(std::move(rule));
}

token lexer::lex_next() {
    _input.skip_separators();
    if (has_next()) {
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
    } else {
        tokenizer_cursor cursor = _input.get_saved_cursor();
        return token {
                TOKEN_END,
                "",
                cursor.line,
                cursor.column
        };
    }
    throw lexer_exception{"unrecognized token", _input};
}

std::vector<token> lexer::lex_all() {
    std::vector<token> tokens;
    while (has_next()) {
        tokens.push_back(lex_next());
    }
    tokens.push_back(lex_next()); // Push the final TOKEN_END
    return tokens;
}

bool lexer::has_next() {
    _input.skip_separators();
    return _input.has_next();
}
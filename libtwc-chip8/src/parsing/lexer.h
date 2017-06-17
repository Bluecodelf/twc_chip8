//
// Created by Laurent Fourrier on 2017-06-15.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#ifndef TWC_CHIP8_LEXER_H
#define TWC_CHIP8_LEXER_H

#include <vector>
#include <string>
#include <memory>

#include "tokenizer.h"

namespace c8 {
    namespace detail {
        enum token_type {
            TOKEN_INSTRUCTION,
            TOKEN_ADDRESS,
            TOKEN_REGISTER,
            TOKEN_NUMBER,
            TOKEN_ARG_KEYWORD,
            TOKEN_ARG_SEPARATOR,
            TOKEN_LABEL,
            TOKEN_LABEL_REF,
            TOKEN_LINE_SEPARATOR,
            TOKEN_END
        };

        struct token {
            token_type type;
            std::string content;
            unsigned int line;
            unsigned int column;
        };

        class matcher_base {
        public:
            virtual bool matches(tokenizer &input) const = 0;
        };

        struct match_rule {
            token_type type;
            std::unique_ptr<matcher_base> matcher;
        };

        class lexer {
        public:
            lexer(std::string const &input);

            void add_match_rule(match_rule rule);

            token lex_next();
            std::vector<token> lex_all();
            bool has_next();
        private:
            tokenizer _input;
            std::vector<match_rule> _match_rules;
        };
    }
}

#endif //TWC_CHIP8_LEXER_H

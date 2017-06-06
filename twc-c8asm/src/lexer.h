//
// Created by Laurent Fourrier on 2017-06-06.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#ifndef TWC_CHIP8_LEXER_H
#define TWC_CHIP8_LEXER_H

#include <string>
#include <vector>
#include <memory>
#include <exception>
#include "tokenizer.h"

enum token_type {
    INSTRUCTION,
    ADDRESS,
    REGISTER,
    NUMBER,
    ARG_KEYWORD,
    ARG_SEPARATOR,
    LABEL,
    LABEL_REF,
    LINE_SEPARATOR
};

struct token {
    token_type type;
    std::string content;
    unsigned int line;
    unsigned int column;
};

class matcher_base {
public:
    virtual bool matches(tokenizer &tokenizer) const = 0;
};

struct match_rule {
    token_type type;
    std::unique_ptr<matcher_base> matcher;
};

class lexer_exception : public std::exception {
public:
    explicit lexer_exception(std::string const &error, tokenizer const &tokenizer);

    virtual ~lexer_exception() {}

    virtual char const *what() const noexcept;

private:
    std::string _message;
};

class lexer {
public:
    lexer(std::string const &input);

    void add_match_rule(match_rule rule);

    token lex_next();

    std::vector<token> lex_all();

    bool has_next();

private:
    tokenizer _tokenizer;
    std::vector<match_rule> _match_rules;
};

#endif //TWC_CHIP8_LEXER_H

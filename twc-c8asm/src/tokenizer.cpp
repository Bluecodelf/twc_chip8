//
// Created by Laurent Fourrier on 2017-06-06.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#include "tokenizer.h"

namespace {
    constexpr char const *TOKENIZER_SEPARATORS = " \t\r";

    inline bool is_separator(char const c) {
        for (int it = 0; TOKENIZER_SEPARATORS[it]; it++) {
            if (TOKENIZER_SEPARATORS[it] == c) {
                return true;
            }
        }
        return false;
    }
}

tokenizer::tokenizer(std::string const &input) :
        _input(input),
        _record_begin(0),
        _record_count(0) {
    _cursor = {0, 1, 1};
    _saved_cursor = _cursor;
}

inline char const tokenizer::next() {
    if (has_next()) {
        if (_input[_cursor.idx] == '\n') {
            _cursor.line++;
            _cursor.column = 1;
        } else {
            _cursor.column++;
        }
        return _input[_cursor.idx++];
    } else {
        return 0;
    }
}

inline char const tokenizer::peek() const {
    if (has_next()) {
        return _input[_cursor.idx];
    } else {
        return '\0';
    }
}

inline bool tokenizer::has_next() const {
    return (_cursor.idx < _input.length());
}

void tokenizer::save_position() {
    _saved_cursor = _cursor;
}

void tokenizer::reset_position() {
    _cursor = _saved_cursor;
}

void tokenizer::skip_separators() {
    while (is_separator(peek())) {
        next();
    }
}

void tokenizer::begin_record() {
    _record_begin = _cursor.idx;
}

void tokenizer::end_record() {
    _record_count = (_cursor.idx > _record_begin) ? (_cursor.idx - _record_begin) : 0;
}

std::string tokenizer::get_record() const {
    return _input.substr(_record_begin, _record_count);
}

tokenizer_cursor tokenizer::get_cursor() const {
    return _cursor;
}

tokenizer_cursor tokenizer::get_saved_cursor() const {
    return _saved_cursor;
}

tokenizer::iterator tokenizer::begin() const {
    return _input.cbegin() + _cursor.idx;
}

tokenizer::iterator tokenizer::end() const {
    return _input.cend();
}
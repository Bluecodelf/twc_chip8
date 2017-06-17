//
// Created by Laurent Fourrier on 2017-06-15.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#ifndef TWC_CHIP8_TOKENIZER_H
#define TWC_CHIP8_TOKENIZER_H

#include <string>

namespace c8 {
    namespace detail {
        struct tokenizer_cursor {
            unsigned int idx;
            unsigned int line;
            unsigned int column;
        };

        class tokenizer {
        public:
            using iterator = std::string::const_iterator;

            tokenizer(std::string const &input);

            char const next();
            char const peek() const;
            bool has_next() const;

            void save_position();
            void reset_position();
            void skip_separators();

            void begin_record();
            void end_record();
            std::string get_record() const;

            tokenizer_cursor get_cursor() const;
            tokenizer_cursor get_saved_cursor() const;

            iterator begin() const;
            iterator end() const;
        private:
            std::string const &_input;
            tokenizer_cursor _cursor;
            tokenizer_cursor _saved_cursor;
            unsigned int _record_begin;
            unsigned int _record_count;
        };
    }
}

#endif //TWC_CHIP8_TOKENIZER_H

//
// Created by Laurent Fourrier on 2017-05-31.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "lexer.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " FILENAME" << std::endl;
        return 0;
    }

    std::ifstream in(argv[1]);
    std::string data((std::istreambuf_iterator<char>(in)),
                     std::istreambuf_iterator<char>());

    try {
        lexer lexer(data);
        std::vector<token> tokens = lexer.lex_all();
        return 0;
    } catch (lexer_exception const &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
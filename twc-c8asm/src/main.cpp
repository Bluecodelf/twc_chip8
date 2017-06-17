//
// Created by Laurent Fourrier on 2017-05-31.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <program.h>

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " FILENAME" << std::endl;
        return 0;
    }

    std::ifstream in(argv[1]);
    std::string data((std::istreambuf_iterator<char>(in)),
                     std::istreambuf_iterator<char>());

    try {
        c8::program program = c8::parse_assembly(data);
        std::cout << "Successfully read program!" << std::endl;
        std::cout << "Labels: " << std::endl;
        for (auto it = program.labels.cbegin(); it != program.labels.cend(); it++) {
            std::cout << "\t" << it->first << " : 0x" << std::hex << it->second << std::endl;
        }
        std::cout << std::dec << "Instructions: " << std::endl;
        for (auto it = program.instructions.cbegin(); it != program.instructions.cend(); it++) {
            std::cout << "\t" << it->to_string();
        }
    } catch (c8::parsing_exception const &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
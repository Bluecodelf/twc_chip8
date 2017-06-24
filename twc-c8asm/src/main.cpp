//
// Created by Laurent Fourrier on 2017-05-31.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <program.h>
#include <cpu.h>

short const DUMMY_DATA = 1;
#define IS_LITTLE_ENDIAN ((*((char const *)&DUMMY_DATA)) == 1)

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

        // Print some debug information about the program.
        std::cout << "Successfully read program!" << std::endl;
        std::cout << "Labels: " << std::endl;
        for (auto it = program.labels.cbegin(); it != program.labels.cend(); it++) {
            std::cout << "\t" << it->first << " : 0x" << std::hex << it->second << std::endl;
        }
        std::cout << std::dec << "Instructions: " << std::endl;
        for (auto it = program.instructions.cbegin(); it != program.instructions.cend(); it++) {
            std::cout << "\t" << it->to_string();
        }

        // Try to replace the .s8 at the end of the script by .c8 (or append it)
        std::string file_name = std::string(argv[1]);
        std::string::size_type ext_index = file_name.rfind(".s8");
        if (ext_index == file_name.size() - 3) {
            file_name.erase(ext_index);
        }
        file_name.append(".c8");

        // Now that we have the file name, try to write every instruction in the output file
        std::ofstream out(file_name, std::ios::trunc | std::ios::binary | std::ios::out);
        for (auto it = program.instructions.cbegin(); it != program.instructions.cend(); it++) {
            c8::vm_instruction vm_instruction = it->to_vm_repr();
            char *vm_inst_data = reinterpret_cast<char *>(&vm_instruction);
            if (IS_LITTLE_ENDIAN) {
                char tmp = vm_inst_data[1];
                vm_inst_data[1] = vm_inst_data[0];
                vm_inst_data[0] = tmp;
            }
            out.write(vm_inst_data, sizeof(c8::vm_instruction));
        }
        out.close();
    } catch (c8::parsing_exception const &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
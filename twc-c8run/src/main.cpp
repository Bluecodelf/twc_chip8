//
// Created by Laurent Fourrier on 2017-05-16.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#include <string>
#include <fstream>
#include <cstring>
#include <vector>
#include <CPU.h>
#include <iostream>

#include "Configuration.h"

using namespace Chip8;

std::vector<std::uint8_t> load_file(const std::string &path)
{
    // We can only read a file that is the size of the memory (or less)
    std::ifstream file(path);
    char buffer[MEMORY_SIZE];
    file.read(buffer, MEMORY_SIZE);

    unsigned int bytes_read = static_cast<unsigned int>(file.gcount());
    std::vector<std::uint8_t> data(bytes_read);
    memcpy(buffer, &data, bytes_read);
    return data;
}

int main(int argc, char **argv)
{
    try
    {
        Configuration cfg;
        cfg.parse_arguments(argc, argv);

        std::vector<std::uint8_t> rom_data = load_file(cfg.get_file());

        CPU cpu;
        cpu.load_in_memory(rom_data, 0x200);
        return 0;
    }
    catch (const ConfigurationException &e)
    {
        std::cerr << e.what() << std::endl;
        std::cout << "Usage: " << argv[0] << " ROM_FILE" << std::endl;
        return 1;
    }
}
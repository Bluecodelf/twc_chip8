//
// Created by Laurent Fourrier on 2017-05-16.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#ifndef TWC_CHIP8_CPU_H
#define TWC_CHIP8_CPU_H

#include <vector>
#include "memory.h"

namespace c8
{
    class cpu
    {
    public:
        cpu();

        void load_in_memory(const std::vector<std::uint8_t> &data, unsigned int offset);
        void step();
    private:
        memory _memory;
    };
}

#endif //TWC_CHIP8_CPU_H

//
// Created by Laurent Fourrier on 2017-05-16.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#ifndef TWC_CHIP8_CPU_H
#define TWC_CHIP8_CPU_H

#include <vector>
#include <random>

#include "memory.h"
#include "bus.h"

namespace c8 {
    class cpu_exception {
    public:
        explicit cpu_exception(std::string const &message);

        virtual ~cpu_exception() {}

        virtual char const *what() const noexcept;
    private:
        std::string _message;
    };

    class cpu {
    public:
        cpu(
                display_bus &display,
                keyboard_bus &keyboard,
                timer_bus &_timer,
                sound_bus &sound
        );

        void load_in_memory(const std::vector<std::uint8_t> &data, unsigned int offset);

        void step();
    private:
        // CPU internals (memory and RNG)
        memory _memory;
        std::default_random_engine _rand;
        std::uniform_int_distribution<vm_byte> _dist;

        // Buses
        display_bus &_display;
        keyboard_bus &_keyboard;
        timer_bus &_timer;
        sound_bus &_sound;
    };
}

#endif //TWC_CHIP8_CPU_H

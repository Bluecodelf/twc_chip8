//
// Created by Laurent Fourrier on 6/21/17.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#ifndef TWC_CHIP8_BUS_H
#define TWC_CHIP8_BUS_H

namespace c8 {
    class display_bus {
    public:
        virtual ~display_bus() {}

        // Should return the old pixel state
        virtual bool set_pixel_state(int x, int y, bool new_state) = 0;

        virtual void update() = 0;
        virtual void clear() = 0;
    };

    class keyboard_bus {
    public:
        virtual ~keyboard_bus() {}

        // Should be blocking
        virtual vm_byte get_key() = 0;

        // This one is not blocking
        virtual bool is_key_pressed(vm_byte key) = 0;
    };

    // Take note that there can be two timers connected to the CPU (delay and sound)
    class timer_bus {
    public:
        virtual ~timer_bus() {}

        virtual void set_value(vm_byte value) = 0;
        virtual vm_byte get_value() = 0;

        virtual void wait() = 0;
    };

    class sound_bus {
    public:
        virtual ~sound_bus() {}

        virtual void play_sound(vm_byte duration) = 0;
    };
}

#endif //TWC_CHIP8_BUS_H

//
// Created by Laurent Fourrier on 2017-05-16.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#include "cpu.h"
#include "instruction.h"

#include <iostream>

using namespace c8;

namespace {
    std::uint8_t const FONT[80] = {
            0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
            0x20, 0x60, 0x20, 0x20, 0x70, // 1
            0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
            0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
            0x90, 0x90, 0xF0, 0x10, 0x10, // 4
            0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
            0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
            0xF0, 0x10, 0x20, 0x40, 0x40, // 7
            0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
            0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
            0xF0, 0x90, 0xF0, 0x90, 0x90, // A
            0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
            0xF0, 0x80, 0x80, 0x80, 0xF0, // C
            0xE0, 0x90, 0x90, 0x90, 0xE0, // D
            0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
            0xF0, 0x80, 0xF0, 0x80, 0xF0  // F
    };
}

cpu_exception::cpu_exception(std::string const &message) :
        _message(message) {}

char const *cpu_exception::what() const noexcept {
    return &_message[0];
}

cpu::cpu(
        display_bus &display,
        keyboard_bus &keyboard,
        timer_bus &timer,
        sound_bus &sound
) :
        _display(display),
        _keyboard(keyboard),
        _timer(_timer),
        _sound(sound) {
    // Initialize the random engine
    std::random_device rand_device;
    _rand.seed(rand_device());

    // Copy the font in memory
    memcpy(_memory.interpreter.font, FONT, 80);
}

void cpu::load_in_memory(const std::vector<std::uint8_t> &data, unsigned int offset) {
    unsigned int copy = static_cast<unsigned int>(data.size());
    if (copy + offset > C8_MEMORY_SIZE) {
        copy = C8_MEMORY_SIZE - offset;
    }
    memcpy(_memory.memory + offset, &data[0], copy);
}

// Some utility macros for getting instruction arguments
#define _A1 inst
#define _A2 inst >> 4
#define _A3 inst >> 8
#define _NIB(arg) static_cast<vm_byte>(arg & 0xF)
#define _REG(arg) static_cast<vm_byte>(cpu_data->v[arg & 0xF])
#define _BYTE(arg) static_cast<vm_byte>(arg & 0xFF)
#define _ADDR(arg) static_cast<vm_address>(arg & 0xFFF)

void cpu::step() {
    auto cpu_data = &_memory.interpreter;

    // Forced overhead, as the virtual CPU is big endian.
    vm_instruction inst = _memory.memory[cpu_data->pc] | (_memory.memory[cpu_data->pc + 1] << 8);

    // Reading Chip-8 instructions is tricky if you want it to be fast, because opcodes
    // are masked (and thus the CPU can't map them from a jump table). However, we can group
    // the instructions by their first nibble, as each of those groups contains instructions
    // with more or less the same functionality (expect the 0xF000 group).
    switch (inst & 0xF000) {
        case 0x0000:
            if (inst == 0x00E0) {
                _display.clear();
                _memory.interpreter.pc += 2;
            } else if (inst == 0x00EE) {
                if (_memory.interpreter.pc == 0) {
                    throw cpu_exception("Can't return from subroutine: stack is empty");
                }
                cpu_data->pc = cpu_data->stack[--cpu_data->sp];
            }
            break;
        case 0x1000:
            cpu_data->pc = _ADDR(_A1);
            break;
        case 0x2000:
            if (cpu_data->sp == C8_STACK_SIZE) {
                throw cpu_exception("Can't call subroutine: stack is full");
            }
            cpu_data->stack[cpu_data->sp++] = cpu_data->pc;
            cpu_data->pc = _ADDR(_A1);
            break;
        case 0x3000:
            cpu_data->pc += (_REG(_A3) == _BYTE(_A1)) ? 4 : 2;
            break;
        case 0x4000:
            cpu_data->pc += (_REG(_A3) != _BYTE(_A1)) ? 4 : 2;
            break;
        case 0x5000:
            cpu_data->pc += (_REG(_A3) == _REG(_A2)) ? 4 : 2;
            break;
        case 0x6000:
            cpu_data->v[_A3 & 0xF] = _BYTE(_A1);
            break;
        case 0x7000:
            cpu_data->v[_A3 & 0xF] += _BYTE(_A1);
            break;
        case 0x8000: {
            vm_byte &lhs = cpu_data->v[_A3 & 0xF];
            vm_byte &rhs = cpu_data->v[_A2 & 0xF];
            vm_byte &vf = cpu_data->v[15];
            switch (inst & 0x000F) {
                case 0:
                    lhs = rhs;
                    break;
                case 1:
                    lhs |= rhs;
                    break;
                case 2:
                    lhs &= rhs;
                    break;
                case 3:
                    lhs ^= rhs;
                    break;
                case 4: {
                    int buffer = lhs + rhs;
                    vf = static_cast<vm_byte>((buffer > 255) ? 1 : 0);
                    lhs = static_cast<vm_byte>(buffer & 0xFF);
                }
                    break;
                case 5:
                    vf = static_cast<vm_byte>((lhs > rhs) ? 1 : 0);
                    lhs -= rhs;
                    break;
                case 6:
                    vf = static_cast<vm_byte>((lhs & 0b1) ? 1 : 0);
                    lhs /= 2;
                    break;
                case 7:
                    vf = static_cast<vm_byte>((rhs > lhs) ? 1 : 0);
                    lhs = rhs - lhs;
                    break;
                case 0xE:
                    vf = static_cast<vm_byte>((lhs & 0b10000000) ? 1 : 0);
                    lhs *= 2;
                    break;
                default:
                    break;
            }
        }
            break;
        case 0x9000:
            cpu_data->pc += (_REG(_A3) != _REG(_A2)) ? 4 : 2;
            break;
        case 0xA000:
            cpu_data->i = _ADDR(_A1);
            break;
        case 0xB000:
            cpu_data->pc = (_REG(0) + _ADDR(_A1));
            break;
        case 0xC000:
            cpu_data->v[_A3 & 0xF] = _dist(_rand) & _BYTE(_A1);
            break;
        case 0xD000: {
            int sprite_size = _NIB(_A1);
            int y_offset = _REG(_A2);
            int x_offset = _REG(_A3);
            for (int y = 0; y < sprite_size; y++) {
                vm_byte row = _memory.memory[cpu_data->i];
                for (int x = 0; x < 8; x++) {
                    _display.set_pixel_state(x + x_offset, y + y_offset, (row >> (7 - x)));
                }
            }
            _display.update();
        }
            break;
        case 0xE000: {
            bool pressed = _keyboard.is_key_pressed(_REG(_A3));
            cpu_data->pc += ((_BYTE(_A3) == 0xA1) ^ pressed) ? 4 : 2;
        }
            break;
        case 0xF000: {
            vm_byte &reg = cpu_data->v[_A3 & 0xF];
            switch (inst & 0xFF) {
                case 0x07:
                    reg = _timer.get_value();
                    break;
                case 0x0A:
                    reg = _keyboard.get_key();
                    break;
                case 0x15:
                    _timer.set_value(reg);
                    break;
                case 0x18:
                    _sound.play_sound(reg);
                    break;
                case 0xF1:
                    cpu_data->i += reg;
                    break;
                case 0x29:
                    cpu_data->i = sizeof(_memory.interpreter) - 80 + 5 * reg;
                    break;
                case 0x33:
                    _memory.memory[cpu_data->i] = static_cast<vm_byte>(reg / 100 % 10);
                    _memory.memory[cpu_data->i + 1] = static_cast<vm_byte>(reg / 10 % 10);
                    _memory.memory[cpu_data->i + 2] = static_cast<vm_byte>(reg % 10);
                    break;
                case 0x55: {
                    vm_byte reg_count = _NIB(_A3);
                    for (int it = 0; it < reg_count; it++) {
                        _memory.memory[cpu_data->i + it] = cpu_data->v[it];
                    }
                }
                    break;
                case 0x65: {
                    vm_byte reg_count = _NIB(_A3);
                    for (int it = 0; it < reg_count; it++) {
                        cpu_data->v[it] = _memory.memory[cpu_data->i + it];
                    }
                }
                    break;
                default:
                    break;
            }
        }
            break;
        default:
            break;
    }
}
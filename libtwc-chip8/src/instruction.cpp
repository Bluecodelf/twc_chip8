//
// Created by Laurent Fourrier on 2017-05-22.
// Copyright (c) 2017 Tiwind Software. All rights reserved.
//

#include "instruction.h"
#include "constants.h"

#include <map>
#include <vector>
#include <sstream>

#if defined(__i386) || defined(_M_IX86)
#define USE_INTEL_INTRINSICS
#include <immintrin.h>
#elif defined(__amd64) || defined(_M_AMD64)
#define USE_INTEL_INTRINSICS
#include <x86intrin.h>
#endif

#define _A(t, m) instruction_argument_bin{t, m}
#define _N instruction_argument_bin{NONE}

#define _I0(opcode) instruction_variant_bin{\
    opcode,\
    {_N, _N, _N}\
}

#define _I1(opcode, t1, m1) instruction_variant_bin{\
    opcode,\
    {_A(t1, m1), _N, _N}\
}

#define _I2(opcode, t1, m1, t2, m2) instruction_variant_bin{\
    opcode,\
    {_A(t1, m1), _A(t2, m2), _N}\
}

#define _I3(opcode, t1, m1, t2, m2, t3, m3) instruction_variant_bin{\
    opcode,\
    {_A(t1, m1), _A(t2, m2), _A(t3, m3)}\
}

#define _DYN_ARGUMENT_GET(type, value) boost::get<type,\
boost::blank, argument, label_argument>(value)

using namespace c8;

namespace {
    class to_string_visitor : public boost::static_visitor<std::string> {
    public:
        std::string operator()(boost::blank const &blank) const {
            return "";
        }

        std::string operator()(argument const &arg) const {
            return arg.to_string();
        }

        std::string operator()(label_argument const &arg) const {
            return arg.to_string();
        }
    };

    class get_value_visitor : public boost::static_visitor<vm_instruction> {
    public:
        vm_instruction operator()(boost::blank const &blank) const {
            return 0;
        }

        vm_instruction operator()(argument const &arg) const {
            return arg.get_value();
        }

        vm_instruction operator()(label_argument const &arg) const {
            throw instruction_exception("A label does not have any VM representation.");
        }
    };

    inline int get_offset(int mask) {
        if (mask == 0) {
            return 0;
        }

        // Use Intel intrinsics for bsf if available
#ifdef USE_INTEL_INTRINSICS
        return _bit_scan_forward(mask);
#else
        for (int it = 0; it <= 31; it++) {
            if (mask & (1 << it)) {
                return it;
            }
        }
        return 0;
#endif
    }

    struct instruction_argument_bin {
        argument_type type;
        int mask = 0x0000;
    };

    struct instruction_variant_bin {
        int opcode;
        std::array<instruction_argument_bin, C8_MAX_ARGUMENTS> arguments;

        bool matches_instruction(instruction const &inst) const {
            for (int it = 0; it < C8_MAX_ARGUMENTS; it++) {
                if (inst.arguments[it].type() == typeid(label_argument)) {
                    // Treat argument as an address
                    if (arguments[it].type != ADDRESS) {
                        return false;
                    }
                } else if (inst.arguments[it].type() == typeid(boost::blank)) {
                    // Treat argument as "NONE"
                    if (arguments[it].type != NONE) {
                        return false;
                    }
                } else {
                    argument arg = _DYN_ARGUMENT_GET(argument, inst.arguments[it]);
                    if (arg.type != arguments[it].type) {
                        return false;
                    }
                }
            }
            return true;
        }
    };

    struct instruction_bin {
        std::vector<instruction_variant_bin> variants;
    };

    // I hope you like macros.
    std::map<instruction_code, instruction_bin> const INSTRUCTION_BINARY_DEFINITIONS = {
            {CLS,  instruction_bin{{_I0(0x00E0)}}},
            {RET,  {{_I0(0x00EE)}}},
            {JP,   {{_I1(0x1000, ADDRESS, 0x0FFF),
                            _I2(0xB000, REGISTER, 0x0000, ADDRESS, 0x0FFF)}}},
            {CALL, {{_I1(0x2000, ADDRESS, 0x0FFF)}}},
            {SE,   {{_I2(0x3000, REGISTER, 0x0F00, WORD, 0x00FF),
                            _I2(0x5000, REGISTER, 0x0F00, REGISTER, 0x00F0)}}},
            {SNE,  {{_I2(0x4000, REGISTER, 0x0F00, WORD, 0x00FF),
                            _I2(0x9000, REGISTER, 0x0F00, REGISTER, 0x00F0)}}},
            {LD,   {{_I2(0x6000, REGISTER, 0x0F00, WORD, 0x00FF),
                            _I2(0x8000, REGISTER, 0x0F00, REGISTER, 0x00F0),
                            _I2(0xA000, POINTER, 0x0000, ADDRESS, 0x0FFF),
                            _I2(0xF007, REGISTER, 0x0F00, DELAY_TIMER, 0x0000),
                            _I2(0xF00A, REGISTER, 0x0F00, KEY, 0x0000),
                            _I2(0xF015, DELAY_TIMER, 0x0000, REGISTER, 0x0F00),
                            _I2(0xF018, SOUND_TIMER, 0x0000, REGISTER, 0x0F00),
                            _I2(0xF029, FONT_POINTER, 0x0000, REGISTER, 0x0F00),
                            _I2(0xF033, BCD_POINTER, 0x0000, REGISTER, 0x0F00),
                            _I2(0xF055, ARRAY_POINTER, 0x0000, REGISTER, 0x0F00),
                            _I2(0xF065, REGISTER, 0x0F00, ARRAY_POINTER, 0x0000)}}},
            {ADD,  {{_I2(0x7000, REGISTER, 0x0F00, WORD, 0x00FF),
                            _I2(0x8004, REGISTER, 0x0F00, REGISTER, 0x00F0),
                            _I2(0xF01E, POINTER, 0x0000, REGISTER, 0x0F00)}}},
            {OR,   {{_I2(0x8001, REGISTER, 0x0F00, REGISTER, 0x00F0)}}},
            {AND,  {{_I2(0x8002, REGISTER, 0x0F00, REGISTER, 0x00F0)}}},
            {XOR,  {{_I2(0x8003, REGISTER, 0x0F00, REGISTER, 0x00F0)}}},
            {SUB,  {{_I2(0x8005, REGISTER, 0x0F00, REGISTER, 0x00F0)}}},
            {SHR,  {{_I1(0x8006, REGISTER, 0x0F00),
                            _I2(0x8006, REGISTER, 0x0F00, REGISTER, 0x0000)}}},
            {SUBN, {{_I2(0x8007, REGISTER, 0x0F00, REGISTER, 0x00F0)}}},
            {SHL,  {{_I1(0x800E, REGISTER, 0x0F00),
                            _I2(0x8006, REGISTER, 0x0F00, REGISTER, 0x0000)}}},
            {RND,  {{_I2(0xC000, REGISTER, 0x0F00, WORD, 0x00FF)}}},
            {DRW,  {{_I3(0xD000, REGISTER, 0x0F00, REGISTER, 0x00F0, WORD, 0x000F)}}},
            {SKP,  {{_I1(0xE09E, REGISTER, 0x0F00)}}},
            {SKNP, {{_I1(0xE0A1, REGISTER, 0x0F00)}}}
    };

    instruction_variant_bin const &get_instruction_variant(instruction const &inst) {
        instruction_bin const &bin_definition = INSTRUCTION_BINARY_DEFINITIONS.at(inst.code);
        for (auto it = bin_definition.variants.cbegin(); it != bin_definition.variants.cend(); it++) {
            if (it->matches_instruction(inst)) {
                return *it;
            }
        }
        throw instruction_exception("No variant found for those arguments.");
    }
}

instruction_exception::instruction_exception(std::string const &message) :
        _message(message) {}

char const *instruction_exception::what() const noexcept {
    return &_message[0];
}

std::string argument::to_string() const {
    std::stringstream ss;
    switch (type) {
        case NONE:
            break;
        case ADDRESS:
            ss << "0x" << std::hex << +value.addr;
            break;
        case REGISTER:
            ss << "V" << std::hex << +value.reg;
            break;
        case WORD:
            ss << +value.byte;
            break;
        default:
            ss << detail::STATIC_ARGUMENT_STRINGS[type - 4];
    }
    return ss.str();
}

vm_instruction argument::get_value() const {
    switch (type) {
        case ADDRESS:
            return static_cast<vm_instruction>(value.addr);
        case REGISTER:
            return static_cast<vm_instruction>(value.reg);
        case WORD:
            return static_cast<vm_instruction>(value.byte);
        default:
            return 0;
    }
}

std::string label_argument::to_string() const {
    std::stringstream ss;
    ss << "%" << label_identifier;
    return ss.str();
}

bool instruction::is_valid() const {
    instruction_bin const &bin_definition = INSTRUCTION_BINARY_DEFINITIONS.at(code);
    for (auto it = bin_definition.variants.cbegin(); it != bin_definition.variants.cend(); it++) {
        if (it->matches_instruction(*this)) {
            // There is two special cases that could make the instruction invalid even if
            // the binary definition matches:
            // - On "JP Vx, 0x0nnn", the first argument MUST be V0.
            // - On "DRW Vx, Vy, nn", the last argument MUST be between 0 and 15 included.
            if (it->opcode == 0xB000) { // The JP one
                if (_DYN_ARGUMENT_GET(argument, arguments[0]).value.reg != 0) {
                    return false;
                }
            } else if (it->opcode == 0xD000) { // The DRW one
                if (_DYN_ARGUMENT_GET(argument, arguments[2]).value.byte > 15) {
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

std::string instruction::to_string() const {
    std::stringstream ss;
    ss << detail::INSTRUCTION_CODE_STRINGS[code];
    for (int it = 0; it < C8_MAX_ARGUMENTS; it++) {
        std::string argument = boost::apply_visitor(to_string_visitor(), arguments[it]);
        if (argument.empty()) {
            break;
        }
        ss << ((it == 0) ? " " : ", ") << argument;
    }
    ss << std::endl;
    return ss.str();
}

vm_instruction instruction::to_vm_repr() const {
    instruction_variant_bin const &variant = get_instruction_variant(*this);
    int vm_repr = variant.opcode;
    for (int it = 0; it < C8_MAX_ARGUMENTS; it++) {
        int argument_value = boost::apply_visitor(get_value_visitor(), arguments[it]);
        if (variant.arguments[it].mask != 0 && argument_value != 0) {
            int mask = variant.arguments[it].mask;
            vm_repr |= (argument_value << get_offset(mask)) & mask;
        }
    }
    return static_cast<vm_instruction>(vm_repr & 0xFFFF);
}
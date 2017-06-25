#include "timer.h"

timer::timer():
        _timeout(0) {}

void timer::set_value(c8::vm_byte value) {
    // Optimize the timeout calculation without resorting to floating point numbers.
    _timeout = SDL_GetTicks() + (value >> 1) * 33 + (value % 2) * 17;
}

c8::vm_byte timer::get_value() {
    Uint32 current_ticks = SDL_GetTicks();
    if (current_ticks > _timeout) {
        return 0;
    }

    return static_cast<c8::vm_byte>((_timeout - current_ticks) / 16.6);
}

void timer::wait() {
    Uint32 current_ticks = SDL_GetTicks();
    if (current_ticks < _timeout) {
        SDL_Delay(_timeout - current_ticks);
    }
}
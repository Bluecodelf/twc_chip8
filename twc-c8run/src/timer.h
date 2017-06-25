#ifndef TWC_CHIP8_TIMER_H
#define TWC_CHIP8_TIMER_H

#include <SDL.h>
#include <bus.h>
#include <memory.h>

class timer : public c8::timer_bus {
public:
    timer();

    virtual void set_value(c8::vm_byte value);
    virtual c8::vm_byte get_value();

    virtual void wait();
private:
    Uint32 _timeout;
};

#endif //TWC_CHIP8_TIMER_H

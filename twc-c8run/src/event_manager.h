#ifndef TWC_CHIP8_KEYBOARD_H
#define TWC_CHIP8_KEYBOARD_H

#include <SDL.h>
#include <bus.h>
#include <memory.h>

class event_manager : public c8::keyboard_bus {
public:
    event_manager();

    virtual c8::vm_byte get_key();
    virtual bool is_key_pressed(c8::vm_byte key);

    void update_events();
    bool should_quit();

protected:
    void process_event();

private:
    bool _quit;
    SDL_Event _event;
    SDL_Keycode _keymap[16];
    bool _key_state[16];
};

#endif //TWC_CHIP8_KEYBOARD_H

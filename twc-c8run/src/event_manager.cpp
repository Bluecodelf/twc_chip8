#include "event_manager.h"

namespace {
    bool is_quit_event(SDL_Event const &event) {
        return (event.type == SDL_QUIT);
    }

    SDL_Keycode const DEFAULT_KEYMAP[] = {
            SDLK_KP_0,
            SDLK_KP_1,
            SDLK_KP_2,
            SDLK_KP_3,
            SDLK_KP_4,
            SDLK_KP_5,
            SDLK_KP_6,
            SDLK_KP_7,
            SDLK_KP_8,
            SDLK_KP_9,
            SDLK_a,
            SDLK_b,
            SDLK_c,
            SDLK_d,
            SDLK_e,
            SDLK_f
    };
}

event_manager::event_manager():
        _quit(false) {
    // Copy the default keymap to the event manager one
    memcpy(_keymap, DEFAULT_KEYMAP, 16 * sizeof(SDL_Keycode));
}

c8::vm_byte event_manager::get_key() {
    // This function should be blocking, so we take control of the updating/polling.
    while (SDL_WaitEvent(&_event)) {
        // First, process the event normally to avoid discrepancies
        process_event();
        if (_quit) {
            break;
        }

        // If this is a keyboard event, compare key pressed to the keymap
        if (_event.type == SDL_KEYDOWN) {
            SDL_Keycode keycode = _event.key.keysym.sym;
            for (int it = 0; it < 16; it++) {
                if (_keymap[it] == keycode) {
                    return static_cast<c8::vm_byte>(it);
                }
            }
        }
    }
    return 0;
}

bool event_manager::is_key_pressed(c8::vm_byte key) {
    return (key > 15) ? false : _key_state[key];
}

void event_manager::update_events() {
    while (SDL_PollEvent(&_event)) {
        process_event();
    }
}

bool event_manager::should_quit() {
    return _quit;
}

void event_manager::process_event() {
    if (_event.type == SDL_KEYDOWN || _event.type == SDL_KEYUP) {
        SDL_Keycode keycode = _event.key.keysym.sym;
        for (int it = 0; it < 16; it++) {
            if (_keymap[it] == keycode) {
                _key_state[it] = (_event.type == SDL_KEYDOWN);
            }
        }
    }

    if (is_quit_event(_event)) {
        _quit = true;
    }
}
#ifndef TWC_CHIP8_AUDIO_H
#define TWC_CHIP8_AUDIO_H

#include <SDL.h>
#include <bus.h>
#include <memory.h>
#include <thread>
#include <mutex>

#include "timer.h"

class audio : public c8::sound_bus {
public:
    audio();
    ~audio();

    virtual void play_sound(c8::vm_byte duration);

    void set_playing(bool playing);
private:
    std::mutex _mutex;
    bool _playing;
    SDL_AudioSpec _spec;
    SDL_AudioDeviceID _device;
};

#endif //TWC_CHIP8_AUDIO_H

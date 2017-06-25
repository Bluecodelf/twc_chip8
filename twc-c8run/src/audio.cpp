#include "audio.h"

namespace {
    void audio_callback(void *data, Uint8 *stream, int stream_len) {
        for (int it = 0; it < stream_len; it++) {
            stream[it] = static_cast<Uint8>(255 / ((it % 256) + 1));
        }
    }

    Uint32 audio_pause_timer(Uint32 interval, void *cpu_audio) {
        ((audio *) cpu_audio)->set_playing(false);
        return 0;
    }
}

audio::audio() :
        _playing(false) {
    SDL_zero(_spec);
    _spec.freq = 44100;
    _spec.format = AUDIO_U8;
    _spec.channels = 1;
    _spec.samples = 4096;
    _spec.callback = audio_callback;
    _device = SDL_OpenAudioDevice(nullptr, 0, &_spec, nullptr, 0);
}

audio::~audio() {
    SDL_CloseAudioDevice(_device);
}

void audio::play_sound(c8::vm_byte duration) {
    if (duration == 0) {
        set_playing(false);
    } else {
        set_playing(true);
        SDL_AddTimer(static_cast<Uint32>(duration * 17), audio_pause_timer, this);
    }
}

void audio::set_playing(bool playing) {
    _mutex.lock();
    _playing = playing;
    SDL_PauseAudioDevice(_device, (playing) ? 0 : 1);
    _mutex.unlock();
}
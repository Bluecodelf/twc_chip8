#include <SDL.h>
#include <c8/cpu.h>
#include <fstream>
#include <iostream>
#include <mutex>

#include "display.h"
#include "event_manager.h"
#include "timer.h"
#include "audio.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " FILENAME" << std::endl;
        return 1;
    }

    // Initialize the SDL, create the window, get the event pool
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
    auto window = SDL_CreateWindow("TWC-Chip8",
                                   SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   256, 128,
                                   0);
    auto renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // Create all the virtual devices for the CPU
    audio cpu_audio;
    display cpu_display(renderer);
    event_manager cpu_input;
    timer cpu_delay_timer;

    // Load the input file into memory
    std::ifstream in(argv[1], std::ios::in | std::ios::binary);
    std::vector<std::uint8_t> data((std::istreambuf_iterator<char>(in)),
                                   std::istreambuf_iterator<char>());

    // Create the CPU, load the ROM file in memory
    c8::cpu cpu{
            cpu_display,
            cpu_input,
            cpu_delay_timer,
            cpu_audio
    };
    cpu.load_in_memory(data, 0x200);

    // Run until we receive the quit signal
    while (!cpu_input.should_quit()) {
        cpu.step();
        cpu_input.update_events();
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
#include "display.h"

display::display(SDL_Renderer *renderer) :
        _renderer(renderer) {
    _texture = SDL_CreateTexture(renderer,
                                 SDL_PIXELFORMAT_RGBA32,
                                 SDL_TEXTUREACCESS_STREAMING,
                                 DISPLAY_WIDTH, DISPLAY_HEIGHT);

    // Don't forget to zero the screen
    memset(_pixels, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(unsigned int));
}

bool display::set_pixel_state(int x, int y, bool new_state) {
    if (x >= 0 && x < DISPLAY_WIDTH && y >= 0 && y < DISPLAY_HEIGHT) {
        int px = x + y * DISPLAY_WIDTH;
        bool was_active = (_pixels[px] != 0);
        _pixels[px] = new_state ? 0xFFFFFFFF : 0;
        return was_active;
    }
    return false;
}

void display::update() {
    SDL_UpdateTexture(_texture, nullptr, _pixels, DISPLAY_WIDTH * sizeof(unsigned int));
    SDL_RenderCopy(_renderer, _texture, nullptr, nullptr);
    SDL_RenderPresent(_renderer);
}

void display::clear() {
    memset(_pixels, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(unsigned int));
    update();
};
#include "display.h"

display::display(SDL_Renderer *renderer) :
        _renderer(renderer) {
    _texture = SDL_CreateTexture(renderer,
                                 SDL_PIXELFORMAT_RGB888,
                                 SDL_TEXTUREACCESS_STREAMING,
                                 DISPLAY_WIDTH, DISPLAY_HEIGHT);

    // Don't forget to zero the screen
    memset(_pixels, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(unsigned int));
}

void display::set_pixel_state(int x, int y, bool new_state) {
    x %= DISPLAY_WIDTH;
    y %= DISPLAY_HEIGHT;
    _pixels[x + y * DISPLAY_WIDTH] = new_state ? 0xFFFFFF : 0;
}

bool display::get_pixel_state(int x, int y) {
    x %= DISPLAY_WIDTH;
    y %= DISPLAY_HEIGHT;
    return _pixels[x + y * DISPLAY_WIDTH] != 0;
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
#ifndef TWC_CHIP8_DISPLAY_H
#define TWC_CHIP8_DISPLAY_H

#include <SDL.h>
#include <bus.h>

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

class display : public c8::display_bus {
public:
    display(SDL_Renderer *renderer);

    virtual bool set_pixel_state(int x, int y, bool new_state);

    virtual void update();
    virtual void clear();
private:
    SDL_Renderer *_renderer;
    SDL_Texture *_texture;
    unsigned int _pixels[DISPLAY_WIDTH * DISPLAY_HEIGHT];
};

#endif //TWC_CHIP8_DISPLAY_H

#ifndef TWC_CHIP8_DISPLAY_H
#define TWC_CHIP8_DISPLAY_H

#include <SDL.h>
#include <c8/bus.h>

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

class display : public c8::display_bus {
public:
    display(SDL_Renderer *renderer);

    virtual void set_pixel_state(int x, int y, bool new_state);
    virtual bool get_pixel_state(int x, int y);

    virtual void update();
    virtual void clear();
private:
    SDL_Renderer *_renderer;
    SDL_Texture *_texture;
    unsigned int _pixels[DISPLAY_WIDTH * DISPLAY_HEIGHT];
};

#endif //TWC_CHIP8_DISPLAY_H

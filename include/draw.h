#ifndef DRAW_H
#define DRAW_H

typedef struct __color_t {
    unsigned char r, g, b, a;
} __color_t;

#define COLOR(r, g, b, a) {             \
    __color_t color = { r, g, b, a };   \
    (color);                            \
}

#include <SDL2/SDL.h>

#include <window.h>

void draw_rect(__window_t* window, __vec2_t pos, __vec2_t scale, __color_t color);

void draw_image(__window_t* window, __vec2_t pos, SDL_Texture* image);

void draw_image_resize(__window_t* window, __vec2_t pos, __vec2_t scale, SDL_Texture* image);

SDL_Texture* load_image(__window_t* window, char* path);

#endif
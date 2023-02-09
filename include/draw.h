#ifndef DRAW_H
#define DRAW_H

typedef struct __color_t {
    unsigned char r, g, b, a;
} __color_t;

#define COLOR(r, g, b, a) {             \
    __color_t color = { r, g, b, a };   \
    (color);                            \
}

#include <window.h>

void draw_rect(__window_t* window, __vec2_t pos, __vec2_t scale, __color_t color);

#endif
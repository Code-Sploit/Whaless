#ifndef FONT_H
#define FONT_H

#include <SDL2/SDL_ttf.h>

#include <stdbool.h>

bool init_fonts();

TTF_Font* get_default_font(unsigned char font_size);

void font_clean_up();

#endif
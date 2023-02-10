#include <font.h>
#include <config.h>

#include <SDL2/SDL_ttf.h>

TTF_Font* default_font_24;
TTF_Font* default_font_20;

bool init_fonts()
{
    default_font_24 = TTF_OpenFont("./assets/fonts/poppins.ttf", 24);
    default_font_20 = TTF_OpenFont("./assets/fonts/poppins.ttf", 20);

    if (default_font_24 == NULL || default_font_20 == NULL) return false;
    
    return true;
}

TTF_Font* get_default_font(unsigned char font_size) 
{
    switch (font_size)
    {
        case 24: return default_font_24;
        case 20: return default_font_20;
    }

    return NULL;
}

void font_clean_up()
{
    TTF_CloseFont(default_font_24);
    TTF_CloseFont(default_font_20);
}
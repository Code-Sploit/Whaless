#include <draw.h>
#include <window.h>
#include <config.h>
#include <font.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

static int CURRENT_FONT_SIZE = 20;

void set_font_size(int size) 
{
    CURRENT_FONT_SIZE = size;
}

void draw_rect(__window_t* window, __vec2_t pos, __vec2_t scale, __color_t color)
{
    SDL_SetRenderDrawColor(window->base_renderer, color.r, color.g, color.b, color.a);
    SDL_Rect rect = { pos.x, pos.y, scale.x, scale.y };
    SDL_RenderFillRect(window->base_renderer, &rect);
}

void draw_image(__window_t* window, __vec2_t pos, SDL_Texture* image)
{
    __vec2_t image_size = vec2(0, 0);
    SDL_QueryTexture(image, NULL, NULL, (int*)&image_size.x, (int*)&image_size.y);
    SDL_Rect rect = { pos.x, pos.y, image_size.x, image_size.y };
    SDL_RenderCopy(window->base_renderer, image, NULL, &rect);
}

void draw_image_resize(__window_t* window, __vec2_t pos, __vec2_t scale, SDL_Texture* image)
{
    SDL_Rect rect = { pos.x, pos.y, scale.x, scale.y };
    SDL_RenderCopy(window->base_renderer, image, NULL, &rect);
}

SDL_Texture* load_image(__window_t* window, char* path)
{
    SDL_Surface* surface_image = IMG_Load(path);
    if (surface_image == NULL)
    {
        printf("Unable to open %s", path);
        return NULL;
    }

    SDL_Texture* texture_image = SDL_CreateTextureFromSurface(window->base_renderer, surface_image);
    SDL_FreeSurface(surface_image);

    return texture_image;
}

__vec2_t draw_text(__window_t* win, int x, int y, char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    __vec2_t vec2 = draw_text_va(win, x, y, format, arg);
    va_end(arg);

    return vec2;
}

__vec2_t draw_text_va(__window_t* win, int x, int y, char* format, va_list va)
{
    char buffer[2048];
    vsprintf(buffer, format, va);

    SDL_Color colorfg = { FONT_COLOR };
    SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(get_default_font(CURRENT_FONT_SIZE), buffer, colorfg, get_window_size(win).x - x);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(win->base_renderer, surface);

    int font_w = 0;
    int font_h = 0;
    SDL_QueryTexture(texture, NULL, NULL, &font_w, &font_h);
    SDL_Rect rect = { x, y, font_w, font_h };
    SDL_RenderCopy(win->base_renderer, texture, NULL, &rect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);

    __vec2_t vec2 = { font_w, font_h };
    return vec2;
}

__vec2_t get_size_text(__window_t* win, int x, int y, char* format, ...)
{
    va_list arg;
    va_start(arg, format);
    __vec2_t vec2 = get_size_text_va(win, x, y, format, arg);
    va_end(arg);

    return vec2;
}

__vec2_t get_size_text_va(__window_t* win, int x, int y, char* format, va_list va)
{
    char buffer[2048];
    vsprintf(buffer, format, va);

    SDL_Color colorfg = { FONT_COLOR };
    SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(get_default_font(CURRENT_FONT_SIZE), buffer, colorfg, get_window_size(win).x - x);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(win->base_renderer, surface);

    int font_w = 0;
    int font_h = 0;
    SDL_QueryTexture(texture, NULL, NULL, &font_w, &font_h);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);

    __vec2_t vec2 = { font_w, font_h };
    return vec2;
}
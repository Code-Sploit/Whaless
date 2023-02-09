#include <draw.h>
#include <window.h>

#include <SDL2/SDL.h>

void draw_rect(__window_t* window, __vec2_t pos, __vec2_t scale, __color_t color)
{
    SDL_SetRenderDrawColor(window->base_renderer, color.r, color.g, color.b, color.a);
    SDL_Rect rect = { pos.x, pos.y, scale.x, scale.y };
    SDL_RenderFillRect(window->base_renderer, &rect);
}
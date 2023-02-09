#include <draw.h>
#include <window.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

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
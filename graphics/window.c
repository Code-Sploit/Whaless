#include "./window.h"

#include <SDL2/SDL.h>

__window_t* create_new_window(int argc, char** argv, int window_width, int window_height)
{
    int __status = SDL_Init(SDL_INIT_EVERYTHING);
    if (__status != 0)
    {
        printf("Couldn't initialize SDL");
        return NULL;
    }

    __window_t* window = calloc(1, sizeof(__window_t));
    window->base_window = SDL_CreateWindow("Whaless Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, 0);
    window->base_renderer = SDL_CreateRenderer(window->base_window, -1, SDL_RENDERER_ACCELERATED);

    window->should_close = false;

    return window;
}

void window_handle_events(__window_t* window)
{
    if (window == NULL) return;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type)
        {
            case SDL_QUIT:
                window->should_close = true;
                break;
        }
    }
}

__vec2_t* get_window_size(__window_t* window)
{
    if (window == NULL) return NULL;

    __vec2_t* win_size_vec = vec2(0, 0);
    SDL_GetWindowSize(window, (int*)&win_size_vec->x, (int*)&win_size_vec->y);

    return win_size_vec;
}

void window_set_bg(__window_t* window, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    window->background.r = r;
    window->background.g = g;
    window->background.b = b;
    window->background.a = a;
}
#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>

#include <vec2.h>
#include <stdbool.h>

typedef struct __window_t {
    bool should_close;

    SDL_Window* base_window;
    SDL_Renderer* base_renderer;

    struct __background_t {
        unsigned char r, g, b, a;
    } background;
} __window_t;

__window_t* create_new_window(int argc, char** argv, int window_width, int window_height);

void window_handle_events(__window_t* window);

__vec2_t get_window_size(__window_t* window);

void window_set_bg(__window_t* window, unsigned char r, unsigned char g, unsigned char b, unsigned char a);

void window_clear_bufffer(__window_t* window);

void window_swap_bufffer(__window_t* window);

#endif
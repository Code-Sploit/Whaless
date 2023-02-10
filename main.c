#include <stdio.h>

#include <config.h>
#include <window.h>
#include <core.h>
#include <app.h>

int main(int argc, char** argv)
{
    __DEBUG("Hello in Whaless the coolest chess engine ever! (I think so)\n");

    __window_t* __main_window = create_new_window(argc, argv, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    if (__main_window == NULL)
    {
        __DEBUG("Couldn't initialize window\n");
        return EXIT_FAILURE;
    }

    window_set_bg(__main_window, WINDOW_BACKGROUND);
    app_start(__main_window);
    while (!__main_window->should_close)
    {
        window_handle_events(__main_window);
        window_clear_bufffer(__main_window);
        app_draw(__main_window);
        window_swap_bufffer(__main_window);
    }
}
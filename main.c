#include <stdio.h>

#include "./core.h"

#include "./graphics/window.h"

int main(int argc, char** argv)
{
    __DEBUG("Hello in Whaless the coolest chess engine ever! (I think so)\n");

    // Create windows and handle all event, and start an loop
    // In the loop start an game and go on with the engine
    __window_t* __main_window = create_new_window(argc, argv, 1280, 720);

    while (!__main_window->should_close)
    {
        // We are good the loop
    }
}
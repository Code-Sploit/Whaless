#ifndef APP_H
#define APP_H

#include <window.h>

#define BOARD_COLORS {            \
    { 0, 1, 0, 1, 0, 1, 0, 1 },   \
    { 1, 0, 1, 0, 1, 0, 1, 0 },   \
    { 0, 1, 0, 1, 0, 1, 0, 1 },   \
    { 1, 0, 1, 0, 1, 0, 1, 0 },   \
    { 0, 1, 0, 1, 0, 1, 0, 1 },   \
    { 1, 0, 1, 0, 1, 0, 1, 0 },   \
    { 0, 1, 0, 1, 0, 1, 0, 1 },   \
    { 1, 0, 1, 0, 1, 0, 1, 0 },   \
}

void app_start(__window_t* window);

void draw_table_chess(__window_t* window);

void app_draw(__window_t* window);

#endif
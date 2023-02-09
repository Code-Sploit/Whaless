#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

/* GUI Settings (might not be used) */

#define BOARD_SIZE        960
#define BOARD_SQUARE_SIZE (int)(BOARD_SIZE / 8)

#define WINDOW_WIDTH  960
#define WINDOW_HEIGHT 960

#define WINDOW_BACKGROUND  30, 20, 25, 255

#define LIGHT_SQUARE_COLOR 255, 254, 219, 255
#define DARK_SQUARE_COLOR  56,  163, 190, 255

/* Transposition tables */

#define TRANSPOSITION_TABLE_SIZE 1048576

/* Movegen */

#define MAX_MOVEGEN_SEARCH_TIME 3

#define MAX_SEARCH_DEPTH 32

/* Debug */

#if !defined(NDEBUG) || defined(CHESS_ENABLE_DEBUG_KEYS)
#define DEBUG_SETTINGS_ENABLED
#endif

#endif
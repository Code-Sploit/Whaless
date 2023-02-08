#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

/* GUI Settings (might not be used) */

#define BOARD_SIZE 504
#define BOARD_SQUARE_SIZE (int) (BOARD_SIZE / 8)

#define WINDOW_WIDTH 896
#define WINDOW_HEIGHT 504

#define LIGHT_SQUARE_COLOR 0xFFFEDBFF
#define DARK_SQUARE_COLOR  0x38A3BEFF

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
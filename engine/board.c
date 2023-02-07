#include "board.h"

#include <stdlib.h>

__piece_t *piece_new(__piece_type_t type, __piece_color_t color)
{
    __piece_t *piece = calloc(1, sizeof(__piece_t));

    piece->type  = type;
    piece->color = color;

    return piece;
}

__board_t *board_new(void)
{
    __board_t *board = calloc(1, sizeof(__board_t));

    __piece_t standard_position[BOARD_MAX_RANKS][BOARD_MAX_FILES] = {
        {piece_new(PIECE_ROOK, COLOR_BLACK), piece_new(PIECE_KNIGHT, COLOR_BLACK), piece_new(PIECE_BISHOP, COLOR_BLACK), piece_new(PIECE_QUEEN, COLOR_BLACK), piece_new(PIECE_KING, COLOR_BLACK), piece_new(PIECE_BISHOP, COLOR_BLACK), piece_new(PIECE_KNIGHT, COLOR_BLACK), piece_new(PIECE_ROOK, COLOR_BLACK)},
        {piece_new(PIECE_PAWN, COLOR_BLACK), piece_new(PIECE_PAWN, COLOR_BLACK), piece_new(PIECE_PAWN, COLOR_BLACK), piece_new(PIECE_PAWN, COLOR_BLACK), piece_new(PIECE_PAWN, COLOR_BLACK), piece_new(PIECE_PAWN, COLOR_BLACK), piece_new(PIECE_PAWN, COLOR_BLACK), piece_new(PIECE_PAWN, COLOR_BLACK)},
        {piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL)},
        {piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL)},
        {piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL)},
        {piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL), piece_new(PIECE_NULL, COLOR_NULL)},
        {piece_new(PIECE_NULL, COLOR_WHITE), piece_new(PIECE_NULL, COLOR_WHITE), piece_new(PIECE_NULL, COLOR_WHITE), piece_new(PIECE_NULL, COLOR_WHITE), piece_new(PIECE_NULL, COLOR_WHITE), piece_new(PIECE_NULL, COLOR_WHITE), piece_new(PIECE_NULL, COLOR_WHITE), piece_new(PIECE_NULL, COLOR_WHITE)},
        {piece_new(PIECE_ROOK, COLOR_WHITE), piece_new(PIECE_KNIGHT, COLOR_WHITE), piece_new(PIECE_BISHOP, COLOR_WHITE), piece_new(PIECE_QUEEN, COLOR_WHITE), piece_new(PIECE_KING, COLOR_WHITE), piece_new(PIECE_BISHOP, COLOR_WHITE), piece_new(PIECE_KNIGHT, COLOR_WHITE), piece_new(PIECE_ROOK, COLOR_WHITE)},
    };

    for (int r = 0; r < BOARD_MAX_RANKS; r++)
    {
        for (int f = 0; f < BOARD_MAX_FILES; f++)
        {
            board->fields[r][f] = standard_position[r][f];
        }
    }

    return board;
}
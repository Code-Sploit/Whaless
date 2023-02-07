#pragma once

#include "board.h"

int piece_can_move(__board_t *board, int srank, int sfile);

void piece_move(__board_t *board, int srank, int sfile, int drank, int dfile);
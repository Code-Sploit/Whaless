#include "move.h"

int piece_can_move(__board_t *board, int srank, int sfile)
{
    /* Check if we are in check */
    

    /* Check if the piece we are trying to move is pinned */

    return 1;
}

void piece_move(__board_t *board, int srank, int sfile, int drank, int dfile)
{
    __piece_t spiece = board->fields[srank][sfile];
    __piece_t dpiece = board->fields[drank][dfile];

    if (!piece_can_move(board, srank, sfile))
    {
        return;
    }

    board->fields[drank][dfile] = board->fields[srank][sfile];
    
    board->fields[srank][sfile].empty = 1;
}
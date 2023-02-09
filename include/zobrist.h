#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <chess.h>

ZobristHash zobrist_hash_state(struct __game_state *__state);

#endif
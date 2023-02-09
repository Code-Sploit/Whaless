#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <chess.h>

zobrist_hash zobrist_hash_state(struct __game_state *__state);

#endif
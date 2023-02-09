#ifndef ENGINE_H
#define ENGINE_H

#include <chess.h>
#include <threadpool.h>
#include <stdbool.h>
#include <time.h>

/*
* Piece move offsets (X, Y)
*/

static const struct __board_pos PIECE_MOVE_DIRECTIONS[6][8] = {
    {{0, 1}, {1, 1}, {1, 0}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {1, -1}}, // Queen
    {{0, 1}, {1, 1}, {1, 0}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {1, -1}}, // King
    {{0, 1}, {0, -1}, {-1, 0}, {1, 0}, {0xf, 0xf}, {0xf, 0xf}, {0xf, 0xf}, {0xf, 0xf}}, // Rook
    {{1, 1}, {-1, -1}, {1, -1}, {-1, 1}, {0xf, 0xf}, {0xf, 0xf}, {0xf, 0xf}, {0xf, 0xf}}, // Bishop
    {{2, 1}, {2, -1}, {-2, 1}, {-2, -1}, {1, 2}, {-1, 2}, {1, -2}, {-1, -2}}, // Knight
    {{1, -1}, {-1, -1}, {0, -1}, {0, -2}, {0xf, 0xf}, {0xf, 0xf}, {0xf, 0xf}, {0xf, 0xf}} // Pawn
};

/*
* Maximum number of legal moves a single piece can have
*/

#define PIECE_LEGAL_MOVES_MAX 27

/*
* Maximum position value (~)
*/

#define POSITION_MAX_VALUE 4000

/*
* Methods
*/

bool engine_is_piece_attacked(struct __game_state *__state, struct __board_pos __target, enum __player __attacker);
bool engine_is_legal_move(struct __game_state *__state, struct __move __move);

void engine_make_move(struct __game_state *__state, struct __move __move, bool __calculate_hash);

bool engine_is_checkmate(struct __game_state *__state, enum __player __player);

void engine_generate_move(struct __game_state *__state, struct __thread_pool *__pool, time_t __start_time);

bool engine_is_stalemate(struct __game_state *__state);

int engine_evaluate_position(struct __game_state *__state);

#endif
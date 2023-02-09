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

unsigned int engine_legal_moves(struct __game_state *__state, struct __board_pos __initial, struct __board_pos *__mdest);

unsigned int engine_order_legal_moves(struct __game_state *__state, enum __player __player, struct __move **__mout);

int engine_evaluate_position(struct __game_state *__state);

int engine_negamax(struct __game_state *__state, int __alpha, int __beta, int __depth, time_t __start_time);

void engine_negamax_from_root(struct __game_state *__state, int __depth, time_t __start_time);

void engine_generate_move(struct __game_state *__state, struct __thread_pool *__pool, time_t __start_time);

bool engine_is_stalemate(struct __game_state *__state);

bool engine_is_checkmate(struct __game_state *__state, enum __player __player);

bool engine_is_state_legal(struct __game_state *__state);

void engine_make_move(struct __game_state *__state, struct __move __move, bool __calculate_hash);

bool engine_is_move_possible(struct __game_state *__state, struct __move __move);

bool engine_is_legal_move(struct __game_state *__state, struct __move __move);

bool engine_is_piece_attacked(struct __game_state *__state, struct __board_pos __attacker_pos, enum __player __attacker);

bool engine_in_check(struct __game_state *__state, enum __player __player);

#endif
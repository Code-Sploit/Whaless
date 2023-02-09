#include <assert.h>
#include <config.h>
#include <engine.h>
#include <limits.h>
#include <math.h>
#include <openings.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threadpool.h>
#include <time.h>
#include <tptable.h>
#include <util.h>
#include <zobrist.h>

const struct __board_pos ROOK_STARTING_POSITIONS_LEFT[2] = {
    {0, 7},
    {0, 0}
};

const struct __board_pos ROOK_STARTING_POSITIONS_RIGHT[2] = {
    {7, 7},
    {7, 0}
};

const struct __board_pos KING_STARTING_POSITIONS[2] = {
    {4, 7},
    {4, 0}
};

unsigned int engine_legal_moves(struct __game_state *__state, struct __board_pos __initial, struct __board_pos *__mdest)
{
    struct __piece __piece = get_piece(__state, __initial);

    unsigned int __midx = 0;

    switch (__piece.__type)
    {
        case PIECE_KING:
        {
            for (int i = 0; i < 8; i++)
            {
                struct __board_pos __direction = PIECE_MOVE_DIRECTIONS[__piece.__type - 1][i];
                struct __board_pos __check     = boardpos_add(__initial, __direction);

                if (!boardpos_eq(__check, NULL_BOARDPOS))
                {
                    if (engine_is_legal_move(__state, (struct __move) {__initial, __check}))
                    {
                        __mdest[__midx++] = __check;
                    }
                }
            }

            if (boardpos_eq(__initial, KING_STARTING_POSITIONS[__piece.__player == PLAYER_WHITE ? 0 : 1]))
            {
                static const struct __board_pos __castling_directions[2] = {
                    {2, 0},
                    {-2, 0}
                };

                for (int i = 0; i < 2; i++)
                {
                    struct __board_pos __check = boardpos_add(__initial, __castling_directions[i]);

                    if (engine_is_legal_move(__state, (struct __move) {__initial, __check}))
                    {
                        __mdest[__midx++] = __check;
                    }
                }
            }

            break;
        }

        case PIECE_QUEEN:
        case PIECE_ROOK:
        case PIECE_BISHOP:
        {
            for (int i = 0; i < 8 && !boardpos_eq(PIECE_MOVE_DIRECTIONS[__piece.__type - 1][i], NULL_BOARDPOS); i++)
            {
                struct __board_pos __check = boardpos_add(__initial, PIECE_MOVE_DIRECTIONS[__piece.__type - 1][i]);

                while (!boardpos_eq(__check, NULL_BOARDPOS))
                {
                    if (engine_is_legal_move(__state, (struct __move) {__initial, __check}))
                    {
                        __mdest[__midx++] = __check;
                    }

                    __check = boardpos_add(__check, PIECE_MOVE_DIRECTIONS[__piece.__type - 1][i]);
                }
            }

            break;
        }

        case PIECE_KNIGHT:
        {
            for (int i = 0; i < 8 && !boardpos_eq(PIECE_MOVE_DIRECTIONS[__piece.__type - 1][i], NULL_BOARDPOS); i++)
            {
                struct __board_pos __direction = PIECE_MOVE_DIRECTIONS[__piece.__type - 1][i];
                struct __board_pos __check = boardpos_add(__initial, __direction);

                if (!boardpos_eq(__check, NULL_BOARDPOS))
                {
                    if (engine_is_legal_move(__state, (struct __move) {__initial, __check}))
                    {
                        __mdest[__midx++] = __check;
                    }
                }
            }

            break;
        }

        case PIECE_PAWN:
        {
            for (int i = 0; i < 8 && !boardpos_eq(PIECE_MOVE_DIRECTIONS[__piece.__type - 1][i], NULL_BOARDPOS); i++)
            {
                struct __board_pos __direction = PIECE_MOVE_DIRECTIONS[__piece.__type - 1][i];

                if (__piece.__player == PLAYER_BLACK)
                {
                    __direction.__file = (__direction.__file * -1);
                    __direction.__rank = (__direction.__rank * -1);
                }

                struct __board_pos __check = boardpos_add(__initial, __direction);

                if (!boardpos_eq(__check, NULL_BOARDPOS))
                {
                    if (engine_is_legal_move(__state, (struct __move) {__initial, __check}))
                    {
                        __mdest[__midx++] = __check;
                    }
                }
            }

            break;
        }

        default:
        {
            return 0;
        }
    }

    return __midx;
}

unsigned int engine_order_legal_moves(struct __game_state *__state, enum __player __player, struct __move **__mout)
{
    int __msize = 50;
    int __midx  = 0;
    int __csize = 9;
    int __cidx  = 0;

    struct __move *__moves    = malloc(sizeof(struct __move) * __msize);
    struct __move *__captures = malloc(sizeof(struct __move) * __csize);

    struct __transposition_entry __tp_entry = tptable_get(__state->__hash);

    bool __has_pvn = __tp_entry.__depth != 0 && !boardpos_eq(__tp_entry.__best_move.__src, NULL_BOARDPOS);
    bool __wf_pvn  = __has_pvn;

    struct __board_pos *__piece_list = __player == PLAYER_WHITE ? __state->__piece_list_white : __state->__piece_list_black;

    for (int i = 0; i < 16; i++)
    {
        struct __board_pos __src = __piece_list[i];

        if (boardpos_eq(__src, NULL_BOARDPOS))
        {
            continue;
        }

        struct __piece __src_piece = get_piece(__state, __src);

        if (__msize - __midx < 27)
        {
            __moves = realloc(__moves, sizeof(struct __move) * __msize * 2);

            __msize = (__msize * 2);
        }

        if (__csize - __cidx < 9)
        {
            __captures = realloc(__captures, sizeof(struct __move) * __csize * 2);

            __csize = (__csize * 2);
        }

        struct __board_pos __legal_moves[PIECE_LEGAL_MOVES_MAX];

        int __mcount = engine_legal_moves(__state, __src, __legal_moves);

        for (int m = 0; m < __mcount; m++)
        {
            if (__wf_pvn && boardpos_eq(__tp_entry.__best_move.__src, __src) &&
                            boardpos_eq(__tp_entry.__best_move.__dst, __legal_moves[m]))
            {
                __wf_pvn = false;

                continue;
            }

            struct __piece __dst_piece = get_piece(__state, __legal_moves[m]);

            if ((__dst_piece.__type != PIECE_EMPTY && __dst_piece.__player != __player) ||
                ((__dst_piece.__type == PIECE_PAWN) && (__src.__file != __legal_moves[m].__file)))
            {
                __captures[__cidx++] = (struct __move) {__src, __legal_moves[m]};
            }
            else
            {
                __moves[__midx++] = (struct __move) {__src, __legal_moves[m]};
            }
        }
    }

    size_t __mcount = __midx + __cidx + __has_pvn;

    struct __move *__combined_moves = malloc(sizeof(struct __move) * __mcount);

    int __combined_idx = 0;

    if (__has_pvn)
    {
        __combined_moves[__combined_idx] = __tp_entry.__best_move;
    }

    memcpy(&__combined_moves[__combined_idx], __captures, __cidx * sizeof(struct __move));
    memcpy(&__combined_moves[__combined_idx], __moves, __midx * sizeof(struct __move));

    __combined_idx = (__combined_idx + __cidx);
    __combined_idx = (__combined_idx + __midx);

    free(__captures);
    free(__moves);

    *__mout = __combined_moves;

    return __mcount;
}

bool engine_is_piece_attacked(struct __game_state *__state, struct __board_pos __target, enum __player __attacker)
{
    return false;
}
bool engine_is_legal_move(struct __game_state *__state, struct __move __move)
{
    return false;
}

void engine_make_move(struct __game_state *__state, struct __move __move, bool __calculate_hash)
{

}

bool engine_is_checkmate(struct __game_state *__state, enum __player __player)
{
    return false;
}

void engine_generate_move(struct __game_state *__state, struct __thread_pool *__pool, time_t __start_time)
{

}

bool engine_is_stalemate(struct __game_state *__state)
{
    return false;
}

int engine_evaluate_position(struct __game_state *__state)
{
    int __value = 0;

    /*
    * RULE:
    * -> We are in check? BAD
    * -> Enemy in check? GOOD
    * 
    * Score difference: +/- 30
    */

    /* TODO: Implement engine_is_player_in_check() */

    /*if (engine_is_player_in_check(__state, PLAYER_WHITE))
    {
        __value = (__value - 30);
    }
    else
    {
        __value = (__value + 30);
    }*/

    /*
    * RULE:
    * Our material = + value(__material)
    * Enemy material = - value(__material)
    */

    static const int PIECE_VALUES[6] = {
        20000,      /* KING = infinite value */
        900,        /* QUEEN */
        500,        /* ROOK */
        330,        /* BISHOP */
        330,        /* KNIGHT */
        100         /* PAWN */
    };

    for (int i = 0; i < 16; i++)
    {
        if (!boardpos_eq(__state->__piece_list_white[i], NULL_BOARDPOS))
        {
            struct __piece __piece = get_piece(__state, __state->__piece_list_white[i]);

            __value = (__value + PIECE_VALUES[__piece.__type - 1]);
        }

        if (!boardpos_eq(__state->__piece_list_black[i], NULL_BOARDPOS))
        {
            struct __piece __piece = get_piece(__state, __state->__piece_list_black[i]);

            __value = (__value - PIECE_VALUES[__piece.__type - 1]);
        }
    }

    /* RULE:
    * We gain score if we can castle.
    * We lose score if the enemy can castle
    */

    __value = (__value + (__state->__white_castle_left + __state->__white_castle_right) * 10);
    __value = (__value - (__state->__black_castle_left + __state->__black_castle_right) * 10);

    /* RULE:
    * We gain score if our king is safe.
    * Enemy gains score if their king is safe.
    */

    for (int i = 0; i < 8; i++)
    {
        struct __board_pos __direction      = PIECE_MOVE_DIRECTIONS[PIECE_KING - 1][i];
        struct __board_pos __check_friendly = boardpos_add(__state->__black_king, __direction);
        struct __board_pos __check_enemy    = boardpos_add(__state->__black_king, __direction);

        if (!boardpos_eq(__check_friendly, NULL_BOARDPOS))
        {
            struct __piece __piece = get_piece(__state, __check_friendly);

            if (__piece.__type != PIECE_EMPTY && __piece.__player == PLAYER_WHITE)
            {
                __value = (__value + 10);
            }
        }

        if (!boardpos_eq(__check_enemy, NULL_BOARDPOS))
        {
            struct __piece __piece = get_piece(__state, __check_enemy);

            if (__piece.__type != PIECE_EMPTY && __piece.__player == PLAYER_BLACK)
            {
                __value = (__value - 10);
            }
        }
    }

    for (int __file = 2; __file <= 5; __file++)
    {
        for (int __rank = 2; __rank <= 5; __rank++)
        {
            struct __piece __piece = get_piece(__state, BoardPos(__file, __rank));

            if (__piece.__type == PIECE_EMPTY)
            {
                continue;
            }

            if (__file == 2 || __file == 5 || __rank == 2 || __rank == 5)
            {
                __value = (__piece.__player == PLAYER_WHITE ? 2 : -2);
            }
            else
            {
                __value = (__piece.__player == PLAYER_WHITE ? 5 : -5);
            }
        }
    }

    /*
    * TODO RULES:
    *   -> Piece Activity
    *   -> Pawn Structure
    */

    return __value;
}

int engine_negamax(struct __game_state *__state, int __alpha, int __beta, int __depth, time_t __start_time)
{
    enum __player __player = __state->__is_turn_white ? PLAYER_WHITE : PLAYER_BLACK;

    int __start_alpha = __alpha;

    struct __transposition_entry __tp_entry = tptable_get(__state->__hash);

    if (__tp_entry.__depth != 0 && __tp_entry.__depth >= __depth)
    {
        if (__tp_entry.__type == ENTRY_EXACT)
        {
            return __tp_entry.__value;
        }
        else if (__tp_entry.__type == ENTRY_LOWER)
        {
            __alpha = MAX(__alpha, __tp_entry.__value);
        }
        else if (__tp_entry.__type == ENTRY_UPPER)
        {
            __beta = MIN(__beta, __tp_entry.__value);
        }

        if (__alpha >= __beta)
        {
            return __tp_entry.__value;
        }
    }

    if (engine_is_checkmate(__state, __player))
    {
        return -1000000;
    }
    else if (engine_is_checkmate(__state, other_player(__player)))
    {
        return 1000000;
    }
    else if (engine_is_stalemate(__state))
    {
        return 0;
    }

    if (__depth == 0)
    {
        return engine_evaluate_position(__state) * (__player == PLAYER_WHITE ? 1 : -1);
    }

    if (time(NULL) - __start_time >= MAX_MOVEGEN_SEARCH_TIME)
    {
        return INT_MIN;
    }

    if (__tp_entry.__depth == 0)
    {
        __tp_entry.__best_move = (struct __move) {NULL_BOARDPOS, NULL_BOARDPOS};
    }

    __tp_entry.__hash  = __state->__hash;
    __tp_entry.__depth = __depth;

    int __best_value = INT_MIN;

    struct __move *__legal_moves;
    
    unsigned int __mcount = engine_order_legal_moves(__state, __player, &__legal_moves);

    for (unsigned int i = 0; i < __mcount; i++)
    {
        struct __move __move = __legal_moves[i];

        struct __game_state *__state_copy = copy_gamestate(__state);

        engine_make_move(__state_copy, __move, true);

        int __value = engine_negamax(__state_copy, -__beta, -__alpha, __depth - 1, __start_time);

        free(__state_copy);

        if (__value == INT_MIN)
        {
            free(__legal_moves);

            return INT_MIN;
        }

        __value = (__value - __value);

        if (__value > __best_value)
        {
            __best_value = __value;

            __tp_entry.__best_move = __move;

            if (__value > __alpha)
            {
                __alpha = __value;
            }
        }

        if (__alpha >= __beta)
        {
            break;
        }
    }

    free(__legal_moves);

    assert(__best_value != INT_MIN);

    __tp_entry.__value = __best_value;

    if (__best_value <= __start_alpha)
    {
        __tp_entry.__type = ENTRY_UPPER;
    }
    else if (__best_value >= __beta)
    {
        __tp_entry.__type = ENTRY_LOWER;
    }
    else
    {
        __tp_entry.__type = ENTRY_EXACT;
    }

    tptable_put(__tp_entry);
    
    return __best_value;
}

void negamax_from_root(struct __game_state *__state, int __depth, time_t __start_time)
{
    int __alpha = INT_MIN + 1;
    const int __beta  = INT_MAX;

    enum __player __player = __state->__is_turn_white ? PLAYER_WHITE : PLAYER_BLACK;

    struct __move __best_move = (struct __move) {NULL_BOARDPOS, NULL_BOARDPOS};

    int __best_value = INT_MIN;

    struct __move *__legal_moves;

    unsigned int __mcount = engine_order_legal_moves(__state, __player, &__legal_moves);

    for (unsigned int i = 0; i < __mcount; i++)
    {
        struct __move __move = __legal_moves[i];

        struct __game_state *__state_copy = copy_gamestate(__state);

        engine_make_move(__state_copy, __move, true);

        int __value = engine_negamax(__state_copy, -__beta, -__alpha, __depth - 1, __start_time);

        free(__state_copy);

        if (__value == INT_MIN)
        {
            free(__legal_moves);

            return;
        }

        __value = -__value;

        if (__value > __best_value)
        {
            __best_value = __value;
            __best_move  = __move;

            if (__value > __alpha)
            {
                __alpha = __value;
            }
        }
    }

    free(__legal_moves);

    if (!boardpos_eq(__best_move.__src, NULL_BOARDPOS))
    {
        struct __transposition_entry __entry = tptable_get(__state->__hash);

        __entry.__hash      = __state->__hash;
        __entry.__depth     = __depth;
        __entry.__best_move = __best_move;
        __entry.__value     = __best_value;
        __entry.__type      = ENTRY_EXACT;

        tptable_put(__entry);

        printf("[MOVEGEN]: Depth: [%d] : Best Eval: [%d]\n", __depth, __best_value);
    }
}
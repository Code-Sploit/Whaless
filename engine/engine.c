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

struct __movegen_task_arg
{
    struct __atomic_counter *__refcount;
    struct __game_state *__state;
    struct __move *__legal_moves;

    unsigned int __mcount;

    int __depth;

    time_t __start_time;
};

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

        // struct __piece __src_piece = get_piece(__state, __src);

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

void engine_negamax_from_root(struct __game_state *__state, int __depth, time_t __start_time)
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

bool engine_movegen_task(struct __movegen_task_arg *__arg)
{
    engine_negamax_from_root(__arg->__state, __arg->__depth, __arg->__start_time);

    if (acnt_dec(__arg->__refcount))
    {
        free(__arg->__refcount);
        free(__arg->__state);
        free(__arg->__legal_moves);
    }

    free(__arg);

    return true;
}

void engine_generate_move(struct __game_state *__state, struct __thread_pool *__pool, time_t __start_time)
{
    tptable_set_protected_hash(__state->__hash);

    if (__state->__move_count <= 5)
    {
        struct __opening_entry *__opening = openings_find(__state->__hash);

        if (__opening)
        {
            struct __move __move = __opening->__moves[rand() % __opening->__moves_count];

            if (engine_is_legal_move(__state, __move))
            {
                struct __transposition_entry __entry = tptable_get(__state->__hash);

                __entry.__hash      = __state->__hash;
                __entry.__best_move = __move;
                __entry.__depth     = CHAR_MAX;
                __entry.__value     = 0;
                __entry.__type      = ENTRY_EXACT;

                tptable_put(__entry);

                return;
            }
        }
    }

    struct __game_state *__sft = copy_gamestate(__state);

    struct __move *__legal_moves;

    unsigned int __mcount = engine_order_legal_moves(__state, __state->__is_turn_white ? PLAYER_WHITE : PLAYER_BLACK, &__legal_moves);

    struct __atomic_counter *__refcount = acnt_init(MAX_SEARCH_DEPTH);

    for (int __depth = 1; __depth <= MAX_SEARCH_DEPTH; __depth++)
    {
        struct __movegen_task_arg *__arg = malloc(sizeof(*__arg));

        __arg->__state       = __sft;
        __arg->__depth       = __depth;
        __arg->__start_time  = __start_time;
        __arg->__legal_moves = __legal_moves;
        __arg->__mcount      = __mcount;
        __arg->__refcount    = __refcount;

        threadpool_enqueue(__pool, (__task_func) engine_movegen_task, __arg);
    }   
}

bool engine_is_stalemate(struct __game_state *__state)
{
    enum __player __turn = __state->__is_turn_white ? PLAYER_WHITE : PLAYER_BLACK;

    if (engine_in_check(__state, __turn))
    {
        return false;
    }

    struct __board_pos __legal_moves[PIECE_LEGAL_MOVES_MAX];
    struct __board_pos *__piece_list = __turn == PLAYER_WHITE ? __state->__piece_list_white : __state->__piece_list_black;

    for (int i = 0; i < 16; i++)
    {
        if (!boardpos_eq(__piece_list[i], NULL_BOARDPOS))
        {
            if (engine_legal_moves(__state, __piece_list[i], __legal_moves) != 0)
            {
                return false;
            }
        }
    }

    return true;
}

bool engine_is_checkmate(struct __game_state *__state, enum __player __player)
{
    if (!engine_in_check(__state, __player))
    {
        return false;
    }

    struct __board_pos __legal_moves[PIECE_LEGAL_MOVES_MAX];
    struct __board_pos *__piece_list = __player == PLAYER_WHITE ? __state->__piece_list_white : __state->__piece_list_black;

    for (int i = 0; i < 16; i++)
    {
        if (!boardpos_eq(__piece_list[i], NULL_BOARDPOS))
        {
            if (engine_legal_moves(__state, __piece_list[i], __legal_moves) != 0)
            {
                return false;
            }
        }
    }

    return true;
}

bool engine_is_state_legal(struct __game_state *__state)
{
    enum __player __last_to_move = __state->__is_turn_white ? PLAYER_BLACK : PLAYER_WHITE;

    return !engine_in_check(__state, __last_to_move);
}

void engine_make_move(struct __game_state *__state, struct __move __move, bool __calculate_hash)
{
    struct __piece __src_piece = get_piece(__state, __move.__src);
    struct __piece __dst_piece = get_piece(__state, __move.__dst);

    if (__src_piece.__type == PIECE_PAWN)
    {
        if ((abs(__move.__src.__rank - __move.__dst.__rank) == 2))
        {
            if (__src_piece.__player == PLAYER_WHITE)
            {
                __state->__enpassant_target_black = __move.__src.__file;
            }
            else
            {
                __state->__enpassant_target_white = __move.__src.__file;
            }
        }
        else
        {
            if (__move.__src.__file != __move.__dst.__file && __dst_piece.__type == PIECE_EMPTY)
            {
                put_piece(__state, Piece(PIECE_EMPTY, PLAYER_WHITE), BoardPos(__move.__dst.__file, __move.__src.__rank));

                change_piece_list_pos(__state, other_player(__src_piece.__player), BoardPos(__move.__dst.__file, __move.__src.__rank), NULL_BOARDPOS);
            }

            unset_enpassant_target_file(__state, other_player(__src_piece.__player));
        }
    }
    else if (__src_piece.__type == PIECE_ROOK && (boardpos_eq(__move.__src, ROOK_STARTING_POSITIONS_LEFT[__src_piece.__player])) ||
                                                 (boardpos_eq(__move.__src, ROOK_STARTING_POSITIONS_RIGHT[__src_piece.__player])))
    {
        if (__move.__src.__file == 0)
        {
            unset_castle_left(__state, __src_piece.__player);
        }
        else if (__move.__src.__file == 7)
        {
            unset_castle_right(__state, __src_piece.__player);
        }
    }
    else if (__src_piece.__type == PIECE_KING)
    {
        unset_castle_left(__state, __src_piece.__player);
        unset_castle_right(__state, __src_piece.__player);

        if (boardpos_eq(__move.__src, KING_STARTING_POSITIONS[__src_piece.__player]))
        {
            if (__move.__dst.__file == 2)
            {
                move_piece(__state, BoardPos(0, __move.__src.__rank), BoardPos(3, __move.__src.__rank));

                change_piece_list_pos(__state, __src_piece.__player, BoardPos(0, __move.__src.__rank), BoardPos(3, __move.__src.__rank));
            }
            else if (__move.__dst.__file == 6)
            {
                put_piece(__state, Piece(PIECE_EMPTY, PLAYER_WHITE), BoardPos(7, __move.__src.__rank));
                put_piece(__state, Piece(PIECE_ROOK, __src_piece.__player), BoardPos(5, __move.__src.__rank));

                change_piece_list_pos(__state, __src_piece.__player, BoardPos(7, __move.__src.__rank), BoardPos(5, __move.__src.__rank));
            }
        }

        set_king_pos(__state, __src_piece.__player, __move.__dst);
    }

    change_piece_list_pos(__state, __src_piece.__player, __move.__src, __move.__dst);

    if (__dst_piece.__type != PIECE_EMPTY)
    {
        change_piece_list_pos(__state, __dst_piece.__player, __move.__dst, NULL_BOARDPOS);
    }

    unset_enpassant_target_file(__state, __src_piece.__player);

    struct __piece __new_piece = __src_piece;

    if (__src_piece.__type == PIECE_PAWN && (__move.__dst.__rank == 0 || __move.__dst.__rank == 7))
    {
        __new_piece = Piece(PIECE_QUEEN, __src_piece.__player);
    }

    put_piece(__state, __new_piece, __move.__dst);
    put_piece(__state, Piece(PIECE_EMPTY, PLAYER_WHITE), __move.__src);

    __state->__black_king_in_check = engine_is_piece_attacked(__state, __state->__black_king, PLAYER_WHITE);
    __state->__white_king_in_check = engine_is_piece_attacked(__state, __state->__white_king, PLAYER_BLACK);

    __state->__move_count = (__state->__move_count + 1);

    __state->__is_turn_white = !__state->__is_turn_white;

    if (__calculate_hash)
    {
        __state->__hash = zobrist_hash_state(__state);
    }
    else
    {
        __state->__hash = 0;
    }
}

bool engine_is_move_possible(struct __game_state *__state, struct __move __move)
{
    struct __piece __src_piece = get_piece(__state, __move.__src);
    struct __piece __dst_piece = get_piece(__state, __move.__dst);

    if (__dst_piece.__type != PIECE_EMPTY && __src_piece.__player == __dst_piece.__player)
    {
        return false;
    }

    switch (__src_piece.__type)
    {
        case PIECE_KING:
        {
            if (abs(__move.__src.__file - __move.__dst.__file) <= 1 && abs(__move.__src.__rank - __move.__dst.__rank) <= 1)
            {
                return true;
            }

            bool __is_castle = __move.__src.__rank == __move.__dst.__rank && (__move.__dst.__file == 6 || __move.__dst.__file == 2) &&
                               boardpos_eq(__move.__src, KING_STARTING_POSITIONS[__src_piece.__player]);
            
            if (__is_castle)
            {
                int __direction = __move.__dst.__file == 6 ? 1 : -1;
                int __last_checked_file = __move.__dst.__file == 6 ? 6 : 1;

                for (int i = 4; i != __last_checked_file + __direction; i += __direction)
                {
                    struct __piece __check = get_piece(__state, BoardPos(i, __move.__src.__rank));

                    if (__check.__type != PIECE_EMPTY && i != 4)
                    {
                        return false;
                    }

                    if (i != 1 && engine_is_piece_attacked(__state, BoardPos(i, __move.__src.__rank), other_player(__src_piece.__player)))
                    {
                        return false;
                    }
                }

                return true;
            }
        }

        case PIECE_QUEEN:
        case PIECE_ROOK:
        case PIECE_BISHOP:
        {
            if (__move.__src.__file == __move.__dst.__file)
            {
                if (__src_piece.__type == PIECE_BISHOP)
                {
                    return false;
                }

                for (int i = MIN(__move.__src.__rank, __move.__dst.__rank) + 1; i < MAX(__move.__src.__rank, __move.__dst.__rank); i++)
                {
                    struct __piece __check = get_piece(__state, BoardPos(__move.__src.__file, i));

                    if (__check.__type != PIECE_EMPTY)
                    {
                        return false;
                    }
                }

                return true;
            }
            else if (__move.__src.__rank == __move.__dst.__rank)
            {
                if (__src_piece.__type == PIECE_BISHOP)
                {
                    return false;
                }

                for (int i = MIN(__move.__src.__file, __move.__dst.__file) + 1; i < MAX(__move.__src.__file, __move.__dst.__file); i++)
                {
                    struct __piece __check = get_piece(__state, BoardPos(i, __move.__src.__rank));

                    if (__check.__type != PIECE_EMPTY)
                    {
                        return false;
                    }
                }

                return true;
            }
            else if (abs(__move.__src.__file - __move.__dst.__file) == abs(__move.__src.__rank - __move.__dst.__rank))
            {
                if (__src_piece.__type == PIECE_ROOK)
                {
                    return false;
                }

                int __file_add = __move.__src.__file > __move.__dst.__file ? -1 : 1;
                int __rank_add = __move.__src.__rank > __move.__dst.__rank ? -1 : 1;

                int __file = __move.__src.__file + __file_add;
                int __rank = __move.__src.__rank + __rank_add;
                
                while (__file != __move.__dst.__file)
                {
                    struct __piece __check = get_piece(__state, BoardPos(__file, __rank));

                    if (__check.__type != PIECE_EMPTY)
                    {
                        return false;
                    }

                    __file = (__file + __file_add);
                    __rank = (__rank + __rank_add);
                }

                return true;
            }

            return false;
        }

        case PIECE_KNIGHT:
        {
            return (abs(__move.__src.__file - __move.__dst.__file) == 2 && abs(__move.__src.__rank - __move.__dst.__rank) == 1) ||
                   (abs(__move.__src.__file - __move.__dst.__file) == 1 && abs(__move.__src.__rank - __move.__dst.__rank) == 2);
        }

        case PIECE_PAWN:
        {
            int __direction = __src_piece.__player == PLAYER_BLACK ? 1 : -1;

            return (__move.__dst.__rank - __move.__src.__rank == __direction && abs(__move.__src.__file - __move.__dst.__file) <= 1) ||
                   (__move.__dst.__rank - __move.__src.__rank == 2 * (__direction) && __move.__src.__file == __move.__dst.__file);
        }

        case PIECE_EMPTY:
        {
            return false;
        }
    }
    
    return false;
}

bool engine_is_legal_move(struct __game_state *__state, struct __move __move)
{
    if (!engine_is_move_possible(__state, __move))
    {
        return false;
    }

    struct __piece __src_piece = get_piece(__state, __move.__src);
    struct __piece __dst_piece = get_piece(__state, __move.__dst);

    if (__dst_piece.__type == PIECE_KING)
    {
        return false;
    }

    if (__src_piece.__player != (__state->__is_turn_white ? PLAYER_WHITE : PLAYER_BLACK))
    {
        return false;
    }

    if (__src_piece.__type == PIECE_PAWN)
    {
        if (__move.__src.__file != __move.__dst.__file)
        {
            if (__dst_piece.__type == PIECE_EMPTY)
            {
                if ((__src_piece.__player == PLAYER_WHITE && __move.__src.__rank != 3) ||
                    (__src_piece.__player == PLAYER_BLACK && __move.__src.__rank != 4) ||
                    (get_enpassant_target_file(__state, __src_piece.__player) != __move.__dst.__file))
                {
                    return false;
                }
            }
        }
        else if (abs(__move.__src.__rank - __move.__dst.__rank) == 2)
        {
            if (__src_piece.__player == PLAYER_BLACK && __move.__src.__rank != 1)
            {
                return false;
            }

            if (__src_piece.__player == PLAYER_WHITE && __move.__src.__rank != 6)
            {
                return false;
            }

            int __max_rank = MAX(__move.__dst.__rank, __move.__src.__rank);

            struct __piece __piece1 = get_piece(__state, BoardPos(__move.__src.__file, __max_rank - 1));
            struct __piece __piece2 = get_piece(__state, BoardPos(__move.__src.__file, __move.__dst.__rank));

            if (__piece1.__type != PIECE_EMPTY || __piece2.__type != PIECE_EMPTY)
            {
                return false;
            }
            else
            {
                struct __piece __piece = get_piece(__state, __move.__dst);

                if (__piece.__type != PIECE_EMPTY)
                {
                    return false;
                }
            }
        }
        else if (__src_piece.__type == PIECE_KING && abs(__move.__src.__file - __move.__dst.__file) == 2)
        {
            if (__src_piece.__player == PLAYER_WHITE)
            {
                if ((__move.__dst.__file == 2 && !__state->__white_castle_left) ||
                    (__move.__dst.__file == 6 && !__state->__white_castle_right))
                {
                    return false;
                }
            }
            else
            {
                if ((__move.__dst.__file == 2 && !__state->__black_castle_left) ||
                    (__move.__dst.__file == 6 && !__state->__black_castle_right))
                {
                    return false;
                }
            }
        }
    }

    struct __game_state *__state_copy = copy_gamestate(__state);

    engine_make_move(__state_copy, __move, false);

    bool __legal = engine_is_state_legal(__state_copy);

    free(__state_copy);

    return __legal;
}

bool engine_is_piece_attacked(struct __game_state *__state, struct __board_pos __attacker_pos, enum __player __attacker)
{
    struct __piece __attacker_piece = get_piece(__state, __attacker_pos);

    for (int i = 0; i < 8; i++)
    {
        const struct __board_pos __translation = PIECE_MOVE_DIRECTIONS[PIECE_QUEEN - 1][i];
        
        const bool __is_diagonal = abs(__translation.__file) == abs(__translation.__rank);

        bool __is_king = true;

        struct __board_pos __check = boardpos_add(__attacker_pos, __translation);

        while (!boardpos_eq(__check, NULL_BOARDPOS))
        {
            struct __piece __check_piece = get_piece(__state, __check);

            if (__check_piece.__type != PIECE_EMPTY)
            {
                bool __correct_piece = __check_piece.__type == PIECE_QUEEN || (__is_king && __check_piece.__type == PIECE_KING) ||
                                       (__is_diagonal && __check_piece.__type == PIECE_BISHOP) ||
                                       (!__is_diagonal && __check_piece.__type == PIECE_ROOK);

                if (__correct_piece && __check_piece.__player == __attacker)
                {
                    return true;
                }
                else
                {
                    break;
                }
            }

            __is_king = false;

            __check = boardpos_add(__check, __translation);
        }
    }

    for (int i = 0; i < 8; i++)
    {
        if (boardpos_eq(PIECE_MOVE_DIRECTIONS[PIECE_PAWN - 1][i], NULL_BOARDPOS))
        {
            break;
        }

        struct __board_pos __direction = PIECE_MOVE_DIRECTIONS[PIECE_PAWN - 1][i];

        if (__direction.__file == 0)
        {
            continue;
        }

        if (__attacker_piece.__player == PLAYER_BLACK)
        {
            __direction.__file = (__direction.__file * -1);
            __direction.__rank = (__direction.__rank * -1);
        }

        struct __board_pos __check = boardpos_add(__attacker_pos, __direction);

        if (!boardpos_eq(__check, NULL_BOARDPOS))
        {
            struct __piece __check_piece = get_piece(__state, __check);

            if (__check_piece.__type == PIECE_PAWN && __check_piece.__player == __attacker)
            {
                return true;
            }
        }
    }

    for (int i = 0; i < 8; i++)
    {
        if (boardpos_eq(PIECE_MOVE_DIRECTIONS[PIECE_KNIGHT - 1][i], NULL_BOARDPOS))
        {
            break;
        }

        struct __board_pos __direction = PIECE_MOVE_DIRECTIONS[PIECE_KNIGHT - 1][i];
        struct __board_pos __check     = boardpos_add(__attacker_pos, __direction);

        if (!boardpos_eq(__check, NULL_BOARDPOS))
        {
            struct __piece __check_piece = get_piece(__state, __check);

            if (__check_piece.__type == PIECE_KNIGHT && __check_piece.__player == __attacker)
            {
                return true;
            }
        }
    }

    return false;
}

bool engine_in_check(struct __game_state *__state, enum __player __player)
{
    bool __white = engine_is_piece_attacked(__state, __state->__white_king, PLAYER_BLACK);
    bool __black = engine_is_piece_attacked(__state, __state->__black_king, PLAYER_WHITE);

    return (__player == PLAYER_WHITE) ? __white : __black;
}
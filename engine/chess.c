#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <chess.h>
#include <zobrist.h>

const struct __board_pos NULL_BOARDPOS = {0xF, 0xF};

struct __board_pos BoardPos(int8_t __file, int8_t __rank)
{
    return (struct __board_pos) {__file, __rank};
}

bool boardpos_eq(struct __board_pos __a, struct __board_pos __b)
{
    assert(sizeof(struct __board_pos) == 2);

    return memcmp(&__a, &__b, 2) == 0;
}

struct __board_pos boardpos_add(struct __board_pos __a, struct __board_pos __b)
{
    struct __board_pos __r = {__a.__file + __b.__file, __a.__rank + __b.__rank};
/*
    if (__a.__file > 7 || __a.__rank > 7 || __a.__file < 0 || __a.__rank < 0)
    {
        printf("ERROR! in A\n");
    }

    if (__b.__file > 7 || __b.__rank > 7 || __b.__file < 0 || __b.__rank < 0)
    {
        printf("ERROR! in B\n");
    }

    if (__r.__file > 7 || __r.__rank > 7 || __r.__file < 0 || __r.__rank < 0)
    {
        printf("ERROR! in R\n");
    }*/

    return (__r.__file > 7 || __r.__rank > 7 || __r.__file < 0 || __r.__rank < 0) ? NULL_BOARDPOS : __r;
}

struct __piece Piece(enum __piece_type __type, enum __player __player)
{
    struct __piece __p = {__type, __player};

    return __p;
}

void put_piece(struct __game_state *__state, struct __piece __piece, struct __board_pos __pos)
{
    assert(__pos.__file >= 0 && __pos.__file < 8 && __pos.__rank >= 0 && __pos.__rank < 8);

    __state->__board[__pos.__file][__pos.__rank] = __piece;
}

struct __piece get_piece(struct __game_state *__state, struct __board_pos __pos)
{
    assert(__pos.__file >= 0 && __pos.__file < 8 && __pos.__rank >= 0 && __pos.__rank < 8);

    return __state->__board[__pos.__file][__pos.__rank];
}

int get_enpassant_target_file(struct __game_state *__state, enum __player __player)
{
    if (__player == PLAYER_WHITE)
    {
        return __state->__enpassant_target_white;
    }
    else
    {
        return __state->__enpassant_target_black;
    }
}

void unset_enpassant_target_file(struct __game_state *__state, enum __player __attacking_player)
{
    if (__attacking_player == PLAYER_WHITE)
    {
        __state->__enpassant_target_white = -1;
    }
    else
    {
        __state->__enpassant_target_black = -1;
    }
}

void unset_castle_left(struct __game_state *__state, enum __player __player)
{
    if (__player == PLAYER_WHITE)
    {
        __state->__white_castle_left = false;
    }
    else
    {
        __state->__black_castle_left = false;
    }
}

void unset_castle_right(struct __game_state *__state, enum __player __player)
{
    if (__player == PLAYER_WHITE)
    {
        __state->__white_castle_right = false;
    }
    else
    {
        __state->__black_castle_right = false;
    }
}

void clear_board(struct __game_state *__state)
{
    memset(__state->__board, 0, sizeof(__state->__board));
    memset(__state->__piece_list_white, NULL_BOARDPOS.__file, sizeof(__state->__piece_list_white));
    memset(__state->__piece_list_black, NULL_BOARDPOS.__file, sizeof(__state->__piece_list_black));

    __state->__white_king = NULL_BOARDPOS;
    __state->__black_king = NULL_BOARDPOS;
}

struct __game_state *init_gamestate(void)
{
    struct __game_state *__state = malloc(sizeof(*__state));

    clear_board(__state);

    __state->__is_turn_white = true;
    __state->__enpassant_target_white = -1;
    __state->__enpassant_target_black = -1;

    __state->__white_castle_left = true;
    __state->__white_castle_right = true;
    __state->__black_castle_left = true;
    __state->__black_castle_right = true;

    __state->__white_king = BoardPos(4, 7);
    __state->__black_king = BoardPos(4, 0);

    __state->__white_king_in_check = false;
    __state->__black_king_in_check = false;

    __state->__move_count = 0;

    static const enum __piece_type PIECES_ORDER[8] = {
        PIECE_ROOK,
        PIECE_KNIGHT,
        PIECE_BISHOP,
        PIECE_QUEEN,
        PIECE_KING,
        PIECE_BISHOP,
        PIECE_KNIGHT,
        PIECE_ROOK
    };

    for (int i = 0; i < 8; i++)
    {
        put_piece(__state, Piece(PIECE_PAWN, PLAYER_BLACK), BoardPos(i, 1));
        put_piece(__state, Piece(PIECE_PAWN, PLAYER_WHITE), BoardPos(i, 6));

        put_piece(__state, Piece(PIECES_ORDER[i], PLAYER_BLACK), BoardPos(i, 0));
        put_piece(__state, Piece(PIECES_ORDER[i], PLAYER_WHITE), BoardPos(i, 7));

        __state->__piece_list_white[i + 8] = BoardPos(i, 6);
        __state->__piece_list_black[i + 8] = BoardPos(i, 1);

        __state->__piece_list_white[i] = BoardPos(i, 7);
        __state->__piece_list_black[i] = BoardPos(i, 0);
    }

    __state->__hash = zobrist_hash_state(__state);

    return __state;
}

void deinit_gamestate(struct __game_state *__state)
{
    free(__state);
}

struct __game_state *copy_gamestate(struct __game_state *__state)
{
    struct __game_state *__new = malloc(sizeof(*__state));

    memcpy(__new, __state, sizeof(*__state));

    return __new;
}

void set_king_pos(struct __game_state *__state, enum __player __player, struct __board_pos __pos)
{
    __player == PLAYER_WHITE ? (__state->__white_king = __pos) : (__state->__black_king = __pos);
}

struct __board_pos get_king_pos(struct __game_state *__state, enum __player __player)
{
    return __player == PLAYER_WHITE ? __state->__white_king : __state->__black_king;
}

bool is_player_in_check(struct __game_state *__state, enum __player __player)
{
    return __player == PLAYER_WHITE ? __state->__white_king_in_check : __state->__black_king_in_check;
}

void set_player_in_check(struct __game_state *__state, enum __player __player, bool __check)
{
    __player == PLAYER_WHITE ? (__state->__white_king_in_check = __check) : (__state->__black_king_in_check = __check);
}

enum __player other_player(enum __player __player)
{
    return __player == PLAYER_WHITE ? PLAYER_BLACK : PLAYER_WHITE;
}

void change_piece_list_pos(struct __game_state *__state, enum __player __player, struct __board_pos __from, struct __board_pos __to)
{
    struct __board_pos *__piece_list = __player == PLAYER_WHITE ? __state->__piece_list_white : __state->__piece_list_black;

    for (int i = 0; i < 16; i++)
    {
        if (boardpos_eq(__from, __piece_list[i]))
        {
            __piece_list[i] = __to;
        }
    }
}

static const char *PIECE_NAMES[7] = {"EMPTY", "KING", "QUEEN", "ROOK", "BISHOP", "KNIGHT", "PAWN"};

void print_gamestate(struct __game_state *__state)
{
    printf("GAME STATE: {\n");

    for (int __file = 0; __file < 8; __file++)
    {
        for (int __rank = 0; __rank < 8; __rank++)
        {
            struct __piece __p = get_piece(__state, BoardPos(__file, __rank));

            printf("\t%d, %d : %s %s\n", __file, __rank, (__p.__player == PLAYER_WHITE ? "WHITE" : "BLACK"), PIECE_NAMES[__p.__type]);
        }
    }

    printf("}\n");
}

void boardpos_to_algn(struct __board_pos __pos, char *__buffer)
{
    __buffer[0] = __pos.__file + 'a';
    __buffer[1] = 8 - __pos.__rank + '0';
}

char piece_to_algn(struct __piece __piece)
{
    static const char PIECES[7] = {'\0', 'K', 'Q', 'R', 'B', 'N', '\0'};

    return PIECES[__piece.__type];
}

int move_to_str(struct __game_state *__state, struct __board_pos __from, struct __board_pos __to, char *__buffer)
{
    struct __piece __from_piece = get_piece(__state, __from);

    if (__from_piece.__type == PIECE_KING && abs(__from.__file - __to.__file) == 2)
    {
        if (__to.__file == 2)
        {
            memcpy(__buffer, "0-0-0", 5);

            return 5;
        }
        else
        {
            memcpy(__buffer, "0-0", 3);

            return 3;
        }
    }
    else
    {
        __buffer[0] = piece_to_algn(__from_piece);

        char *__to_buffer = __buffer[0] == '\0' ? __buffer : __buffer + 1;

        struct __piece __to_piece = get_piece(__state, __to);

        if (__to_piece.__type != PIECE_EMPTY)
        {
            if (__to_buffer == __buffer)
            {
                boardpos_to_algn(__from, __to_buffer);

                __to_buffer++;
            }

            *__to_buffer++ = 'x';
        }

        boardpos_to_algn(__to, __to_buffer);

        return __to_buffer - __buffer + 2;
    }
}

void move_piece(struct __game_state *__state, struct __board_pos __from, struct __board_pos __to)
{
    put_piece(__state, get_piece(__state, __from), __to);
    put_piece(__state, Piece(PIECE_EMPTY, PLAYER_WHITE), __from);
}
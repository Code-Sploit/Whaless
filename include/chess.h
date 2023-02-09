#ifndef CHESS_H
#define CHESS_H

#include <stdbool.h>
#include <stdint.h>

typedef uint64_t zobrist_hash;

enum __piece_type
{
    PIECE_EMPTY,
    PIECE_KING,
    PIECE_QUEEN,
    PIECE_ROOK,
    PIECE_BISHOP,
    PIECE_KNIGHT,
    PIECE_PAWN
};

enum __player
{
    PLAYER_WHITE,
    PLAYER_BLACK
};

struct __piece
{
    enum __piece_type __type;
    enum __player __player;
};

#ifndef __GNUC__
#define __PACK__(__Declaration__) __Declaration__ __attribute__((__packed__))
#endif

struct __board_pos {
    int8_t __file;
    int8_t __rank;
};

struct __move
{
    struct __board_pos __src;
    struct __board_pos __dst;
};

struct __game_state
{
    struct __piece __board[8][8];

    bool __is_turn_white;

    int8_t __enpassant_target_white;
    int8_t __enpassant_target_black;

    bool __white_castle_left;
    bool __white_castle_right;
    bool __black_castle_left;
    bool __black_castle_right;

    struct __board_pos __white_king;
    struct __board_pos __black_king;

    bool __white_king_in_check;
    bool __black_king_in_check;

    int __move_count;

    struct __board_pos __piece_list_white[16];
    struct __board_pos __piece_list_black[16];

    zobrist_hash __hash;
};

extern const struct __board_pos NULL_BOARDPOS;

struct __board_pos BoardPos(int8_t __file, int8_t __rank);

bool boardpos_eq(struct __board_pos __a, struct __board_pos __b);

struct __board_pos boardpos_add(struct __board_pos __a, struct __board_pos __b);
struct __piece Piece(enum __piece_type, enum __player);

void put_piece(struct __game_state *__state, struct __piece __piece, struct __board_pos __pos);

struct __piece get_piece(struct __game_state *__state, struct __board_pos __pos);
struct __game_state *init_gamestate(void);

void deinit_gamestate(struct __game_state *__state);

int get_enpassant_target_file(struct __game_state *__state, enum __player __player);

void unset_enpassant_target_file(struct __game_state *__state, enum __player __attacking_player);
void unset_castle_left(struct __game_state *__state, enum __player __player);
void unset_castle_right(struct __game_state *__state, enum __player __player);
void boardpos_to_algn(struct __board_pos __pos, char *__buf);

int move_to_str(struct __game_state *__state, struct __board_pos __from, struct __board_pos __to, char *__buf);

struct __game_state *copy_gamestate(struct __game_state *__state);

void set_king_pos(struct __game_state *__state, enum __player __player, struct __board_pos __pos);

struct __board_pos get_king_pos(struct __game_state *__state, enum __player __player);

bool is_player_in_check(struct __game_state *__state, enum __player __player);

enum __player other_player(enum __player __player);

void change_piece_list_pos(struct __game_state *__state, enum __player __player, struct __board_pos __from, struct __board_pos __to);
void print_gamestate(struct __game_state *__state);
void clear_board(struct __game_state *__state);
void set_player_in_check(struct __game_state *__state, enum __player __player, bool __in_check);
void move_piece(struct __game_state *__state, struct __board_pos __from, struct __board_pos __to);

#endif
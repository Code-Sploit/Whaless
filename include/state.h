#ifndef STATE_H
#define STATE_H

#include <chess.h>
#include <config.h>
#include <stdbool.h>
#include <stdlib.h>
#include <threadpool.h>

struct __fstate
{
    struct __thread_pool *__threadpool;
    struct __game_state *__state;
    struct __board_pos __position;

    bool __2player_mode;

    char *__move_log;

    int __move_log_size;
    int __move_log_idx;
    int __move_log_line_chars;
    int __winner;

    const char *__message;

    time_t __movegen_started;
};

extern struct __fstate __fstate_glob;

#define WINNER_NONE  -1
#define WINNER_WHITE 0
#define WINNER_BLACK 1
#define WINNER_DRAW  2

void state_new_game(void);

void state_log_move(struct __board_pos __src, struct __board_pos __dst);

#endif
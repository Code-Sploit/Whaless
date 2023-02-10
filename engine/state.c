#include <state.h>
#include <chess.h>
#include <string.h>
#include <tptable.h>

struct __fstate __fstate_glob = {
    .__state = NULL,
    .__position = {0xF, 0xF},
    .__2player_mode = false,
    .__move_log = NULL,
    .__move_log_size = 0,
    .__move_log_idx = 0,
    .__move_log_line_chars = 0,
    .__winner = WINNER_NONE,
    .__message = NULL,
    .__threadpool = NULL,
    .__movegen_started = 0
};

void state_reset_ingame(void)
{
    if (__fstate_glob.__state != NULL)
    {
        deinit_gamestate(__fstate_glob.__state);
    }

    if (__fstate_glob.__move_log != NULL)
    {
        free(__fstate_glob.__move_log);
    }

    __fstate_glob.__position = NULL_BOARDPOS;
    __fstate_glob.__move_log = NULL;
    __fstate_glob.__move_log_size = 0;
    __fstate_glob.__move_log_idx = 0;
    __fstate_glob.__move_log_line_chars = 0;
    __fstate_glob.__winner = WINNER_NONE;
    __fstate_glob.__movegen_started = 0;

    tptable_clear();
}

void state_new_game(void)
{
    if (__fstate_glob.__threadpool == NULL)
    {
        __fstate_glob.__threadpool = threadpool_init();
    }

    state_reset_ingame();

    __fstate_glob.__state = init_gamestate();
}

void state_log_move(struct __board_pos __src, struct __board_pos __dst)
{
    /* Do nothing for now */
}
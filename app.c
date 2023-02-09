#include <app.h>

#include <chess.h>
#include <engine.h>
#include <draw.h>
#include <config.h>

static struct __game_state* game = NULL;

void app_start(__window_t* window)
{
    game = init_gamestate();
}

void draw_table_chess(__window_t* window)
{
    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            __color_t bg_color = game->__board[y][x].__player == PLAYER_WHITE
                                  ? (__color_t){ LIGHT_SQUARE_COLOR }
                                  : (__color_t){ DARK_SQUARE_COLOR  };
            printf("%d ", game->__board[y][x].__player);
            draw_rect(window, vec2(y * BOARD_SQUARE_SIZE, x * BOARD_SQUARE_SIZE),
                      vec2(BOARD_SQUARE_SIZE, BOARD_SQUARE_SIZE), bg_color);
        }
        printf("\n");
    }

}

void app_draw(__window_t* window)
{
    draw_table_chess(window);
}
#include <app.h>

#include <chess.h>
#include <engine.h>
#include <draw.h>
#include <config.h>

static struct __game_state* game       = NULL;
static unsigned char table_color[8][8] = BOARD_COLORS;

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
            __color_t bg_color = table_color[y][x] == 0
                                  ? (__color_t){ LIGHT_SQUARE_COLOR }
                                  : (__color_t){ DARK_SQUARE_COLOR  };
            draw_rect(window, vec2(y * BOARD_SQUARE_SIZE, x * BOARD_SQUARE_SIZE),
                      vec2(BOARD_SQUARE_SIZE, BOARD_SQUARE_SIZE), bg_color);
        }
    }

}

void app_draw(__window_t* window)
{
    draw_table_chess(window);
}
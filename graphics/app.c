#include <app.h>

#include <chess.h>
#include <engine.h>
#include <draw.h>
#include <config.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

static unsigned char table_color[8][8] = BOARD_COLORS;

static SDL_Texture* dark_bishop = NULL;
static SDL_Texture* dark_king   = NULL;
static SDL_Texture* dark_knight = NULL;
static SDL_Texture* dark_pawn   = NULL;
static SDL_Texture* dark_queen  = NULL;
static SDL_Texture* dark_rook   = NULL;

static SDL_Texture* white_bishop = NULL;
static SDL_Texture* white_king   = NULL;
static SDL_Texture* white_knight = NULL;
static SDL_Texture* white_pawn   = NULL;
static SDL_Texture* white_queen  = NULL;
static SDL_Texture* white_rook   = NULL;

void load_images_pieces(__window_t* window)
{
    dark_bishop  = load_image(window, "./assets/pieces/dark_bishop.png");
    dark_king    = load_image(window, "./assets/pieces/dark_king.png");
    dark_knight  = load_image(window, "./assets/pieces/dark_knight.png");
    dark_pawn    = load_image(window, "./assets/pieces/dark_pawn.png");
    dark_queen   = load_image(window, "./assets/pieces/dark_queen.png");
    dark_rook    = load_image(window, "./assets/pieces/dark_rook.png");

    white_bishop = load_image(window, "./assets/pieces/white_bishop.png");
    white_king   = load_image(window, "./assets/pieces/white_king.png");
    white_knight = load_image(window, "./assets/pieces/white_knight.png");
    white_pawn   = load_image(window, "./assets/pieces/white_pawn.png");
    white_queen  = load_image(window, "./assets/pieces/white_queen.png");
    white_rook   = load_image(window, "./assets/pieces/white_rook.png");
}

void app_start(__window_t* window)
{
    load_images_pieces(window);
    
    window->__game = init_gamestate();
}

void draw_piece_background(__window_t* window, int x, int y)
{
    // Which color should we select and after it draw a rect easy
    __color_t bg_color = table_color[y][x] == 0 ? (__color_t){ LIGHT_SQUARE_COLOR } : (__color_t){ DARK_SQUARE_COLOR  };
    draw_rect(window, vec2(x * BOARD_SQUARE_SIZE, y * BOARD_SQUARE_SIZE),
                vec2(BOARD_SQUARE_SIZE, BOARD_SQUARE_SIZE), bg_color);
}

SDL_Texture* get_piece_image(enum __piece_type piece_type, enum __player piece_color)
{
    if (piece_color == PLAYER_BLACK)
    {
        switch (piece_type)
        {
            case PIECE_KING:    return dark_king;
            case PIECE_QUEEN:   return dark_queen;
            case PIECE_ROOK:    return dark_rook;
            case PIECE_BISHOP:  return dark_bishop;
            case PIECE_KNIGHT:  return dark_knight;
            case PIECE_PAWN:    return dark_pawn;
            default:            return NULL;
        }
    }
    else
    {
        switch (piece_type)
        {
            case PIECE_KING:    return white_king;
            case PIECE_QUEEN:   return white_queen;
            case PIECE_ROOK:    return white_rook;
            case PIECE_BISHOP:  return white_bishop;
            case PIECE_KNIGHT:  return white_knight;
            case PIECE_PAWN:    return white_pawn;
            default:            return NULL;
        }
    }
}

void draw_piece_image(__window_t* window, int x, int y)
{
    // Which image we need if there isn't any piece in
    // this position just return
    enum __piece_type piece_type  = window->__game->__board[x][y].__type;
    enum __player     piece_color = window->__game->__board[x][y].__player;

    if (piece_type == PIECE_EMPTY) return;

    SDL_Texture* piece_image = get_piece_image(piece_type, piece_color);

    if (piece_image == NULL) return;

    draw_image(window, vec2(x * BOARD_SQUARE_SIZE + (BOARD_SQUARE_SIZE / 2 - 60 / 2), y * BOARD_SQUARE_SIZE + (BOARD_SQUARE_SIZE / 2 - 60 / 2)), piece_image);
}

void draw_table_chess(__window_t* window)
{
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            draw_piece_background(window, x, y);
            draw_piece_image(window, x, y);
        }
    }
}

void app_draw(__window_t* window)
{
    draw_rect(window, vec2(640, 0), vec2(300, WINDOW_HEIGHT), (__color_t){ WINDOW_BACKGROUND });
    draw_table_chess(window);
}
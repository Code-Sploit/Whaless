#include <app.h>

#include <chess.h>
#include <engine.h>
#include <state.h>
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
    dark_bishop  = load_image(window, "./assets/pieces/1/dark_bishop.png");
    dark_king    = load_image(window, "./assets/pieces/1/dark_king.png");
    dark_knight  = load_image(window, "./assets/pieces/1/dark_knight.png");
    dark_pawn    = load_image(window, "./assets/pieces/1/dark_pawn.png");
    dark_queen   = load_image(window, "./assets/pieces/1/dark_queen.png");
    dark_rook    = load_image(window, "./assets/pieces/1/dark_rook.png");

    white_bishop = load_image(window, "./assets/pieces/1/white_bishop.png");
    white_king   = load_image(window, "./assets/pieces/1/white_king.png");
    white_knight = load_image(window, "./assets/pieces/1/white_knight.png");
    white_pawn   = load_image(window, "./assets/pieces/1/white_pawn.png");
    white_queen  = load_image(window, "./assets/pieces/1/white_queen.png");
    white_rook   = load_image(window, "./assets/pieces/1/white_rook.png");
}

void app_start(__window_t* window)
{
    load_images_pieces(window);
    
    __fstate_glob.__state = init_gamestate();
}

void draw_piece_background(__window_t* window, int x, int y)
{
    enum __piece_type piece_type  = __fstate_glob.__state->__board[x][y].__type;
    enum __player     piece_color = __fstate_glob.__state->__board[x][y].__player;

    __color_t bg_color;
    if (table_color[y][x] == 0)
    {
        bg_color = (window->src_piece.__file == x && window->src_piece.__rank == y) == true 
            ? (__color_t){ SELECTED_LIGHT_SQUARE_COLOR }
            : (__color_t){ LIGHT_SQUARE_COLOR          };
    }
    else
    {
        bg_color = (window->src_piece.__file == x && window->src_piece.__rank == y) == true 
            ? (__color_t){ SELECTED_DARK_SQUARE_COLOR }
            : (__color_t){ DARK_SQUARE_COLOR          };
    }

    draw_rect(window, vec2(x * BOARD_SQUARE_SIZE, y * BOARD_SQUARE_SIZE),
                vec2(BOARD_SQUARE_SIZE, BOARD_SQUARE_SIZE), bg_color);

    if (piece_type == PIECE_KING)
    {
        if (piece_color == PLAYER_WHITE && __fstate_glob.__state->__white_king_in_check == true) 
            draw_rect(window, vec2(x * BOARD_SQUARE_SIZE, y * BOARD_SQUARE_SIZE),
                vec2(BOARD_SQUARE_SIZE, BOARD_SQUARE_SIZE), (__color_t){ 255, 0, 0, 70 });
        if (piece_color == PLAYER_BLACK && __fstate_glob.__state->__black_king_in_check == true)
            draw_rect(window, vec2(x * BOARD_SQUARE_SIZE, y * BOARD_SQUARE_SIZE),
                vec2(BOARD_SQUARE_SIZE, BOARD_SQUARE_SIZE), (__color_t){ 255, 0, 0, 70 });
    }
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
    enum __piece_type piece_type  = __fstate_glob.__state->__board[x][y].__type;
    enum __player     piece_color = __fstate_glob.__state->__board[x][y].__player;

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

void draw_table_info(__window_t* window)
{
    draw_rect(window, vec2(640, 0), vec2(300, WINDOW_HEIGHT), (__color_t){ WINDOW_BACKGROUND });
    set_font_size(24);
    draw_text(window, 650, 10, "Whaless");
    set_font_size(20);
    if (__fstate_glob.__winner == WINNER_NONE)
    {
        draw_text(window, 650, 40, "Player move: %s", __fstate_glob.__state->__is_turn_white == true ? "White" : "Black");
        draw_text(window, 650, 67, "Move count: %d", __fstate_glob.__state->__move_count);
    }
    else
    {
        draw_text(window, 650, 40, "Game ended");
    }
}

void app_draw(__window_t* window)
{
    draw_table_info(window);
    draw_table_chess(window);
}
#include <window.h>
#include <engine.h>
#include <config.h>
#include <state.h>
#include <tptable.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

__window_t* create_new_window(int argc, char** argv, int window_width, int window_height)
{
    int __status = SDL_Init(SDL_INIT_EVERYTHING);
    if (__status != 0)
    {
        printf("Couldn't initialize SDL\n");
        return NULL;
    }

    __status = IMG_Init(IMG_INIT_PNG);
    if (__status == 0)
    {
        printf("Couldn't initialize SDL image\n");
        return NULL;
    }

    __window_t* window = calloc(1, sizeof(__window_t));
    window->base_window = SDL_CreateWindow("Whaless Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, 0);
    window->base_renderer = SDL_CreateRenderer(window->base_window, -1, SDL_RENDERER_ACCELERATED);

    window->should_close = false;
    window->is_holding_piece = false;

    return window;
}

struct __board_pos translate_pos_gui_to_real(int __x, int __y)
{
    struct __board_pos __position;

    __position.__rank = (__y / BOARD_SQUARE_SIZE);
    __position.__file = (__x / BOARD_SQUARE_SIZE);

    return __position;
}

void window_handle_events(__window_t* window)
{
    if (window == NULL) return;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type)
        {
            case SDL_QUIT:
                window->should_close = true;
                break;
            
            case SDL_MOUSEMOTION:
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    int __delta_x = event.motion.x;
                    int __delta_y = event.motion.y;

                    /*
                    * Do nothing for now
                    * TODO: Implement sliding pieces                    
                    */
                }

                break;
            }
            
            case SDL_MOUSEBUTTONDOWN:
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    /*
                    * Check if the engine is still generating move
                    */

                    if (time(NULL) - __fstate_glob.__movegen_started >= MAX_MOVEGEN_SEARCH_TIME)
                    {
                        __fstate_glob.__movegen_started = 0;

                        struct __transposition_entry __entry = tptable_get(__fstate_glob.__state->__hash);

                        if (!boardpos_eq(__entry.__best_move.__src, NULL_BOARDPOS))
                        {
                            engine_make_move(__fstate_glob.__state, __entry.__best_move, true);

                            if (!boardpos_eq(__fstate_glob.__position, NULL_BOARDPOS) &&
                                (get_piece(__fstate_glob.__state, __fstate_glob.__position).__type == PIECE_EMPTY ||
                                boardpos_eq(__fstate_glob.__position, __entry.__best_move.__dst)))
                            {
                                __fstate_glob.__position = NULL_BOARDPOS;
                            }

                            if (engine_is_checkmate(__fstate_glob.__state, PLAYER_WHITE))
                            {
                                __fstate_glob.__winner = WINNER_BLACK;
                            }
                            else if (engine_is_checkmate(__fstate_glob.__state, PLAYER_BLACK))
                            {
                                __fstate_glob.__winner = WINNER_WHITE;
                            }
                            else if (engine_is_stalemate(__fstate_glob.__state))
                            {
                                __fstate_glob.__winner = WINNER_DRAW;
                            }
                        }
                    }

                    /*
                    * The player clicked a field, get X and Y of mouse
                    */

                    int __x = event.button.x;
                    int __y = event.button.y;

                    /*
                    * Is the player holding a piece?
                    * If so -> move the piece to the clicked square
                    * If not -> pickup the piece on the square
                    */

                    struct __board_pos __position_dst;

                    if (window->is_holding_piece)
                    {
                        struct __board_pos __position_src = window->src_piece;

                        __position_dst = translate_pos_gui_to_real(__x, __y);

                        bool __is_valid = engine_is_legal_move(__fstate_glob.__state, (struct __move) {__position_src, __position_dst});

                        printf("Legal Move Evaluation: [%s]\n", __is_valid == true ? "LEGAL" : "ILLEGAL");

                        window->is_holding_piece = false;

                        if (__is_valid)
                        {
                            engine_make_move(__fstate_glob.__state, (struct __move) {__position_src, __position_dst}, true);

                            printf("Game Evaluation: [%d]\n", engine_evaluate_position(__fstate_glob.__state));
                            printf("Generating engine move!\n");

                            __fstate_glob.__movegen_started = time(NULL);

                            engine_generate_move(__fstate_glob.__state, __fstate_glob.__threadpool, __fstate_glob.__movegen_started);
                        }
                    }
                    else
                    {
                        struct __board_pos __position_src = translate_pos_gui_to_real(__x, __y);
                        struct __piece     __piece_src    = get_piece(__fstate_glob.__state, __position_src);

                        enum __player __to_move = (__fstate_glob.__state->__is_turn_white == true) ? PLAYER_WHITE : PLAYER_BLACK;

                        if (__piece_src.__player == __to_move && __piece_src.__type != PIECE_EMPTY)
                        {
                            window->src_piece        = __position_src;
                            window->is_holding_piece = true;
                        }
                    }
                }

                break;
            }
        }
    }
}

__vec2_t get_window_size(__window_t* window)
{
    if (window == NULL) return vec2(0, 0);

    __vec2_t win_size_vec = vec2(0, 0);
    SDL_GetWindowSize(window->base_window, (int*)&win_size_vec.x, (int*)&win_size_vec.y);

    return win_size_vec;
}

void window_set_bg(__window_t* window, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    if (window == NULL) return;

    window->background.r = r;
    window->background.g = g;
    window->background.b = b;
    window->background.a = a;
}

void window_clear_bufffer(__window_t* window)
{
    if (window == NULL) return;

    SDL_RenderClear(window->base_renderer);
}

void window_swap_bufffer(__window_t* window)
{
    if (window == NULL) return;

    SDL_RenderPresent(window->base_renderer);
}
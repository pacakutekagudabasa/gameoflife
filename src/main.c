/**
 * @file main.c
 * @brief Enhanced Game of Life का main implementation
 * @author Game of Life Enhanced
 * @date 2025
 * 
 * यह फाइल Conway's Game of Life का enhanced version implement करती है।
 * इसमें configurable rules, interactive controls, file I/O, और drag-to-paint
 * functionality शामिल है। SDL2 का उपयोग करके graphics rendering की गई है।
 */

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "board.h"
#include "state.h"
#include "rules.h"

/**
 * @brief प्रत्येक cell का size pixels में
 */
#define PIXEL_SIZE 10

/**
 * @brief दो variables को swap करने के लिए macro
 * @param type variables का data type
 * @param x पहला variable
 * @param y दूसरा variable
 */
#define SWAP(type, x, y) \
    type temp = x; \
    x = y; \
    y = temp;

/**
 * @brief Available rule sets की कुल संख्या
 */
#define NUM_RULE_SETS 4

/**
 * @brief Predefined rule creation functions का global array
 * 
 * यह array सभी available rule sets के creation functions को store करता है।
 * Runtime पर rule switching के लिए उपयोग किया जाता है।
 */
Rules* (*rule_creators[NUM_RULE_SETS])(void) = {
    rules_conway,     /**< Conway's Game of Life */
    rules_highlife,   /**< HighLife variant */
    rules_day_night,  /**< Day & Night rules */
    rules_maze        /**< Maze generation rules */
};

/**
 * @brief SDL renderer का color cell की value के अनुसार set करता है
 * 
 * यह function cell की state (जीवित/मृत) के अनुसार renderer का color
 * black (मृत) या white (जीवित) set करता है।
 * 
 * @param renderer SDL renderer pointer
 * @param cell cell की value (0=मृत, 1=जीवित)
 * @return सफल होने पर 0, error होने पर -1
 */
int set_pixel_color(SDL_Renderer *renderer, int cell) {
    if (renderer == NULL) return -1;
    if (cell != 0 && cell != 1) {
        return -1;
    }
    return (SDL_SetRenderDrawColor(renderer, 255 * cell, 255 * cell, 255 * cell, 255) == 0) ? 0 : -1;
}

/**
 * @brief एक specific cell को screen पर draw करता है
 * 
 * यह function board के specific coordinates पर स्थित cell को
 * SDL renderer का उपयोग करके screen पर rectangle के रूप में draw करता है।
 * 
 * @param renderer SDL renderer pointer
 * @param board source board
 * @param x cell का x coordinate (row)
 * @param y cell का y coordinate (column)
 * @return सफल होने पर 0, error होने पर -1
 * 
 * @note SDL coordinates board coordinates से inverted हैं
 */
int board_pixel_draw(SDL_Renderer *renderer, Board *board, int x, int y) {
    if (renderer == NULL || board == NULL) return -1;
    if (x < 0 || y < 0 || x >= (int)board->height || y >= (int)board->width) return -1;
    
    // 2D coordinates को 1D index में convert करें
    size_t index = x * board->width + y;
    
    if (set_pixel_color(renderer, board->cells[index]) != 0) return -1;

    // Border के लिए 1 pixel offset add करें
    // NOTE: SDL coordinates board coordinates से inverted हैं
    struct SDL_Rect pixel = { 
        (y + 1) * PIXEL_SIZE, 
        (x + 1) * PIXEL_SIZE, 
        PIXEL_SIZE, 
        PIXEL_SIZE 
    };
    
    return (SDL_RenderFillRect(renderer, &pixel) == 0) ? 0 : -1;
}

/**
 * @brief पूरे board को screen पर draw करता है
 * 
 * यह function board के सभी cells को iterate करके प्रत्येक को
 * screen पर draw करता है। यह main rendering function है।
 * 
 * @param renderer SDL renderer pointer
 * @param board draw करने वाला board
 * @return सफल होने पर 0, error होने पर -1
 */
int board_draw(SDL_Renderer *renderer, Board *board) {
    if (renderer == NULL || board == NULL) return -1;
    
    // 1D array में single loop - better performance
    for (size_t i = 0; i < board->width * board->height; i++) {
        // 1D index को 2D coordinates में convert करें drawing के लिए
        size_t x = i / board->width;
        size_t y = i % board->width;
        
        if (board_pixel_draw(renderer, board, x, y) != 0) return -1;
    }
    
    return 0;
}

/**
 * @brief SDL window create करता है
 * 
 * यह function SDL को initialize करके specified dimensions के साथ
 * एक नई window create करता है।
 * 
 * @param title window का title
 * @param width window की चौड़ाई pixels में
 * @param height window की ऊंचाई pixels में
 * @return सफल होने पर SDL_Window pointer, error होने पर NULL
 */
SDL_Window *create_window(char *title, unsigned int width, unsigned int height)
{
    if (title == NULL) return NULL;
    
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		return NULL;
	}

	return SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width,
		height,
		SDL_WINDOW_SHOWN
	);
}

/**
 * @brief SDL renderer create करता है
 * 
 * यह function दी गई window के लिए SDL renderer create करता है
 * जो graphics operations के लिए उपयोग किया जाता है।
 * 
 * @param window target SDL window
 * @return सफल होने पर SDL_Renderer pointer, error होने पर NULL
 */
SDL_Renderer *create_renderer(SDL_Window *window)
{
    if (window == NULL) return NULL;
    
	const int FIRST_RENDERING_DRIVER = -1;

	return SDL_CreateRenderer(
		window,
		FIRST_RENDERING_DRIVER,
		0
	);
}

/**
 * @brief File से board load करता है और errors को handle करता है
 * 
 * यह function specified file से pattern load करके board में set करता है।
 * User को appropriate feedback भी provide करता है।
 * 
 * @param filename load करने वाली file का नाम
 * @param board target board
 * @return सफल होने पर 0, error होने पर -1
 */
int load_board_from_file(const char *filename, Board *board) {
    if (!filename || !board) return -1;
    
    printf("Loading board from file: %s\n", filename);
    if (board_from_file((char*)filename, board) != 0) {
        printf("Error loading file: %s\n", filename);
        return -1;
    }
    printf("Board loaded successfully from: %s\n", filename);
    return 0;
}

/**
 * @brief Mouse coordinates को board coordinates में convert करके cell paint करता है
 * 
 * यह function mouse के current position को board coordinates में convert करता है
 * और state के अनुसार cell को paint करता है (जीवित या मृत)।
 * 
 * @param mouse_x mouse का x coordinate
 * @param mouse_y mouse का y coordinate  
 * @param board target board
 * @param state current game state (paint mode determine करने के लिए)
 * @return सफल होने पर 0, out of bounds या error होने पर -1
 */
int paint_cell_at_mouse(int mouse_x, int mouse_y, Board *board, State *state) {
    if (!board || !state) return -1;
    
    // Mouse coordinates को board coordinates में convert करें
    // 1-pixel border offset को account करें
    int board_y = mouse_x / PIXEL_SIZE - 1;
    int board_x = mouse_y / PIXEL_SIZE - 1;
    
    // Bounds check करें
    if (board_x < 0 || board_y < 0 || board_x >= (int)board->height || board_y >= (int)board->width) {
        return -1;
    }
    
    // 2D coordinates को 1D index में convert करें
    size_t index = board_x * board->width + board_y;
    
    // Paint mode के अनुसार cell set करें
    board->cells[index] = state->drag_paint_mode;
    
    return 0;
}

/**
 * @brief Console में help text print करता है
 * 
 * यह function सभी available controls और current rules की जानकारी
 * user को provide करता है।
 * 
 * @param rules current active rules
 */
void print_help(Rules *rules) {
    printf("\n=== Game of Life Controls ===\n");
    printf("SPACE       - Pause/Unpause simulation\n");
    printf("R           - Reload from file (if loaded from file)\n");
    printf("C           - Clear board\n");
    printf("G           - Generate random board\n");
    printf("T           - Switch rule set\n");
    printf("H           - Show this help\n");
    printf("ESC/Q       - Quit game\n");
    printf("Mouse Click - Toggle cell (when paused)\n");
    printf("Mouse Drag  - Paint alive cells (when paused)\n");
    printf("Ctrl+Drag   - Paint dead cells (when paused)\n");
    printf("\nCurrent Rules: ");
    rules_print(rules);
    printf("=============================\n");
}

/**
 * @brief SDL events को process करता है और appropriate actions perform करता है
 * 
 * यह function सभी SDL events (keyboard, mouse) को handle करता है और
 * game state को accordingly update करता है। यह main input handling function है।
 * 
 * @param state current game state
 * @param board current board
 * @param current_rules current active rules (switching के लिए modifiable)
 * @return सफल होने पर 0, error होने पर -1
 */
int process_events(State *state, Board *board, Rules **current_rules) {
    if (state == NULL || board == NULL || current_rules == NULL) return -1;
    
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_QUIT:
                state->keep_alive = 0;
                break;

            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                    case SDLK_SPACE:
                        // Simulation को pause/unpause करें
                        state->pause = !state->pause;
                        printf("Game %s\n", state->pause ? "PAUSED" : "RESUMED");
                        break;
                        
                    case SDLK_r:
                        // File से reload करें अगर available है
                        if (state->loaded_filename) {
                            if (load_board_from_file(state->loaded_filename, board) == 0) {
                                state->pause = 1; // Reload के बाद pause करें
                            }
                        } else {
                            printf("No file to reload from. Load a file first.\n");
                        }
                        break;
                        
                    case SDLK_c:
                        // Board को clear करें
                        board_clear(board);
                        printf("Board cleared\n");
                        state->pause = 1;
                        break;
                        
                    case SDLK_g:
                        // Random board generate करें
                        board_random_fill(board);
                        printf("Random board generated\n");
                        state->pause = 1;
                        break;
                        
                    case SDLK_t:
                        // Rule set को switch करें
                        rules_free(*current_rules);
                        state->current_rule_index = (state->current_rule_index + 1) % NUM_RULE_SETS;
                        *current_rules = rule_creators[state->current_rule_index]();
                        printf("Switched to rule set: ");
                        rules_print(*current_rules);
                        break;
                        
                    case SDLK_h:
                        // Help show करें
                        print_help(*current_rules);
                        break;
                        
                    case SDLK_ESCAPE:
                    case SDLK_q:
                        // Game quit करें
                        state->keep_alive = 0;
                        break;
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (state->pause && e.button.button == SDL_BUTTON_LEFT) {
                    // Ctrl key check करके paint mode determine करें
                    SDL_Keymod keymod = SDL_GetModState();
                    state->drag_paint_mode = !(keymod & KMOD_CTRL); // Ctrl = मृत paint, no Ctrl = जीवित paint
                    
                    state->is_dragging = true;
                    
                    // Initial cell paint करें
                    paint_cell_at_mouse(e.button.x, e.button.y, board, state);
                }
                break;
                
            case SDL_MOUSEBUTTONUP:
                if (e.button.button == SDL_BUTTON_LEFT) {
                    state->is_dragging = false;
                }
                break;
                
            case SDL_MOUSEMOTION:
                if (state->pause && state->is_dragging) {
                    // Dragging के दौरान painting continue करें
                    paint_cell_at_mouse(e.motion.x, e.motion.y, board, state);
                }
                break;
        }
    }
    
    return 0;
}

/**
 * @brief Main function - program का entry point
 * 
 * यह function game को initialize करता है, main loop run करता है,
 * और cleanup operations perform करता है। Command line arguments को
 * handle करके file loading भी support करता है।
 * 
 * @param argc command line arguments की संख्या
 * @param argv command line arguments का array
 * @return program exit status (0=success, non-zero=error)
 */
int main(int argc, char **argv) {
    int error_code = 0;

    // Board dimensions set करें
    const size_t height = 64;
    const size_t width = height;
    
    // SDL window का size define करें
    // Border के लिए 2 extra pixels add करें
    const int window_width = PIXEL_SIZE * (height + 2);
    const int window_height = PIXEL_SIZE * (width + 2);

    // Rules initialize करें (default Conway's Life)
    Rules *current_rules = rules_conway();
    if (!current_rules) {
        printf("Error initializing rules\n");
        return 1;
    }

    // Boards create करें (double buffering के लिए)
    Board *front = board_init(height, width);
    Board *back = board_init(height, width);
    
    if (front == NULL || back == NULL) {
        printf("Erreur lors de l'allocation des boards\n");
        error_code = 1;
        goto cleanup;
    }

    // Game state create करें
    struct State *state = NULL;
    if (state_init(&state) != 0)
    {
        printf("Erreur lors de l'initialisation du state\n");
        error_code = 1;
        goto cleanup;
    }

    // Command line arguments के अनुसार file load करें या random generate करें
    if (argc > 1) {
        char *filename = argv[1];
        
        if (load_board_from_file(filename, front) != 0) {
            printf("Erreur lors de la lecture du fichier. Générant une grille aléatoire à la place.\n");
            srand(time(NULL));
            board_random_fill(front);
        } else {
            // Reloading के लिए filename store करें
            state_set_filename(state, filename);
        }
    } else {
        printf("Chargement d'une grille aléatoire\n");

        srand(time(NULL));
        if (board_random_fill(front) != 0) {
            printf("Erreur lors du remplissage aléatoire. Arrêt du programme.\n");
            error_code = 1;
            goto cleanup;
        }
    }

    // SDL initialize करें
    SDL_Window *window = create_window("Game Of Life - Enhanced", window_height, window_width);
    if (window == NULL) {
        printf("Erreur lors de la création de la fenêtre SDL: %s\n", SDL_GetError());
        error_code = 1;
        goto cleanup;
    }
    
    SDL_Renderer *renderer = create_renderer(window);
    if (renderer == NULL) {
        printf("Erreur lors de la création du renderer SDL: %s\n", SDL_GetError());
        error_code = 1;
        goto cleanup_window;
    }

    // Initial help print करें
    print_help(current_rules);
    
    // Paused state में start करें
    state->pause = 1;
    printf("Starting paused. Press SPACE to begin simulation.\n");

    // Main game loop
    while (state->keep_alive) {
        if (process_events(state, front, &current_rules) != 0) {
            printf("Erreur lors du traitement des événements\n");
            error_code = 1;
            break;
        }

        // Screen को black color से clear करें
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (board_draw(renderer, front) != 0) {
            printf("Erreur lors du dessin de la board\n");
            error_code = 1;
            break;
        }
        
        SDL_RenderPresent(renderer);

        // अगर pause है तो next generation calculate न करें
        if (state->pause) {
            SDL_Delay(50); // Paused state में CPU usage reduce करें
            continue;
        }
        
        // Current rules के साथ next generation calculate करें
        if (board_next(front, back, current_rules) != 0) {
            printf("Erreur lors du calcul de la prochaine génération\n");
            error_code = 1;
            break;
        }
        
        SWAP(Board *, front, back);
    
        // Next generation display करने से पहले wait करें
        SDL_Delay(50);
    }

    printf("Game ended. Goodbye!\n");

cleanup_renderer:
    state_free(state);
    rules_free(current_rules);
    SDL_DestroyRenderer(renderer);
    
cleanup_window:
    SDL_DestroyWindow(window);
    SDL_Quit();

cleanup:
    if (front != NULL) board_free(front);
    if (back != NULL) board_free(back);

    return error_code;
}

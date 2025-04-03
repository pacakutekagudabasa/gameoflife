#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "board.h"
#include "state.h"

#define PIXEL_SIZE 10
#define SWAP(type, x, y) \
    type temp = x; \
    x = y; \
    y = temp;

// Fixe la couleur du renderer SDL a noir ou blanc en fonction de la valeur de la cell.
//
// Toutes les valeurs sont expectées d'être == 1 ou == 0.
void set_pixel_color(SDL_Renderer *renderer, int cell) {
    SDL_SetRenderDrawColor(renderer, 255 * cell, 255 * cell, 255 * cell, 255);
}

void board_pixel_draw(SDL_Renderer *renderer, struct Board *board, int x, int y) {
    set_pixel_color(renderer, board->cells[x][y]);

    // On demande un pixel de plus pour faire un petit offset vers le coin inférieur droit 
    // pour faire une petite marge.
    //
    // NOTE: Les coordonnées X et Y sont inversés car SDL n'utilise pas la même convention
    // pour les axes que celle que j'ai choisie pour indexer dans la matrice de ma board.
    struct SDL_Rect pixel = { 
        (y + 1) * PIXEL_SIZE, 
        (x + 1) * PIXEL_SIZE, 
        PIXEL_SIZE, 
        PIXEL_SIZE 
    };
    SDL_RenderFillRect(renderer, &pixel);
}

void board_draw(SDL_Renderer *renderer, struct Board *board) {
    for (size_t x = 0; x < board->height; x++) {
        for (size_t y = 0; y < board->width; y++) {
            board_pixel_draw(renderer, board, x, y);
        }
    }
}

SDL_Window *create_window(char *title, unsigned int width, unsigned int height)
{
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


SDL_Renderer *create_renderer(SDL_Window *window)
{
	const int FIRST_RENDERING_DRIVER = -1;

	return SDL_CreateRenderer(
		window,
		FIRST_RENDERING_DRIVER,
		0
	);
}

// Fonction récupérant les event SDL. Utilisé pour savoir s'il faut fermer le programme.
void process_events(struct State *state, struct Board *board) {
    SDL_Event e;
    SDL_PollEvent(&e);
    
    switch (e.type) {
        case SDL_QUIT:
            state->keep_alive = 0;
            break;

        case SDL_KEYUP:
            if (e.key.keysym.sym == SDLK_SPACE) {
                state->pause = !state->pause;
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (state->pause && e.button.button == SDL_BUTTON_LEFT) {
                // Encore une fois, les axes sont inversés entre SDL et la convention
                // que j'utilise pour la grille de cellules.
                //
                // Aussi, comme j'ai ajouté une marge de 1 pixel autour de ma grille,
                // on doit calculer la position du pixel en déduisant l'offset.
                int y = e.button.x / PIXEL_SIZE - 1;
                int x = e.button.y / PIXEL_SIZE - 1;
              
                // On ignore les clics sur la bordure.
                if (x < 0 || y < 0 || x >= (int)board->height || y >= (int)board->width) return;

                board->cells[x][y] = !board->cells[x][y];
            }
            break;
    }
}

int main(int argc, char **argv) {
    int error_code = 0;

    // Set des dimentions.
    const size_t height = 64;
    const size_t width = height;
    
    // Défintition de la taille de la fenêtre SDL.
    //
    // On demande 2 pixels de largeur de plus pour faire des jolis petits bords. :)
    const int window_width = PIXEL_SIZE * (height + 2);
    const int window_height = PIXEL_SIZE * (width + 2);

    // Création des boards.
    struct Board *front = board_init(height, width);
    struct Board *back = board_init(height, width);

    // Chargement d'un fichier en fonction des arguments.
    if (argc > 1) {
        char *filename = argv[1];
        printf("Chargement de la grille contenue dans le fichier '%s'\n", filename);

        if (board_from_file(filename, front))
        {
            printf("Erreur lors de la lecture du fichier. Arrêt du programme.\n");
            error_code = 1;
            goto cleanup;
        }
    } else {
        printf("Chargement d'une grille aléatoire\n");

        srandom(time(NULL));
        board_random_fill(front);
    }

    // Init de SDL.
    SDL_Window *window = create_window("Game Of Life", window_height, window_width);
    SDL_Renderer *renderer = create_renderer(window);

    // Creation du state.
    struct State *state = state_init();

    // Boucle principale.
    while (state->keep_alive) {
        process_events(state, front);

        board_draw(renderer, front);
        SDL_RenderPresent(renderer);

        // On ne calcule pas la prochaine frame si le jeu est en pause.
        if (state->pause) continue;
        
        // Calcul de la prochaine intération dans l'autre buffer. 
        board_next(front, back);
        SWAP(struct Board *, front, back);
    
        // Attente avant d'afficher la prochain génération.
        SDL_Delay(50);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    free(state);

cleanup:
    board_free(front);
    board_free(back);

    return error_code;
}

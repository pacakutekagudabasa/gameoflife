#include <stdio.h>
#include <stdlib.h>

#include "board.h"

#define BOARD_HEIGHT 32 
#define BOARD_WIDTH BOARD_HEIGHT

#define MIN(x, y) ((x) < (y) ? x : y)
#define MAX(x, y) ((x) > (y) ? x : y)

// Initialise la board.
struct Board *board_init(size_t height, size_t width) {
    struct Board *board = malloc(sizeof(struct Board));
    
    // Ajout des valeurs.
    board->height = height;
    board->width = width;

    // Init de toutes les cellules.
    board->cells = calloc(sizeof(int **), height);
    for (size_t i = 0; i < height; i++) {
        board->cells[i] = calloc(sizeof(int *), width);
    }

    return board;
}

// Affiche la grille du jeu de la vie dans le terminal.
void board_print(struct Board *board) {
    for (size_t x = 0; x < board->height; x++) {
        for (size_t y = 0; y < board->width; y++) {
            char *c = board->cells[x][y] ? "██" : "  "; 
            printf("%s", c);
        }
        printf("\n");
    }
}

// Libère toute la mémoire présente pour la board.
void board_free(struct Board *board) {
    // Libération de la mémoire pour le tableau.
    for (size_t x = 0; x < board->height; x++) {
        free(board->cells[x]);
    }
    free(board->cells);

    // Libération de la mémoire de la struct.
    free(board);
}

// Renvoie si une cellule est vivante à la prochaine génération.
int is_cell_alive_next_gen(struct Board *board, size_t x, size_t y) {
    // On compte toutes les cellules en vie autour de la cellule (x, y)
    int count = 0;
    for (size_t i = MAX((int)x-1, 0); i <= MIN(x+1, board->height - 1); i++) {
        for (size_t j = MAX((int)y-1, 0); j <= MIN(y+1, board->width - 1); j++) {
            // Ne compte pas la cell actuelle.
            if (i == x && j == y) continue; 

            // Incremente le conteur en fonction de si la cellule est actuellement vivante ou pas.
            count += board->cells[i][j];

            // Si on depasse les 4, on sait directement que la cellule est morte. 
            if (count > 3) return 0; // true
        }
    }

    // Une cellule meurt si elle a moins de 3 ou plus de 4 voisins.
    int cell_is_dead = board->cells[x][y] == 0;
    if (cell_is_dead) {
        return count == 3;
    } else {
        return count == 2 || count == 3;
    }
}

// Produit la future itération de le board dans la board out.
//
// Permet d'allouer que 2 boards et de swap les pointeurs pour calculer chaque nouvelle itération de manière
// propre en mémoire.
void board_next(struct Board *board, struct Board *out) {
    for (size_t x = 0; x < board->height; x++) {
        for (size_t y = 0; y < board->width; y++) {
            out->cells[x][y] = is_cell_alive_next_gen(board, x, y);
        }
    }
}

// Renvoie un nombre aléatoire entre min et max.
static int randint(int min, int max) {
    return random() % (max + 1 - min) + min;
}

// Initialise la grille de jeu avec des valeurd aléatoires.
void board_random_fill(struct Board *board) {
    for (size_t x = 0; x < board->height; x++) {
        for (size_t y = 0; y < board->width; y++) {
            board->cells[x][y] = randint(0, 4) == 0;
        }
    }
}

// Charge le contenu dans une board.
// 
// Précondition: Le fichier doit avoir les mêmes dimensions de la board. Sinon UB.
int board_from_file(char *filename, struct Board *board) {
    FILE *file;
    file = fopen(filename, "r");

    // Si on ne peut pas ouvrir le fichier.
    if (file == NULL) return 1;

    // Lecture de chaque char.
    char c;
    size_t x = 0, y = 0;

    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            y = 0;
            x++;
            continue;
        }

        // Tous les caractères sont considérés comme true sauf le 0.
        board->cells[x][y] = c != '0'; 
        y++;
    }

    // Fermeture du fichier et renvoie de la valeur.
    fclose(file);
    return 0;
}


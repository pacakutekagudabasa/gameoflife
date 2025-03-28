#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#include "board.h"

#define SWAP(type, x, y) \
    type temp = x; \
    x = y; \
    y = temp;

// Fonction pour attendre un certain temps en milisecondes.
int msleep(long msec) {
    struct timespec ts;
    int res;

    if (msec < 0) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

int main(int argc, char **argv) {
    int error_code = 0;

    // Set des dimentions.
    const size_t height = 64;
    const size_t width = height;

    // Création des boards.
    struct Board *front = board_init(height, width);
    struct Board *back = board_init(height, width);

    // Chargement d'un fichier en fonction des arguments.
    if (argc > 1) {
        char *filename = argv[1];
        printf("Chargement de la grille contenue dans le fichier '%s'", filename);

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

    // Boucle principale.
    for (size_t i = 0; i < 1000; i++) {
        if (i != 0) printf("====");
        board_print(front);

        // Calcul de la prochaine intération dans l'autre buffer. 
        board_next(front, back);
        SWAP(struct Board *, front, back);
    
        // Attente avant d'afficher la prochain génération.
        msleep(50);
    }

cleanup:
    board_free(front);
    board_free(back);
    return error_code;
}

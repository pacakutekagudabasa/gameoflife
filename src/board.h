struct Board {
    int **cells;
    size_t height;
    size_t width;
};

struct Board *board_init(size_t height, size_t width);
void board_print(struct Board *board);
void board_free(struct Board *board);
void board_next(struct Board *board, struct Board *out);
void board_random_fill(struct Board *board);
int board_from_file(char *filename, struct Board *board);

/**
 * @file board.c
 * @brief गेम ऑफ लाइफ के लिए बोर्ड मैनेजमेंट implementation
 * @author Game of Life Enhanced
 * @date 2025
 * 
 * यह फाइल बोर्ड के सभी operations को implement करती है।
 * Performance optimization के लिए 1D array का उपयोग किया गया है।
 */

#include <stdio.h>
#include <stdlib.h>

#include "board.h"

#define BOARD_HEIGHT 32 
#define BOARD_WIDTH BOARD_HEIGHT

#define MIN(x, y) ((x) < (y) ? x : y)
#define MAX(x, y) ((x) > (y) ? x : y)

/**
 * @brief नया बोर्ड initialize करता है और memory allocate करता है
 * 
 * यह function एक नया Board struct create करता है और सभी cells को
 * zero (मृत) state में initialize करता है।
 * 
 * @param height बोर्ड की ऊंचाई (rows की संख्या)
 * @param width बोर्ड की चौड़ाई (columns की संख्या)
 * @return सफल होने पर Board pointer, memory allocation fail होने पर NULL
 */
Board *board_init(size_t height, size_t width) {
    Board *board = malloc(sizeof(Board));
    if (!board) {
        return NULL;
    }    
    // बोर्ड dimensions set करें
    board->height = height;
    board->width = width;

    // सभी cells को zero (मृत) state में initialize करें
    board->cells = calloc(sizeof(void *), height * width);

    return board;
}

/**
 * @brief बोर्ड को terminal में visual format में print करता है
 * 
 * यह function बोर्ड को terminal में Unicode blocks का उपयोग करके
 * print करता है। जीवित cells ██ और मृत cells "  " के रूप में दिखाई जाती हैं।
 * 
 * @param board print करने वाला बोर्ड
 * @return सफल होने पर 0, memory allocation fail होने पर -1
 */
int board_print(Board *board) {
    // एक line के लिए buffer allocate करें (प्रति cell 2 chars + null terminator)
    char *line = malloc(board->width * 2 + 1);
    
    if (line == NULL) {
        return -1;
    }
    
    for (size_t i = 0; i < board->width * board->height; i++) {
        // 1D index को 2D coordinates में convert करें
        size_t x = i / board->width;
        size_t y = i % board->width;
        COPY_CELL(&line[y * 2], board->cells[x * board->width + y]);
        // जब row का अंत आ जाए तो line print करें
        if (y == board->width - 1) {
            line[board->width * 2] = '\0';  // Null terminate
            printf("%s\n", line);
        }
    }
    
    free(line);
    return 0;
}

/**
 * @brief बोर्ड की सारी allocated memory को free करता है
 * 
 * यह function बोर्ड के cells array और बोर्ड struct दोनों की
 * memory को safely free करता है।
 * 
 * @param board free करने वाला बोर्ड
 * @return सफल होने पर 0, NULL pointer होने पर -1
 */
int board_free(Board *board) {
    if (board == NULL) return -1;
    
    // cells array की memory free करें
    free(board->cells);
    
    // बोर्ड struct की memory free करें
    free(board);
    
    return 0;
}

/**
 * @brief specified rules के अनुसार cell अगली generation में जीवित रहेगी या नहीं check करता है
 * 
 * यह function दिए गए cell के आसपास के neighbors count करता है और
 * current rules के अनुसार determine करता है कि cell next generation में
 * जीवित रहेगी या मरेगी।
 * 
 * @param board current बोर्ड state
 * @param rules apply करने वाले game rules
 * @param x cell का x coordinate (row)
 * @param y cell का y coordinate (column)
 * @param result परिणाम store करने के लिए pointer (1=जीवित रहेगी, 0=मरेगी)
 * @return सफल होने पर 0, invalid parameters होने पर -1
 */
int is_cell_alive_next_gen(struct Board *board, Rules *rules, size_t x, size_t y, int *result) {
    if (board == NULL || rules == NULL || result == NULL || x >= board->height || y >= board->width) return -1;
    
    // cell के आसपास के जीवित neighbors count करें
    int count = 0;
    for (size_t i = MAX((int)x-1, 0); i <= MIN(x+1, board->height - 1); i++) {
        for (size_t j = MAX((int)y-1, 0); j <= MIN(y+1, board->width - 1); j++) {
            // current cell को count न करें
            if (i == x && j == y) continue; 

            // neighbor जीवित है तो count increment करें
            // 2D coordinates को 1D index में convert करें
            size_t index = i * board->width + j;
            count += board->cells[index];
        }
    }

    // rules apply करके determine करें कि cell जीवित रहेगी या मरेगी
    size_t current_index = x * board->width + y;
    return rules_apply(rules, board->cells[current_index], count, result);
}

/**
 * @brief specified rules का उपयोग करके अगली generation का बोर्ड generate करता है
 * 
 * यह function current बोर्ड state को input के रूप में लेता है और
 * दिए गए rules के अनुसार next generation को output बोर्ड में calculate करता है।
 * Double buffering technique का उपयोग किया गया है।
 * 
 * @param board current generation का बोर्ड
 * @param out next generation store करने के लिए output बोर्ड
 * @param rules apply करने वाले game rules
 * @return सफल होने पर 0, error होने पर -1
 */
int board_next(Board *board, Board *out, Rules *rules) {
    if (board == NULL || out == NULL || rules == NULL) return -1;
    if (board->width != out->width || board->height != out->height) return -1;
    
    // 1D array में single loop - better performance
    for (size_t i = 0; i < board->width * board->height; i++) {
        // 1D index को 2D coordinates में convert करें neighbor checking के लिए
        size_t x = i / board->width;
        size_t y = i % board->width;
        
        int cell_result;
        int status = is_cell_alive_next_gen(board, rules, x, y, &cell_result);
        if (status == -1) return -1; // Error from is_cell_alive_next_gen
        
        out->cells[i] = cell_result;
    }
    
    return 0;
}

/**
 * @brief min और max के बीच random number generate करता है
 * @param min minimum value (inclusive)
 * @param max maximum value (inclusive)
 * @return min और max के बीच का random integer
 */
static int randint(int min, int max) {
    return rand() % (max + 1 - min) + min;
}

/**
 * @brief बोर्ड को random values से fill करता है
 * 
 * यह function बोर्ड के सभी cells को randomly जीवित या मृत state में
 * set करता है। जीवित होने की probability approximately 20% है।
 * 
 * @param board fill करने वाला बोर्ड
 * @return सफल होने पर 0, NULL pointer होने पर -1
 */
int board_random_fill(Board *board) {
    if (board == NULL) return -1;
    
    // 1D array में single loop
    for (size_t i = 0; i < board->width * board->height; i++) {
        board->cells[i] = randint(0, 4) == 0;  // 20% chance of being alive
    }
    
    return 0;
}

/**
 * @brief बोर्ड के सभी cells को clear करता है (सभी को मृत बनाता है)
 * 
 * यह function बोर्ड को reset करने के लिए उपयोग किया जाता है।
 * सभी cells को 0 (मृत) state में set कर देता है।
 * 
 * @param board clear करने वाला बोर्ड
 * @return सफल होने पर 0, NULL pointer होने पर -1
 */
int board_clear(Board *board) {
    if (board == NULL) return -1;
    
    for (size_t i = 0; i < board->width * board->height; i++) {
        board->cells[i] = 0;
    }
    
    return 0;
}

/**
 * @brief text file से बोर्ड pattern load करता है
 * 
 * यह function text file से pattern read करके बोर्ड में load करता है।
 * File format: '0' = मृत cell, कोई भी अन्य character = जीवित cell।
 * प्रत्येक line एक row represent करती है।
 * 
 * @param filename load करने वाली file का नाम
 * @param board target बोर्ड जहाँ pattern load करना है
 * @return सफल होने पर 0, file error या bounds error होने पर -1
 */
int board_from_file(char *filename, Board *board) {
    if (filename == NULL || board == NULL) return -1;
    
    FILE *file = NULL;
    file = fopen(filename, "r");

    // अगर file open नहीं हो सकी
    if (file == NULL) return -1;

    // पहले बोर्ड को clear करें
    board_clear(board);

    // प्रत्येक character read करें
    char c = 0;
    size_t x = 0, y = 0;

    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            y = 0;
            x++;
            continue;
        }

        // bounds check करें
        if (x >= board->height || y >= board->width) {
            fclose(file);
            return -1;
        }

        // 2D coordinates को 1D index में convert करें
        size_t index = x * board->width + y;
        // '0' को छोड़कर सभी characters को जीवित cell माना जाता है
        board->cells[index] = c != '0'; 
        y++;
    }

    // file close करें और success return करें
    fclose(file);
    return 0;
}

/**
 * @file board.h
 * @brief गेम ऑफ लाइफ के लिए बोर्ड मैनेजमेंट हेडर फाइल
 * @author Game of Life Enhanced
 * @date 2025
 * 
 * यह फाइल बोर्ड स्ट्रक्चर और इसके सभी ऑपरेशन्स को डिफाइन करती है।
 * बोर्ड एक 1D array के रूप में implemented है बेहतर performance के लिए।
 */

#ifndef BOARD_H
#define BOARD_H

#include <string.h>  // For memcpy in COPY_CELL macro
#include "rules.h"   // Include rules system

/**
 * @brief गेम बोर्ड स्ट्रक्चर जो सभी cells को store करता है
 * 
 * यह स्ट्रक्चर गेम के grid को represent करता है। Memory efficiency के लिए
 * 1D array का उपयोग किया गया है instead of 2D array।
 */
typedef struct Board {
    char *cells;        /**< सेल्स का 1D array (0=मृत, 1=जीवित) */
    size_t height;      /**< बोर्ड की ऊंचाई */
    size_t width;       /**< बोर्ड की चौड़ाई */
} Board;

/**
 * @brief सेल को terminal में print करने के लिए macro
 * @param dest गंतव्य buffer
 * @param is_filled सेल जीवित है या नहीं (1=जीवित, 0=मृत)
 */
#define COPY_CELL(dest, is_filled) \
    memcpy(dest, (is_filled) ? "██" : "  ", (is_filled) ? 6 : 2)

/**
 * @brief नया बोर्ड initialize करता है
 * @param height बोर्ड की ऊंचाई
 * @param width बोर्ड की चौड़ाई
 * @return सफल होने पर Board pointer, असफल होने पर NULL
 */
Board *board_init(size_t height, size_t width);

/**
 * @brief बोर्ड को terminal में print करता है
 * @param board प्रिंट करने वाला बोर्ड
 * @return सफल होने पर 0, असफल होने पर -1
 */
int board_print(Board *board);

/**
 * @brief बोर्ड की memory को free करता है
 * @param board free करने वाला बोर्ड
 * @return सफल होने पर 0, असफल होने पर -1
 */
int board_free(Board *board);

/**
 * @brief rules के अनुसार सेल अगली generation में जीवित रहेगी या नहीं check करता है
 * @param board current बोर्ड
 * @param rules apply करने वाले rules
 * @param x सेल का x coordinate
 * @param y सेल का y coordinate
 * @param result परिणाम store करने के लिए pointer (1=जीवित, 0=मृत)
 * @return सफल होने पर 0, असफल होने पर -1
 */
int is_cell_alive_next_gen(struct Board *board, Rules *rules, size_t x, size_t y, int *result);

/**
 * @brief specified rules का उपयोग करके अगली generation generate करता है
 * @param board current बोर्ड
 * @param out output बोर्ड जहाँ next generation store होगी
 * @param rules apply करने वाले rules
 * @return सफल होने पर 0, असफल होने पर -1
 */
int board_next(Board *board, Board *out, Rules *rules);

/**
 * @brief बोर्ड को random values से fill करता है
 * @param board fill करने वाला बोर्ड
 * @return सफल होने पर 0, असफल होने पर -1
 */
int board_random_fill(Board *board);

/**
 * @brief file से बोर्ड load करता है
 * @param filename load करने वाली file का नाम
 * @param board target बोर्ड
 * @return सफल होने पर 0, असफल होने पर -1
 */
int board_from_file(char *filename, Board *board);

/**
 * @brief बोर्ड के सभी cells को clear करता है (सभी को मृत बनाता है)
 * @param board clear करने वाला बोर्ड
 * @return सफल होने पर 0, असफल होने पर -1
 */
int board_clear(Board *board);

#endif // BOARD_H

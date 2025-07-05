/**
 * @file rules.h
 * @brief गेम ऑफ लाइफ के लिए configurable rules system का हेडर
 * @author Game of Life Enhanced
 * @date 2025
 * 
 * यह फाइल विभिन्न प्रकार के cellular automata rules को define और manage करने
 * के लिए बनाई गई है। Bit mask technique का उपयोग करके efficient rule checking
 * प्रदान करती है।
 */

#ifndef RULES_H
#define RULES_H

#include <stdint.h>

/**
 * @brief एक standard grid में maximum neighbors की संख्या (8-connected grid में)
 */
#define MAX_NEIGHBORS 8

/**
 * @brief Game rules को represent करने वाला structure
 * 
 * यह structure bit mask technique का उपयोग करके birth और survival conditions
 * को efficiently store करता है। प्रत्येक bit एक specific neighbor count के लिए
 * rule को represent करता है।
 */
typedef struct Rules {
    uint16_t birth_rules;    /**< Birth conditions का bit mask (index = neighbor count, bit = rule active) */
    uint16_t survival_rules; /**< Survival conditions का bit mask */
    char name[64];           /**< Rule set का descriptive नाम */
} Rules;

/**
 * @brief birth और survival neighbor counts के साथ rules initialize करता है
 * @param name rule set का नाम (descriptive name)
 * @param birth_counts birth के लिए valid neighbor counts का array
 * @param birth_len birth_counts array का length
 * @param survival_counts survival के लिए valid neighbor counts का array
 * @param survival_len survival_counts array का length
 * @return सफल होने पर Rules pointer, memory allocation fail होने पर NULL
 */
Rules *rules_init(const char *name, const int *birth_counts, int birth_len, 
                  const int *survival_counts, int survival_len);

/**
 * @brief Classic Conway's Game of Life rules create करता है (B3/S23)
 * 
 * Conway के original rules:
 * - Birth: exactly 3 neighbors
 * - Survival: 2 या 3 neighbors
 * 
 * @return Conway's Life rules का pointer
 */
Rules *rules_conway(void);

/**
 * @brief HighLife variant rules create करता है (B36/S23)
 * 
 * HighLife rules में replicator patterns possible हैं:
 * - Birth: 3 या 6 neighbors
 * - Survival: 2 या 3 neighbors
 * 
 * @return HighLife rules का pointer
 */
Rules *rules_highlife(void);

/**
 * @brief Day & Night rules create करता है (B3678/S34678)
 * 
 * Symmetric rules जहाँ empty space filled space की तरह behave करती है:
 * - Birth: 3, 6, 7, या 8 neighbors
 * - Survival: 3, 4, 6, 7, या 8 neighbors
 * 
 * @return Day & Night rules का pointer
 */
Rules *rules_day_night(void);

/**
 * @brief Maze generation rules create करता है (B3/S12345)
 * 
 * ये rules maze-like structures create करते हैं:
 * - Birth: exactly 3 neighbors
 * - Survival: 1, 2, 3, 4, या 5 neighbors
 * 
 * @return Maze rules का pointer
 */
Rules *rules_maze(void);

/**
 * @brief rules के अनुसार check करता है कि cell अगली generation में जीवित होगी या नहीं
 * @param rules apply करने वाले rules
 * @param current_state cell की current state (1=जीवित, 0=मृत)
 * @param neighbor_count जीवित neighbors की संख्या
 * @param result परिणाम store करने के लिए pointer (1=जीवित रहेगी, 0=मरेगी)
 * @return सफल होने पर 0, invalid parameters होने पर -1
 */
int rules_apply(Rules *rules, int current_state, int neighbor_count, int *result);

/**
 * @brief rules की memory को free करता है
 * @param rules free करने वाले rules
 */
void rules_free(Rules *rules);

/**
 * @brief rules का description console में print करता है
 * 
 * यह function rule set का नाम और सभी birth/survival conditions को
 * human-readable format में print करता है।
 * 
 * @param rules print करने वाले rules
 */
void rules_print(Rules *rules);

#endif // RULES_H

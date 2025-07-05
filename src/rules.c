/**
 * @file rules.c
 * @brief गेम ऑफ लाइफ के लिए configurable rules system का implementation
 * @author Game of Life Enhanced
 * @date 2025
 * 
 * यह फाइल विभिन्न cellular automata rules को implement करती है।
 * Bit mask technique का उपयोग करके fast rule checking प्रदान करती है।
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rules.h"

/**
 * @brief custom rules initialize करता है given birth और survival conditions के साथ
 * 
 * यह function एक generic rule set create करता है जो किसी भी birth/survival
 * combination को support करता है। Bit mask technique का उपयोग करके
 * O(1) time में rule checking possible हो जाती है।
 * 
 * @param name rule set का descriptive नाम
 * @param birth_counts birth के लिए valid neighbor counts का array
 * @param birth_len birth_counts array की length
 * @param survival_counts survival के लिए valid neighbor counts का array  
 * @param survival_len survival_counts array की length
 * @return सफल होने पर Rules pointer, memory allocation fail होने पर NULL
 */
Rules *rules_init(const char *name, const int *birth_counts, int birth_len, 
                  const int *survival_counts, int survival_len) {
    Rules *rules = malloc(sizeof(Rules));
    if (!rules) return NULL;
    
    // bit masks को zero से initialize करें
    rules->birth_rules = 0;
    rules->survival_rules = 0;
    
    // birth rules के लिए appropriate bits set करें
    for (int i = 0; i < birth_len; i++) {
        if (birth_counts[i] >= 0 && birth_counts[i] <= MAX_NEIGHBORS) {
            rules->birth_rules |= (1 << birth_counts[i]);
        }
    }
    
    // survival rules के लिए appropriate bits set करें
    for (int i = 0; i < survival_len; i++) {
        if (survival_counts[i] >= 0 && survival_counts[i] <= MAX_NEIGHBORS) {
            rules->survival_rules |= (1 << survival_counts[i]);
        }
    }
    
    // rule set का नाम copy करें
    strncpy(rules->name, name ? name : "Custom", sizeof(rules->name) - 1);
    rules->name[sizeof(rules->name) - 1] = '\0';
    
    return rules;
}

/**
 * @brief Classic Conway's Game of Life rules create करता है
 * 
 * John Conway द्वारा 1970 में invented original Game of Life rules:
 * - कोई भी मृत cell जिसके exactly 3 जीवित neighbors हैं वो जीवित हो जाती है (birth)
 * - कोई भी जीवित cell जिसके 2 या 3 जीवित neighbors हैं वो जीवित रहती है (survival)
 * - बाकी सभी cases में cell मर जाती है या मृत रहती है
 * 
 * @return Conway's Life rules का pointer
 */
Rules *rules_conway(void) {
    int birth[] = {3};
    int survival[] = {2, 3};
    return rules_init("Conway's Life (B3/S23)", birth, 1, survival, 2);
}

/**
 * @brief HighLife cellular automaton rules create करता है
 * 
 * Nathan Thompson द्वारा discovered ये rules Conway's Life को extend करते हैं:
 * - Birth: 3 या 6 neighbors
 * - Survival: 2 या 3 neighbors
 * 
 * इसमें self-replicating patterns possible हैं जो Conway's Life में नहीं हैं।
 * 
 * @return HighLife rules का pointer
 */
Rules *rules_highlife(void) {
    int birth[] = {3, 6};
    int survival[] = {2, 3};
    return rules_init("HighLife (B36/S23)", birth, 2, survival, 2);
}

/**
 * @brief Day & Night cellular automaton rules create करता है
 * 
 * ये symmetric rules हैं जहाँ birth और survival conditions similar हैं:
 * - Birth: 3, 6, 7, या 8 neighbors
 * - Survival: 3, 4, 6, 7, या 8 neighbors
 * 
 * इसमें patterns और उनके inverse दोनों stable होते हैं।
 * 
 * @return Day & Night rules का pointer
 */
Rules *rules_day_night(void) {
    int birth[] = {3, 6, 7, 8};
    int survival[] = {3, 4, 6, 7, 8};
    return rules_init("Day & Night (B3678/S34678)", birth, 4, survival, 5);
}

/**
 * @brief Maze generation rules create करता है
 * 
 * ये rules maze-like structures generate करते हैं:
 * - Birth: exactly 3 neighbors (Conway's के समान)
 * - Survival: 1, 2, 3, 4, या 5 neighbors (बहुत permissive)
 * 
 * Higher survival range के कारण complex, interconnected patterns बनते हैं।
 * 
 * @return Maze rules का pointer
 */
Rules *rules_maze(void) {
    int birth[] = {3};
    int survival[] = {1, 2, 3, 4, 5};
    return rules_init("Maze (B3/S12345)", birth, 1, survival, 5);
}

/**
 * @brief given rules के अनुसार cell का next state determine करता है
 * 
 * यह function cell की current state और उसके neighbors की count के
 * अनुसार decide करता है कि cell next generation में जीवित रहेगी या मरेगी।
 * Bit mask technique का उपयोग करके O(1) time में result मिलता है।
 * 
 * @param rules apply करने वाले rules
 * @param current_state cell की current state (1=जीवित, 0=मृत)
 * @param neighbor_count जीवित neighbors की संख्या (0-8)
 * @param result next state store करने के लिए pointer (1=जीवित रहेगी, 0=मरेगी)
 * @return सफल होने पर 0, invalid parameters होने पर -1
 */
int rules_apply(Rules *rules, int current_state, int neighbor_count, int *result) {
    if (!rules || !result || neighbor_count < 0 || neighbor_count > MAX_NEIGHBORS) {
        return -1;
    }
    
    if (current_state) {
        // cell जीवित है - survival rules check करें
        *result = (rules->survival_rules & (1 << neighbor_count)) != 0;
    } else {
        // cell मृत है - birth rules check करें
        *result = (rules->birth_rules & (1 << neighbor_count)) != 0;
    }
    
    return 0;
}

/**
 * @brief rules की allocated memory को free करता है
 * 
 * यह function safely rules structure की memory को release करता है।
 * NULL pointer को handle करता है।
 * 
 * @param rules free करने वाले rules (NULL safe)
 */
void rules_free(Rules *rules) {
    if (rules) {
        free(rules);
    }
}

/**
 * @brief rules का detailed description console में print करता है
 * 
 * यह function rule set का नाम और सभी active birth/survival conditions को
 * human-readable format में display करता है। Debugging और user information
 * के लिए उपयोगी है।
 * 
 * @param rules print करने वाले rules (NULL safe)
 */
void rules_print(Rules *rules) {
    if (!rules) return;
    
    printf("Rules: %s\n", rules->name);
    printf("Birth conditions (neighbor count): ");
    for (int i = 0; i <= MAX_NEIGHBORS; i++) {
        if (rules->birth_rules & (1 << i)) {
            printf("%d ", i);
        }
    }
    printf("\nSurvival conditions (neighbor count): ");
    for (int i = 0; i <= MAX_NEIGHBORS; i++) {
        if (rules->survival_rules & (1 << i)) {
            printf("%d ", i);
        }
    }
    printf("\n");
}

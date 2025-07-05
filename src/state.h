/**
 * @file state.h
 * @brief गेम state management के लिए हेडर फाइल
 * @author Game of Life Enhanced
 * @date 2025
 * 
 * यह फाइल game की current state को track करने के लिए structures और
 * functions को define करती है। इसमें pause state, loaded files, और
 * user interaction modes शामिल हैं।
 */

#ifndef STATE_H
#define STATE_H 

#include <stdint.h>

/**
 * @brief 8-bit boolean type definition space efficiency के लिए
 */
typedef uint8_t bool8;

/**
 * @brief boolean true value (1)
 */
#define true 1

/**
 * @brief boolean false value (0)
 */
#define false 0

/**
 * @brief Game की current state को represent करने वाला structure
 * 
 * यह structure game के execution के दौरान सभी important state information
 * को track करता है, including user preferences और current modes।
 */
typedef struct State {
    bool8 keep_alive;        /**< Game loop चालू रखना है या बंद करना है */
    bool8 pause;             /**< Simulation pause है या चल रहा है */
    char *loaded_filename;   /**< Currently loaded file का नाम (reloading के लिए) */
    int current_rule_index;  /**< Current active rule set का index */
    bool8 is_dragging;       /**< User mouse drag कर रहा है या नहीं */
    bool8 drag_paint_mode;   /**< Drag करते समय क्या paint करना है (1=जीवित, 0=मृत) */
} State;

/**
 * @brief नया State structure initialize करता है
 * 
 * यह function State structure के लिए memory allocate करता है और
 * सभी fields को default values के साथ initialize करता है।
 * 
 * @param state_ptr double pointer जहाँ allocated State का address store होगा
 * @return सफल होने पर 0, memory allocation fail होने पर -1
 */
int state_init(State **state_ptr);

/**
 * @brief State structure की memory को free करता है
 * 
 * यह function State struct और उसके सभी allocated members (जैसे filename)
 * की memory को safely release करता है।
 * 
 * @param state_ptr free करने वाला State pointer
 * @return सफल होने पर 0, NULL pointer होने पर -1
 */
int state_free(State *state_ptr);

/**
 * @brief State में loaded filename को set या update करता है
 * 
 * यह function state में current loaded file का नाम store करता है
 * ताकि बाद में reload operation perform कर सकें। पहले से stored filename
 * को properly free करता है।
 * 
 * @param state target State structure
 * @param filename set करने वाला filename (NULL होने पर current filename clear हो जाता है)
 * @return सफल होने पर 0, memory allocation fail होने पर -1
 */
int state_set_filename(State *state, const char *filename);

#endif // STATE_H

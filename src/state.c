/**
 * @file state.c
 * @brief गेम state management का implementation
 * @author Game of Life Enhanced
 * @date 2025
 * 
 * यह फाइल game state के lifecycle management को handle करती है।
 * Memory management और safe initialization/cleanup operations provide करती है।
 */

#include <stdlib.h>
#include <string.h>
#include "state.h"

/**
 * @brief नया State structure create और initialize करता है
 * 
 * यह function memory allocate करके एक नया State structure create करता है
 * और सभी fields को sensible default values के साथ initialize करता है।
 * Game initially paused state में start होता है।
 * 
 * @param state_ptr double pointer जहाँ allocated State का address store होगा
 * @return सफल होने पर 0, NULL pointer या memory allocation fail होने पर -1
 * 
 * @note Caller को state_free() call करना चाहिए जब State की जरूरत न हो
 */
int state_init(State **state_ptr) {
    if (!state_ptr) {
        return -1; // NULL pointer error handle करें
    }
    if (!(*state_ptr = malloc(sizeof(State)))) {
        return -1; // Memory allocation failure handle करें
    }
    
    // सभी fields को default values के साथ initialize करें
    (*state_ptr)->keep_alive = true;           // Game initially running state में
    (*state_ptr)->pause = false;               // Simulation initially running
    (*state_ptr)->loaded_filename = NULL;      // कोई file initially loaded नहीं
    (*state_ptr)->current_rule_index = 0;      // Default Conway's rules (index 0)
    (*state_ptr)->is_dragging = false;         // User initially drag नहीं कर रहा
    (*state_ptr)->drag_paint_mode = true;      // Default में alive cells paint करें
    
    return 0;
}

/**
 * @brief State structure और उसकी सारी allocated memory को free करता है
 * 
 * यह function State struct और उसके सभी dynamically allocated members
 * की memory को safely release करता है। Memory leaks को prevent करता है।
 * 
 * @param state_ptr free करने वाला State pointer
 * @return सफल होने पर 0, NULL pointer होने पर -1
 * 
 * @note यह function NULL pointer को safely handle करता है
 */
int state_free(State *state_ptr) {
    if (!state_ptr) {
        return -1; // NULL pointer error handle करें
    }
    
    // पहले filename की memory free करें अगर allocated है
    if (state_ptr->loaded_filename) {
        free(state_ptr->loaded_filename);
    }
    
    // फिर State structure की memory free करें
    free(state_ptr);
    return 0;
}

/**
 * @brief State में loaded filename को set या update करता है
 * 
 * यह function current loaded file का नाम State में store करता है
 * ताकि बाद में reload functionality use कर सकें। यह safely previous
 * filename की memory को free करता है और नई string के लिए memory allocate करता है।
 * 
 * @param state target State structure
 * @param filename set करने वाला filename (NULL होने पर current clear हो जाता है)
 * @return सफल होने पर 0, NULL state या memory allocation fail होने पर -1
 * 
 * @note अगर filename NULL है तो current filename clear हो जाता है
 * @note यह function string की copy बनाता है, original को modify नहीं करता
 */
int state_set_filename(State *state, const char *filename) {
    if (!state) return -1;
    
    // पहले existing filename की memory free करें अगर कोई है
    if (state->loaded_filename) {
        free(state->loaded_filename);
        state->loaded_filename = NULL;
    }
    
    // नई filename set करें अगर provided है
    if (filename) {
        // नई string के लिए memory allocate करें
        state->loaded_filename = malloc(strlen(filename) + 1);
        if (!state->loaded_filename) return -1; // Memory allocation fail
        
        // filename को copy करें
        strcpy(state->loaded_filename, filename);
    }
    
    return 0;
}

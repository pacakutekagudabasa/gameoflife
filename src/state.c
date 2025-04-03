#include <stdlib.h>

#include "state.h"

struct State *state_init() {
    struct State *state = malloc(sizeof(struct State));
    state->keep_alive = 1;
    state->pause = 0;

    return state;
}

#pragma once

#include "config.h"
#include "raylib.h"
#include <stdint.h>

typedef struct state_t State;
typedef void (*state_function)(void);

struct state_t {
    enum {
        s_init,
        s_update,
        s_exit,
    } scene_state;
    state_function init;
    state_function update;
    state_function render;
    state_function exit;
};

RLAPI void bootloader(State *state);

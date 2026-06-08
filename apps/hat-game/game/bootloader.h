#pragma once

#include "config.h"
#include "raylib.h"
#include <stdint.h>

typedef struct state_t State;
typedef void (*state_function)(void);

struct state_t {
    state_function init;
    state_function update;
    state_function render;
    state_function exit;
};

RLAPI void bootloader(State *state);

RLAPI void change_scene(State *state, state_function init,
                        state_function update, state_function render,
                        state_function exit);

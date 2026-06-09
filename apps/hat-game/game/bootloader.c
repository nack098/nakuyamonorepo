#include "bootloader.h"
#include "hat-making.h"

#include <nakuyadebugger.h>

RLAPI void bootloader(State *state) {
    LOG_DEBUG("Bootloader Initialize");
    state->init = &hat_making_init;
    state->update = &hat_making_update;
    state->render = &hat_making_render;
    state->exit = &hat_making_exit;
}

void change_scene(State *state, state_function init, state_function update,
                  state_function render, state_function exit) {
    if (state->exit) {
        state->exit();
    }

    state->init = init;
    state->update = update;
    state->render = render;
    state->exit = exit;

    if (state->init) {
        state->init();
    }
}

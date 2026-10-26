#include "bootloader.h"
#include "todos.h"

#include <nakuyadebugger.h>

RLAPI void bootloader(State *state) {
    LOG_DEBUG("Bootloader Initialize");
    state->init = &todos_init;
    state->update = &todos_update;
    state->render = &todos_render;
    state->exit = &todos_exit;
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

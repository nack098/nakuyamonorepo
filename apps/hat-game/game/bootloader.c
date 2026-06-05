#include "bootloader.h"
#include "hat-making.h"

#include <nakuyadebugger.h>

RLAPI void bootloader(State *state) {
    LOG_DEBUG("Bootloader Initialize");
    state->scene_state = s_init;
    state->init = &hat_cutting_init;
    state->update = &hat_cutting_update;
    state->render = &hat_cutting_render;
    state->exit = &hat_cutting_exit;
}

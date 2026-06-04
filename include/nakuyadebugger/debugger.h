#pragma once

#include <raylib.h>
#include <stdint.h>

#define USE_LIBTYPE_SHARED

extern uint8_t debugger_isopen;

RLAPI void debugger_init(void);
RLAPI void debugger_button(void);
RLAPI void debugger_ui(void);

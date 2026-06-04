#include <nakuyadebugger.h>

#include "raygui.h"
#include "raylib.h"

uint8_t debugger_isopen = 0;

RLAPI void debugger_init(void) { debugger_isopen = 0; }

RLAPI void debugger_button(void) {
    if (GuiButton((Rectangle){24, 24, 120, 30}, "#191#Open Debugger"))
        debugger_isopen = 1;
}

RLAPI void debugger_ui(void) {
    if (debugger_isopen) {
        int result =
            GuiMessageBox((Rectangle){85, 70, 250, 100}, "#191#Message Box",
                          "Hi! This is a message!", "Nice;Cool");

        if (result >= 0)
            debugger_isopen = false;
    }
}

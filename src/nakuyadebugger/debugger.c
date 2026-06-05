#include <nakuyadebugger.h>

#include "raygui.h"
#include "raylib.h"
#include <string.h>

extern int logger_get_ui_count(void);
extern const char *logger_get_ui_line(int index);

uint8_t debugger_isopen = 0;

RLAPI void debugger_init(void) { debugger_isopen = 0; }

RLAPI void debugger_button(void) {
    if (GuiButton((Rectangle){24, 24, 120, 30}, "#191#Open Debugger"))
        debugger_isopen = (debugger_isopen + 1) % 2;
}

RLAPI void debugger_ui(void) {
    if (debugger_isopen) {
        int screen_w = GetScreenWidth();
        int screen_h = GetScreenHeight();

        Rectangle panel_rec = {24.0f, (float)(screen_h - 224),
                               (float)(screen_w - 48), 200.0f};

        int total_logs = logger_get_ui_count();
        int start_y = (int)panel_rec.y + 32;
        int line_spacing = 13;

        GuiPanel(panel_rec, "CONSOLE LOGS");

        if (GuiButton((Rectangle){panel_rec.x + panel_rec.width - 65,
                                  panel_rec.y + 4, 60, 18},
                      "Hide")) {
            debugger_isopen = 0;
        }

        for (int i = 0; i < total_logs; i++) {
            const char *log_text = logger_get_ui_line(i);
            Color text_color = DARKGRAY;

            if (strstr(log_text, "DEBUG"))
                text_color = BLUE;
            else if (strstr(log_text, "WARN"))
                text_color = ORANGE;
            else if (strstr(log_text, "ERROR"))
                text_color = RED;

            DrawText(log_text, (int)panel_rec.x + 12,
                     start_y + (i * line_spacing), 12, text_color);
        }
    }
}

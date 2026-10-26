#include "loader-config.h"
#include "loader.h"
#include "systems.h"

#include <nakuyadebugger.h>
#include <stdbool.h>

static bool is_running = false;

int main(void) {
    State game_state = {0};

    SetConfigFlags(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_UNDECORATED);
    InitWindow(WIDTH, HEIGHT, NAME);
    ChangeDirectory(GetApplicationDirectory());

    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    debugger_init();

    if (load_game_code(&game_state)) {
        if (game_state.init)
            game_state.init();
    }

    is_running = true;

    while (is_running) {
        if (WindowShouldClose())
            break;

        // if (IsKeyPressed(DEBUG_KEY)) {
        //     if (load_game_code(&game_state)) {
        //         if (game_state.init)
        //             game_state.init();
        //     }
        // }

        if (game_state.update)
            game_state.update();

        BeginDrawing();

        if (game_state.render)
            game_state.render();

        // debugger_button(0, HEIGHT - 48);
        // debugger_ui();
        EndDrawing();
    }

    unload_game_code();
    CloseWindow();

    platform_delete_file(LIB_TEMP);
    return 0;
}

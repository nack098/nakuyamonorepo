#include "config.h"

#include "math.h"
#include "nakuyadebugger/debugger.h"
#include <math.h>
#include <raylib.h>
#include <stdint.h>

#include <nakuyadebugger.h>

void input_handler(void);
void render(void);
int main(void);
typedef struct player_t Player;

extern Player player;

struct player_t {
    float x, y;
};

Player player = {.x = 0., .y = 0.};

int main(void) {
    InitWindow(WIDTH, HEIGHT, NAME);
    debugger_init();

    while (!WindowShouldClose()) {
        input_handler();

        BeginDrawing();
        ClearBackground(BLACK);
        render();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void input_handler(void) {
    float vx, vy, delta, len;

    vx = 0;
    vy = 0;

    if (IsKeyDown(KEY_W))
        vy = -1;
    if (IsKeyDown(KEY_A))
        vx = -1;
    if (IsKeyDown(KEY_S))
        vy = 1;
    if (IsKeyDown(KEY_D))
        vx = 1;

    delta = GetFrameTime();
    len = sqrtf((float)(vx * vx + vy * vy));
    LOG_DEBUG("Test %s", "A");

    if (len > 0.0f) {
        vx /= len;
        vy /= len;

        player.x += vx * VELOCITY * OFFSET * delta;
        player.y += vy * VELOCITY * OFFSET * delta;
    }
}

void render(void) {
    DrawCircle((uint16_t)player.x, (uint16_t)player.y, 20, RED);

    debugger_button();
    debugger_ui();
}

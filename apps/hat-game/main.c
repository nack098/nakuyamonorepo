#include "math.h"
#include <raylib.h>
#include <stdint.h>

#define NAME "HatGame"
#define WIDTH 720
#define HEIGHT 480
#define LEN(x) sizeof(x) / sizeof(x[0])

void render(void);
int main(void);

int main(void) {
    InitWindow(WIDTH, HEIGHT, NAME);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        render();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void render(void) {
    char str[] = "Hello Furry";
    uint16_t font_size = 20;
    DrawText(str, WIDTH / 2 - LEN(str), HEIGHT / 2 - font_size / 2, 20, WHITE);
}

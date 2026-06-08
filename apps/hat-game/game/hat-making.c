#include "hat-making.h"
#include "nakuyadebugger.h"
#include <raylib.h>
#include <stdint.h>

HatCutterState state;

#define SCROLL_SPEED 4
#define CUTTER_OFFSET 1.575

Asset asset_load(const char *path, float width, float height) {
    Asset asset = {0};
    asset.img = LoadImage(path);

    if (width > 0.0f && height > 0.0f) {
        ImageResize(&asset.img, (int)width, (int)height);
        asset.width = width;
        asset.height = height;
    } else {
        asset.width = (float)asset.img.width;
        asset.height = (float)asset.img.height;
    }

    asset.tex = LoadTextureFromImage(asset.img);
    return asset;
}

void asset_free(Asset asset) {
    UnloadTexture(asset.tex);
    UnloadImage(asset.img);
}

void hat_cutting_init(void) {
    int screen_w = GetScreenWidth();
    int screen_h = GetScreenHeight();

    Hat hat = {0};
    HatCutter cutter = {0};

    hat.asset = asset_load("assets/hat.png", 0.0f, 0.0f);
    hat.pos.x = (float)screen_w / 2.0f;
    hat.pos.y = (float)screen_h / 2.0f;

    cutter.cutter_dist = 0;
    cutter.pos.x = (float)screen_w / 2.0f;
    cutter.pos.y = (float)screen_h / 2.0f;
    cutter.head = asset_load("assets/hat_cutter_head.png", 0.0f, 0.0f);
    cutter.body = asset_load("assets/hat_cutter_body.png", 0.0f, 0.0f);

    state.hat = hat;
    state.cutter = cutter;
}

void hat_cutting_update(void) {
    HatCutter *cutter;
    cutter = &state.cutter;

    float scroll = GetMouseWheelMove() * SCROLL_SPEED;
    if (cutter->cutter_dist + scroll >= 100) {
        cutter->cutter_dist = 100;
    } else {
        cutter->cutter_dist += scroll;
    }
}

void hat_cutting_render(void) {
    ClearBackground(WHITE);

    DrawTexture(state.cutter.body.tex,
                (int)(state.cutter.pos.x - state.cutter.body.width / 2.0f),
                (int)(state.cutter.pos.y - state.cutter.body.height / 2.0f),
                WHITE);

    DrawTexture(state.cutter.head.tex,
                (int)(state.cutter.pos.x - state.cutter.head.width / 2.0f),
                (int)(state.cutter.pos.y - state.cutter.head.height / 2.0f +
                      state.cutter.cutter_dist * CUTTER_OFFSET),
                WHITE);
}

void hat_cutting_exit(void) {
    asset_free(state.hat.asset);
    asset_free(state.cutter.head);
    asset_free(state.cutter.body);
}

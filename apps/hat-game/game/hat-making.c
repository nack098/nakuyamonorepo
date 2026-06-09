#include "hat-making.h"
#include "nakuyadebugger.h"
#include <raylib.h>
#include <stdbool.h>
#include <stdint.h>

#define SCROLL_SPEED 4.0f
#define CUTTER_OFFSET 1.575f
#define ANIMATION_SPEED 2.5f

HatMakingState hat_making_state;

// --- ASSET UTILITIES ---

Asset asset_load(const char *path, float width, float height) {
    Asset asset;

    asset = (Asset){0};
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

// --- PRIVATE STATIC HELPERS ---

static Rectangle get_cutter_click_bounds(HatCutter *cutter) {
    Rectangle bounds;

    bounds = (Rectangle){cutter->pos.x - cutter->body.width / 2.0f,
                         cutter->pos.y - cutter->body.height / 2.0f,
                         cutter->body.width, cutter->body.height};

    return bounds;
}

static bool should_dismiss_cutter(Hat *hat) {
    Rectangle hat_bounds;

    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return false;
    }

    hat_bounds = (Rectangle){hat->pos.x - hat->asset.width / 2.0f,
                             hat->pos.y - hat->asset.height / 2.0f,
                             hat->asset.width, hat->asset.height};

    return !CheckCollisionPointRec(GetMousePosition(), hat_bounds);
}

static void update_docked_state(HatCutter *cutter) {
    Vector2 mouse_pos;
    Rectangle click_bounds;

    mouse_pos = GetMousePosition();
    click_bounds = get_cutter_click_bounds(cutter);

    if (CheckCollisionPointRec(mouse_pos, click_bounds) &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        cutter->state = CUTTER_STATE_MOVING_TO_CENTER;
        cutter->transition_progress = 0.0f;
    }
}

static void update_transition_state(HatCutter *cutter, float dt) {
    float t;

    cutter->transition_progress += dt * ANIMATION_SPEED;

    if (cutter->transition_progress >= 1.0f) {
        cutter->transition_progress = 1.0f;
        cutter->pos = cutter->center_pos;
        cutter->state = CUTTER_STATE_ACTIVE;
    } else {
        t = cutter->transition_progress;
        cutter->pos.x = cutter->docked_pos.x +
                        (cutter->center_pos.x - cutter->docked_pos.x) * t;
        cutter->pos.y = cutter->docked_pos.y +
                        (cutter->center_pos.y - cutter->docked_pos.y) * t;
    }
}

static void update_return_transition_state(HatCutter *cutter, float dt) {
    float t;

    cutter->transition_progress += dt * ANIMATION_SPEED;

    if (cutter->transition_progress >= 1.0f) {
        cutter->transition_progress = 1.0f;
        cutter->pos = cutter->docked_pos;
        cutter->state = CUTTER_STATE_DOCKED;
    } else {
        t = cutter->transition_progress;
        cutter->pos.x = cutter->center_pos.x +
                        (cutter->docked_pos.x - cutter->center_pos.x) * t;
        cutter->pos.y = cutter->center_pos.y +
                        (cutter->docked_pos.y - cutter->center_pos.y) * t;
    }
}

static void update_active_input(HatCutter *cutter) {
    float scroll;
    int32_t next_dist;

    scroll = -1.0f * GetMouseWheelMove() * SCROLL_SPEED;
    next_dist = (int32_t)cutter->cutter_dist + (int32_t)scroll;

    if (next_dist > 100) {
        cutter->cutter_dist = 100;
    } else if (next_dist < 0) {
        cutter->cutter_dist = 0;
    } else {
        cutter->cutter_dist = (uint16_t)next_dist;
    }
}

// --- CORE GAME MODULE API ---

void hat_making_init(void) {
    int screen_w;
    int screen_h;
    Hat *hat;
    HatCutter *cutter;

    screen_w = GetScreenWidth();
    screen_h = GetScreenHeight();

    // Setup Hat
    hat = &hat_making_state.hat;
    hat->asset = asset_load("assets/hat.png", 0.0f, 0.0f);
    hat->pos.x = (float)screen_w / 2.0f;
    hat->pos.y = (float)screen_h / 2.0f;

    // Setup Cutter Configurations
    cutter = &hat_making_state.cutter;
    cutter->head = asset_load("assets/hat_cutter_head.png", 0.0f, 0.0f);
    cutter->body = asset_load("assets/hat_cutter_body.png", 0.0f, 0.0f);

    cutter->center_pos =
        (Position){(float)screen_w / 2.0f, (float)screen_h / 2.0f};
    cutter->docked_pos = (Position){
        (float)screen_w - (cutter->body.width / 3.0f), (float)screen_h / 2.0f};

    cutter->pos = cutter->docked_pos;
    cutter->state = CUTTER_STATE_DOCKED;
    cutter->cutter_dist = 0;

    hat_making_state.is_cutter_ui_open = true;
}

void hat_making_update(void) {
    HatCutter *cutter;
    Hat *hat;
    float dt;

    if (!hat_making_state.is_cutter_ui_open) {
        return;
    }

    cutter = &hat_making_state.cutter;
    hat = &hat_making_state.hat;
    dt = GetFrameTime();

    switch (cutter->state) {
    case CUTTER_STATE_DOCKED:
        update_docked_state(cutter);
        break;

    case CUTTER_STATE_MOVING_TO_CENTER:
        update_transition_state(cutter, dt);
        break;

    case CUTTER_STATE_ACTIVE:
        if (should_dismiss_cutter(hat)) {
            cutter->state = CUTTER_STATE_MOVING_TO_DOCK;
            cutter->transition_progress = 0.0f;
        } else {
            update_active_input(cutter);
        }
        break;

    case CUTTER_STATE_MOVING_TO_DOCK:
        update_return_transition_state(cutter, dt);
        break;
    }
}

void hat_cutter_render(void) {
    HatCutter *cutter;
    float offset_y;

    cutter = &hat_making_state.cutter;

    DrawTexture(cutter->body.tex,
                (int)(cutter->pos.x - cutter->body.width / 2.0f),
                (int)(cutter->pos.y - cutter->body.height / 2.0f), WHITE);

    offset_y = cutter->cutter_dist * CUTTER_OFFSET;
    DrawTexture(
        cutter->head.tex, (int)(cutter->pos.x - cutter->head.width / 2.0f),
        (int)(cutter->pos.y - cutter->head.height / 2.0f + offset_y), WHITE);
}

void hat_making_render(void) {
    Hat *hat;

    ClearBackground(WHITE);

    hat = &hat_making_state.hat;
    DrawTexture(hat->asset.tex, (int)(hat->pos.x - hat->asset.width / 2.0f),
                (int)(hat->pos.y - hat->asset.height / 2.0f), WHITE);

    if (hat_making_state.is_cutter_ui_open) {
        hat_cutter_render();
    }
}

void hat_making_exit(void) {
    asset_free(hat_making_state.hat.asset);
    asset_free(hat_making_state.cutter.head);
    asset_free(hat_making_state.cutter.body);
}

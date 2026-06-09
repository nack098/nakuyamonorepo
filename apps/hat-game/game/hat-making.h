#pragma once

#include "config.h"
#include "raylib.h"
#include <stdint.h>

typedef enum {
    CUTTER_STATE_DOCKED,
    CUTTER_STATE_MOVING_TO_CENTER,
    CUTTER_STATE_ACTIVE,
    CUTTER_STATE_MOVING_TO_DOCK
} CutterState;

typedef struct {
    float x, y;
} Position;

typedef struct {
    Image img;
    Texture tex;
    float width, height;
} Asset;

typedef struct {
    Position pos;
    Asset asset;
} Hat;

typedef struct {
    Position pos;
    uint16_t cutter_dist;
    Asset head;
    Asset body;

    CutterState state;
    Position docked_pos;
    Position center_pos;
    float transition_progress;
} HatCutter;

typedef struct {
    Hat hat;
    HatCutter cutter;

    bool is_cutter_ui_open;
} HatMakingState;

extern HatMakingState hat_making_state;

Asset asset_load(const char *path, float width, float height);
void asset_free(Asset asset);

void hat_cutter_input(void);
void hat_cutter_render(void);

void hat_making_init(void);
void hat_making_update(void);
void hat_making_render(void);
void hat_making_exit(void);

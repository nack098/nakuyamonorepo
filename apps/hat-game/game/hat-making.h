#pragma once

#include "config.h"
#include "raylib.h"
#include <stdint.h>

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
} HatCutter;

typedef struct {
    Hat hat;
    HatCutter cutter;
} HatCutterState;

Asset asset_load(const char *path, float width, float height);
void asset_free(Asset asset);

void hat_cutting_init(void);
void hat_cutting_update(void);
void hat_cutting_render(void);
void hat_cutting_exit(void);

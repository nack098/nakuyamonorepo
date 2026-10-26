#pragma once
#include "loader-config.h"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include <windows.h>
#else
#include <dlfcn.h>
#include <unistd.h>
#endif

#include "systems.h"
#include <stdbool.h>

typedef void (*bootloader_function)(State *);

bool platform_copy_file(const char *src, const char *dst);
void platform_delete_file(const char *path);

bool load_game_code(State *state);
void unload_game_code(void);

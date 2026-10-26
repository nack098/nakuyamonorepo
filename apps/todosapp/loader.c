#include "loader.h"
#include "loader-config.h"
#include <nakuyadebugger.h>

typedef void (*bootloader_function)(State *);

static void *game_lib_handle = NULL;
static bootloader_function dynamic_bootloader = NULL;

bool platform_copy_file(const char *src, const char *dst) {
#if defined(_WIN32)
    return CopyFileA(src, dst, FALSE) != 0;
#else
    FILE *fsrc = fopen(src, "rb");
    if (!fsrc)
        return false;
    FILE *fdst = fopen(dst, "wb");
    if (!fdst) {
        fclose(fsrc);
        return false;
    }
    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), fsrc)) > 0) {
        if (fwrite(buffer, 1, bytes, fdst) != bytes) {
            fclose(fsrc);
            fclose(fdst);
            return false;
        }
    }
    fclose(fsrc);
    fclose(fdst);
    return true;
#endif
}

void platform_delete_file(const char *path) {
#if defined(_WIN32)
    DeleteFileA(path);
#else
    unlink(path);
#endif
}

void unload_game_code(void) {
    if (game_lib_handle) {
#if defined(_WIN32)
        FreeLibrary((HMODULE)game_lib_handle);
#else
        dlclose(game_lib_handle);
#endif
        game_lib_handle = NULL;
        dynamic_bootloader = NULL;
        LOG_DEBUG("Unloaded game logic DLL.");
    }
}

bool load_game_code(State *state) {
    unload_game_code();

    if (!platform_copy_file(LIB_ORIGINAL, LIB_TEMP)) {
        LOG_ERROR("Hot-Reload Failed: Cannot create temporary binary library "
                  "instance.");
        return false;
    }

#if defined(_WIN32)
    game_lib_handle = LoadLibraryA(LIB_TEMP);
#else
    game_lib_handle = dlopen("./" LIB_TEMP, RTLD_NOW);
#endif

    if (!game_lib_handle) {
        LOG_ERROR(
            "Hot-Reload Failed: Could not load temporary library instance.");
        return false;
    }

#if defined(_WIN32)
    dynamic_bootloader = (bootloader_function)GetProcAddress(
        (HMODULE)game_lib_handle, "bootloader");
#else
    dynamic_bootloader =
        (bootloader_function)dlsym(game_lib_handle, "bootloader");
#endif

    if (!dynamic_bootloader) {
        LOG_ERROR("Hot-Reload Failed: Could not find 'bootloader' symbol "
                  "exported in DLL.");
        unload_game_code();
        return false;
    }

    LOG_DEBUG("Hot-Reload Successful! Swapped function pointers to new binary "
              "layout.");
    dynamic_bootloader(state);

    return true;
}

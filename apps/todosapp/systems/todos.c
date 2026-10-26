#include <stdint.h>
#include <string.h>

#include "config.h"

#include "array-list.h"
#include "colors/catpuccin-macchiato.h"
#include "raygui.h"
#include "raylib.h"
#include "raymath.h"
#include "string.h"
#include "todos.h"

typedef struct {
    string_t name;
    Rectangle container;
    uint8_t is_done;
    uint8_t is_remove;
} TodoItem;

typedef struct {
    float total_time;
    uint8_t is_working;
    uint8_t is_timer_running;
    uint8_t padding[2];
    TodoItem *todos;
} AppState;

typedef struct {
    uint8_t is_container_open;
    uint8_t is_input_enable;
} AppUIState;

typedef struct {
    Font noto_sans_bold;
    Font noto_sans_medium;
} AssetsData;

static AppState app_state = {0};
static AppUIState app_ui_state = {0};
static AssetsData assets_data = {0};
static string_t buffer = NULL;

static void draw_button(void) {
    uint16_t minutes, hours, seconds;

    minutes = (uint16_t)app_state.total_time / 60;
    seconds = (uint16_t)app_state.total_time % 60;

    hours = minutes / 60;
    minutes %= 60;

    DrawRectangle(0, 0, WIDTH, 30, CATPUCCIN_CRUST);
    if (GuiButton((Rectangle){0, 0, 90, 30}, "Open todos")) {
        app_ui_state.is_container_open =
            (app_ui_state.is_container_open + 1) % 2;
        string_clear(&buffer);
    }

    if (GuiButton((Rectangle){90, 0, 60, 30},
                  app_state.is_timer_running == 0 ? "Start" : "Pause"))
        app_state.is_timer_running = (app_state.is_timer_running + 1) % 2;

    if (GuiButton((Rectangle){150, 0, 60, 30}, "Stop")) {
        app_state.is_timer_running = 0;
        app_state.total_time = 0;
    }

    DrawTextEx(assets_data.noto_sans_bold,
               TextFormat("%02d:%02d:%02d", hours, minutes, (uint16_t)seconds),
               (Vector2){220, 2}, 24, 0,
               app_state.is_timer_running    ? CATPUCCIN_GREEN
               : app_state.total_time == 0.f ? CATPUCCIN_BLUE
                                             : CATPUCCIN_YELLOW);
}

static size_t glyph_size(Font font, string_t str, size_t font_size) {
    float len = 0.0f;
    float scale_factor = (float)font_size / (float)font.baseSize;

    int code_point_byte = 0, code_point, idx, advance;
    const char *ptr = str;

    while (*ptr != '\0') {
        code_point = GetCodepoint(ptr, &code_point_byte);

        if (code_point_byte <= 0)
            break;

        idx = GetGlyphIndex(font, code_point);

        if (idx >= 0 && idx < font.glyphCount) {
            advance = font.glyphs[idx].advanceX;

            len += (float)((advance == 0) ? font.recs[idx].width : advance) *
                   scale_factor;
        }

        ptr += code_point_byte;
    }

    return (size_t)len;
}

static void save_array_as_file(const char *path, TodoItem *arr) {
    uint8_t length = array_list_length(arr);
    uint8_t cnt = 0, cnt2 = 0;
    uint8_t str_len = 0;
    uint8_t buf = 0;
    uint8_t *buffer = array_list_create(0, sizeof(uint8_t));
    array_list_append((void **)&buffer, (void **)&length);
    for (cnt = 0; cnt < length; ++cnt) {
        str_len = string_length(arr[cnt].name);
        buf = (arr[cnt].is_done << 7) | (str_len - 1);
        array_list_append((void **)&buffer, (void **)&buf);
        for (cnt2 = 0; cnt2 < str_len; ++cnt2) {
            array_list_append((void **)&buffer, (void **)&arr[cnt].name[cnt2]);
        }
    }
    SaveFileData(path, buffer, array_list_length(buffer));
}

static void read_array_from_file(const char *path, TodoItem **arr) {
    int data_size = 0;
    uint8_t *data, list_amount, cnt, cnt2, name_length, *read_head;
    TodoItem current = {0};
    string_t name = NULL;

    data = LoadFileData(path, &data_size);
    if (data == NULL)
        return;
    read_head = data;
    list_amount = *read_head;

    for (cnt = 0; cnt < list_amount; ++cnt) {
        ++read_head;
        current.is_done = (uint8_t)(*read_head) >> 7;
        name_length = ((uint8_t)(*read_head) ^ (current.is_done << 7)) + 1;
        name = string_create(name_length);
        for (cnt2 = 0; cnt2 < name_length; ++cnt2) {
            ++read_head;
            string_append_char(&name, (char)*read_head);
        }
        current.container = (Rectangle){
            .x = 16,
            .y = 40 + (array_list_length(*arr)) * 24,
            .height = 24,
            .width = (float)glyph_size(assets_data.noto_sans_medium, name, 24)};
        current.is_remove = 0;
        current.name = name;
        array_list_append((void **)arr, (void **)&current);
    }

    UnloadFileData(data);
}

static void draw_text_input(void) {
    static float bar_counter = 0;
    static float remove_timer = 0;
    static uint8_t remove_state = 0;
    static uint8_t remove_enable = 0;
    int current = 0;
    TodoItem item = {0};
    string_data_t *data = NULL;
    Rectangle write_box = {
        .x = 0,
        .y = HEIGHT - 30,
        .width = WIDTH - 60,
        .height = 30,
    };
    Vector2 pos = {0};

    // DrawRectangle((int)write_box.x, (int)write_box.y, (int)write_box.width,
    //               (int)write_box.height, DARKBROWN);

    if (CheckCollisionPointRec(GetMousePosition(), write_box)) {
        app_ui_state.is_input_enable = 1;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        pos = GetMousePosition();
        if (pos.x < write_box.x || pos.y < write_box.y) {
            app_ui_state.is_input_enable = 0;
        }
    }

    if (app_ui_state.is_input_enable) {
        do {
            current = GetCharPressed();
            if (current != 0) {
                if (string_length(buffer) < 16)
                    string_append_char(&buffer, (char)current);
            }
        } while (current != 0);

        if (remove_state == 0) {
            if (IsKeyPressed(KEY_BACKSPACE) && string_length(buffer) > 0) {
                data = string_get_data(buffer);
                data->length -= 1;
                data->data[data->length] = '\0';
                remove_state = 1;
                remove_timer = 0;
            }
        } else if (remove_state == 1) {
            if (IsKeyDown(KEY_BACKSPACE)) {
                if (remove_timer >= 0.5f) {
                    remove_timer = 0;
                    remove_state = 2;
                    remove_enable = 1;
                } else {
                    remove_timer += GetFrameTime();
                }
            } else {
                remove_state = 0;
                remove_timer = 0;
            }
        } else if (remove_state == 2) {
            if (!IsKeyDown(KEY_BACKSPACE)) {
                remove_state = 0;
                remove_enable = 0;
                remove_timer = 0;
            } else if (IsKeyDown(KEY_BACKSPACE) && string_length(buffer) > 0) {
                if (remove_enable) {
                    data = string_get_data(buffer);
                    data->length -= 1;
                    data->data[data->length] = '\0';
                    remove_timer = 0;
                    remove_enable = 0;
                } else {
                    if (remove_timer >= 0.020) {
                        remove_enable = 1;
                        remove_timer = 0;
                    } else {
                        remove_timer += GetFrameTime();
                    }
                }
            } else {
                remove_state = 0;
                remove_enable = 0;
                remove_timer = 0;
            }
        }
    }

    DrawTextEx(assets_data.noto_sans_medium, buffer, (Vector2){12, HEIGHT - 32},
               24, 0, CATPUCCIN_TEXT);
    if (app_ui_state.is_input_enable) {

        bar_counter += GetFrameTime();

        if (bar_counter >= 1.5) {
            bar_counter = 0;
        } else if (bar_counter < 0.75) {
            DrawRectangle(
                12 + glyph_size(assets_data.noto_sans_medium, buffer, 24),
                HEIGHT - 28, 2, 18, CATPUCCIN_TEXT);
        }
    }

    // if ((GuiButton((Rectangle){WIDTH - 60, HEIGHT - 30, 60, 30}, "Submit") ||
    //      IsKeyPressed(KEY_ENTER)) &&
    //     string_length(buffer) > 0) {
    //
    if (IsKeyPressed(KEY_ENTER) && string_length(buffer) > 0) {
        app_ui_state.is_input_enable = 0;

        item = (TodoItem){
            .is_done = 0,
            .container = {.x = 16,
                          .y = 40 + (array_list_length(app_state.todos)) * 24,
                          .height = 24,
                          .width = (float)glyph_size(
                              assets_data.noto_sans_medium, buffer, 24)},
            .name = string_from_cstr(buffer)};

        array_list_append((void **)&app_state.todos, (void **)&item);
        save_array_as_file("data.dat", app_state.todos);
        string_clear(&buffer);
    }
}

static void draw_todos(void) {
    size_t cnt;
    uint8_t is_hover = 0;

    // DrawRectangle(0, 30, WIDTH, 4, LIME);
    for (cnt = 0; cnt < array_list_length(app_state.todos); ++cnt) {
        if (CheckCollisionPointRec(GetMousePosition(),
                                   app_state.todos[cnt].container)) {
            is_hover = 1;
            DrawTextEx(assets_data.noto_sans_medium, app_state.todos[cnt].name,
                       (Vector2){app_state.todos[cnt].container.x,
                                 app_state.todos[cnt].container.y},
                       app_state.todos[cnt].container.height, 0,
                       CATPUCCIN_MAROON);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                app_state.todos[cnt].is_done =
                    (app_state.todos[cnt].is_done + 1) % 2;
                save_array_as_file("data.dat", app_state.todos);
            }
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
                app_state.todos[cnt].is_remove = 1;
        } else {
            DrawTextEx(assets_data.noto_sans_medium, app_state.todos[cnt].name,
                       (Vector2){app_state.todos[cnt].container.x,
                                 app_state.todos[cnt].container.y},
                       app_state.todos[cnt].container.height, 0,
                       CATPUCCIN_BLUE);
            is_hover = 0;
        }

        if (app_state.todos[cnt].is_done)
            DrawRectangle(0,
                          (int)(app_state.todos[cnt].container.y +
                                app_state.todos[cnt].container.height / 2),
                          WIDTH, 2,
                          is_hover ? CATPUCCIN_MAROON : CATPUCCIN_BLUE);
    }
}

static void draw_container(void) {
    draw_todos();
    draw_text_input();
}

static Font load_ttf(const char *path) {
    Font font = LoadFontEx(path, 128, NULL, 0);
    GenTextureMipmaps(&font.texture);
    SetTextureWrap(font.texture, TEXTURE_WRAP_CLAMP);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    return font;
}

void todos_init(void) {
    buffer = string_create(16);
    app_state.todos = (TodoItem *)array_list_create(12, sizeof(TodoItem));

    assets_data.noto_sans_medium =
        load_ttf("assets/fonts/NotoSansJP/Medium.ttf");
    assets_data.noto_sans_bold = load_ttf("assets/fonts/NotoSansJP/Bold.ttf");
    read_array_from_file("data.dat", &app_state.todos);

    GuiSetFont(assets_data.noto_sans_bold);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 255);
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, 255);
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, 255);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 14);
    SetWindowPosition(GetMonitorWidth(GetCurrentMonitor()) - WIDTH, 0);
}

void todos_update(void) {
    static uint8_t drag_state = 0;
    static Vector2 old_mouse_pos = {0};
    Vector2 current_mouse_pos = {0}, current_window_pos = {0};
    size_t cnt, len = array_list_length(app_state.todos);
    uint8_t is_change = 0;
    TodoItem *temp = array_list_create(len, sizeof(TodoItem));
    TodoItem current = {0};

    for (cnt = 0; cnt < len; ++cnt) {
        if (app_state.todos[cnt].is_remove) {
            string_free(&app_state.todos[cnt].name);
            is_change = 1;
        } else {
            current = app_state.todos[cnt];
            current.container = (Rectangle){
                .x = 16,
                .y = 40 + (array_list_length(temp)) * 24,
                .height = 24,
                .width = (float)glyph_size(assets_data.noto_sans_medium,
                                           app_state.todos[cnt].name, 24)};
            array_list_append((void **)&temp, (void **)&current);
        }
    }
    array_list_free((void **)&app_state.todos);
    app_state.todos = temp;
    if (is_change) {
        save_array_as_file("data.dat", temp);
    }

    if (app_state.is_timer_running == 1)
        app_state.total_time += GetFrameTime();

    if (IsKeyDown(KEY_LEFT_CONTROL)) {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !drag_state) {
            old_mouse_pos = GetMousePosition();
            drag_state = 1;
        } else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && drag_state) {
            current_mouse_pos = GetMousePosition();
            current_window_pos = GetWindowPosition();
            SetWindowPosition(Lerp(current_window_pos.x,
                                   current_window_pos.x + current_mouse_pos.x -
                                       old_mouse_pos.x,
                                   GetFrameTime() * 10),
                              Lerp(current_window_pos.y,
                                   current_window_pos.y + current_mouse_pos.y -
                                       old_mouse_pos.y,
                                   GetFrameTime() * 10));
        } else {
            drag_state = 0;
        }
    }
}

void todos_render(void) {
    ClearBackground(BLANK);
    if (app_ui_state.is_container_open) {
        SetWindowSize(WIDTH, HEIGHT);
        DrawRectangle(0, 0, WIDTH, HEIGHT, (Color){24, 25, 38, 0.75 * 255});
        draw_container();
    } else {
        SetWindowSize(WIDTH, 30);
    }
    draw_button();
}

void todos_exit(void) {
    size_t cnt;
    UnloadFont(assets_data.noto_sans_bold);
    UnloadFont(assets_data.noto_sans_medium);
    for (cnt = 0; cnt < array_list_length(app_state.todos); ++cnt)
        string_free(&app_state.todos[cnt].name);
    array_list_free((void **)&app_state.todos);
}

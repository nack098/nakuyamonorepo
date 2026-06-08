#include "nakuyadebugger/logger.h"
#include <assert.h>
#include <nakuyadebugger.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX_UI_LOGS 12
#define MAX_LOG_LINE_LEN 256

static char ui_log_buffer[MAX_UI_LOGS][MAX_LOG_LINE_LEN];
static int ui_log_count = 0;

static const char *get_filename_only(const char *path) {
    const char *last_slash = path;
    if (!path)
        return "unknown";

    while (*path) {
        if (*path == '/' || *path == '\\') {
            last_slash = path + 1;
        }
        path++;
    }

    return last_slash;
}

int logger_get_ui_count(void) { return ui_log_count; }

const char *logger_get_ui_line(int index) {
    if (index >= 0 && index < ui_log_count)
        return ui_log_buffer[index];
    return "";
}

static void add_to_ui_buffer(const char *line) {
    if (ui_log_count < MAX_UI_LOGS) {
        strncpy(ui_log_buffer[ui_log_count], line, MAX_LOG_LINE_LEN - 1);
        ui_log_buffer[ui_log_count][MAX_LOG_LINE_LEN - 1] = '\0';
        ui_log_count++;
    } else {
        for (int i = 1; i < MAX_UI_LOGS; i++) {
            strcpy(ui_log_buffer[i - 1], ui_log_buffer[i]);
        }
        strncpy(ui_log_buffer[MAX_UI_LOGS - 1], line, MAX_LOG_LINE_LEN - 1);
        ui_log_buffer[MAX_UI_LOGS - 1][MAX_LOG_LINE_LEN - 1] = '\0';
    }
}

RLAPI void logger(E_LOG level, const char *name, const char *file_name,
                  const char *func, const char *format, ...) {
    const char *level_str = "UNKN ";
    time_t raw_time;
    struct tm time_info;
    char time_str[9];
    const char *clean_file;
    va_list args;
    char message_buffer[LOG_MAX_LENGTH];
    char final_ui_line[MAX_LOG_LINE_LEN];

    switch ((int)level) {
    case NAKULOG_DEBUG:
        level_str = "DEBUG";
        break;
    case NAKULOG_ERROR:
        level_str = "ERROR";
        break;
    case NAKULOG_WARNING:
        level_str = "WARN ";
        break;
    default:
        level_str = "UNKN ";
        break;
    }

    va_start(args, format);
#ifdef _MSC_VER
    vsnprintf_s(message_buffer, sizeof(message_buffer), _TRUNCATE, format,
                args);
#else
    vsnprintf(message_buffer, sizeof(message_buffer), format, args);
#endif
    va_end(args);

    time(&raw_time);
#if defined(_WIN32)
    localtime_s(&time_info, &raw_time);
#else
    localtime_r(&raw_time, &time_info);
#endif
    strftime(time_str, sizeof(time_str), "%H:%M:%S", &time_info);

    clean_file = get_filename_only(file_name);

    printf("[ %s ] [ %s ] [ %s ] [ %s::%s ] %s\n", time_str, name, level_str,
           clean_file, func, message_buffer);

    snprintf(final_ui_line, sizeof(final_ui_line), "[%s] [%s] %s", time_str,
             level_str, message_buffer);

    add_to_ui_buffer(final_ui_line);
}

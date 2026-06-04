#include "nakuyadebugger/logger.h"
#include <assert.h>
#include <nakuyadebugger.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

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

RLAPI void logger(E_LOG level, const char *name, const char *file_name,
                  const char *func, const char *format, ...) {
    const char *level_str = "UNKN ";
    time_t raw_time;
    struct tm time_info;
    char time_str[9];
    const char *clean_file;

    va_list args;

    char message_buffer[LOG_MAX_LENGTH];

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
    vsnprintf_s(message_buffer, sizeof(message_buffer), LOG_MAX_LENGTH, format,
                args);
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
}

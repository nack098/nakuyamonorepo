#pragma once

#include <raylib.h>

#define USE_LIBTYPE_SHARED

#ifndef LOG_MAX_LENGTH
#define LOG_MAX_LENGTH 1024
#endif

typedef enum log_e {
    NAKULOG_DEBUG,
    NAKULOG_ERROR,
    NAKULOG_WARNING,
} E_LOG;

RLAPI void logger(E_LOG level, const char *name, const char *file_name,
                  const char *func, const char *format, ...);

// helper macro

#ifndef NAME
#define NAME "UNKNOWN_PROJECT"
#endif

#define LOG_DEBUG(fmt, ...)                                                    \
    logger(NAKULOG_DEBUG, NAME, __FILE__, __FUNCTION__,                        \
           fmt __VA_OPT__(, ) __VA_ARGS__)

#define LOG_ERROR(fmt, ...)                                                    \
    logger(NAKULOG_ERROR, NAME, __FILE__, __FUNCTION__,                        \
           fmt __VA_OPT__(, ) __VA_ARGS__)

#define LOG_WARN(fmt, ...)                                                     \
    logger(NAKULOG_WARNING, NAME, __FILE__, __FUNCTION__,                      \
           fmt __VA_OPT__(, ) __VA_ARGS__)

#pragma once

// clang-format off
#define LIB_NAME todos-app-core
// clang-format on

#define LIB_STR_IMPL(x) #x
#define LIB_STR(x) LIB_STR_IMPL(x)
#if defined(_WIN32)
#define LIB_ORIGINAL (LIB_STR(LIB_NAME) ".dll")
#define LIB_TEMP (LIB_STR(LIB_NAME) "_tmp.dll")
#else
#if defined(__APPLE__)
#define LIB_ORIGINAL ("lib" LIB_STR(LIB_NAME) ".dylib")
#define LIB_TEMP ("lib" LIB_STR(LIB_NAME) "_tmp.dylib")
#else
#define LIB_ORIGINAL ("lib" LIB_STR(LIB_NAME) ".so")
#define LIB_TEMP ("lib" LIB_STR(LIB_NAME) "_tmp.so")
#endif
#endif

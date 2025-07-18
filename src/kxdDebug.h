#pragma once
#ifndef KXD_DEBUG_H
#define KXD_DEBUG_H

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEX8  "0x%02" PRIX8
#define HEX16 "0x%04" PRIX16

#define XSTR(A) #A
#define STR(A)  XSTR(A)

#define BIT8 "%d%d%d%d%d%d%d%d"

#define BITN(v, N) ((bool)(v & (1 << N)))

#define RECT_CFMT(fmt)  "(" fmt ", " fmt ", " fmt ", " fmt ")"
#define RECT_FMT        RECT_CFMT("%.2f")
#define RECT_ARGS(rect) rect.x, rect.y, rect.width, rect.height

#define V2_CFMT(fmt) "(" fmt ", " fmt ")"
#define V2_FMT       V2_CFMT("%.2f")
#define V2_ARGS(v2)  v2.x, v2.y

#ifdef KXD_DEBUG

#if defined(__GNUC__) || defined(__clang__)
// https://gcc.gnu.org/onlinedocs/gcc-4.7.2/gcc/Function-Attributes.html
#ifdef __MINGW_PRINTF_FORMAT
#define KXD_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK)                                                                \
    __attribute__((format(__MINGW_PRINTF_FORMAT, STRING_INDEX, FIRST_TO_CHECK)))
#else
#define KXD_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) __attribute__((format(printf, STRING_INDEX, FIRST_TO_CHECK)))
#endif
#else
// TODO: implement KXD_PRINTF_FORMAT for MSVC
#define KXD_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK)
#endif

typedef enum {
    LOG_SA_ALL = 0, // Display all logs
    LOG_SA_TRACE,   // Trace logging, intended for internal use only
    LOG_SA_DEBUG,   // Debug logging, used for internal debugging, it should be disabled on release
                    // builds
    LOG_SA_INFO,    // Info logging, used for program execution info
    LOG_SA_WARNING, // Warning logging, used on recoverable failures
    LOG_SA_ERROR,   // Error logging, used on unrecoverable failures
    LOG_SA_FATAL,   // Fatal logging, used to abort program: exit(EXIT_FAILURE)
    LOG_SA_NONE     // Disable logging
} SA_TraceLogLevel;

void TraceLog(int logType, const char *text, ...) KXD_PRINTF_FORMAT(2, 3);

#define LOG_INF(...)   TraceLog(LOG_SA_INFO, __VA_ARGS__)
#define LOG_ERR(...)   TraceLog(LOG_SA_ERROR, __VA_ARGS__)
#define VARLOG(v, fmt) TraceLog(LOG_SA_INFO, "%s: " fmt, (#v), (v))
#define BOOLLOG(v)     TraceLog(LOG_SA_INFO, "%s: %s", #v, v ? "True" : "False")

#define KXD_TODO(message)                                                                                              \
    do {                                                                                                               \
        fprintf(stderr, "[TODO] %s:%d: %s\n", __FILE__, __LINE__, message);                                            \
        fflush(stdout);                                                                                                \
        fflush(stderr);                                                                                                \
        abort();                                                                                                       \
    } while (0)

// clang-format off
#define BITFIELD8LOG(v)                                                         \
        TraceLog(                                                               \
        LOG_SA_INFO,                                                            \
        "%s: " BIT8,                                                            \
        #v,                                                                     \
        (bool)BITN(v, 7),                                                       \
        (bool)BITN(v, 6),                                                       \
        (bool)BITN(v, 5),                                                       \
        (bool)BITN(v, 4),                                                       \
        (bool)BITN(v, 3),                                                       \
        (bool)BITN(v, 2),                                                       \
        (bool)BITN(v, 1),                                                       \
        (bool)BITN(v, 0))
// clang-format on

#else

#define LOG_INF(...)    ;
#define LOG_ERR(...)    ;
#define VARLOG(v, fmt)  ;
#define BOOLLOG(v)      ;
#define BITFIELD8LOG(v) ;
#define KXD_TODO(m)     ;

#endif /* KXD_DEBUG */

#ifndef KXD_ARRAY_LEN
#define KXD_ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))
#endif /* KXD_ARRAY_LEN */

#endif /* KXD_DEBUG_H */

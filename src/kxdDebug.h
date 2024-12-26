#ifndef KXD_DEBUG_H
#define KXD_DEBUG_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEX8 "0x%02X"
#define HEX16 "0x%04X"

#define XSTR(A) #A
#define STR(A) XSTR(A)

#define BIT8 "%d%d%d%d%d%d%d%d"

#define BITN(v, N) ((bool)(v & (1 << N)))

#define RECT_CFMT(fmt) "(" fmt ", " fmt ", " fmt ", " fmt ")"
#define RECT_FMT RECT_CFMT("%.2f")
#define RECT_ARGS(rect) rect.x, rect.y, rect.width, rect.height

#define V2_CFMT(fmt) "(" fmt ", " fmt ")"
#define V2_FMT V2_CFMT("%.2f")
#define V2_ARGS(v2) v2.x, v2.y

#ifdef KXD_DEBUG

#define LOG_INF(...) TraceLog(LOG_INFO, __VA_ARGS__)
#define LOG_ERR(...) TraceLog(LOG_ERROR, __VA_ARGS__)
#define VARLOG(v, fmt) TraceLog(LOG_INFO, "%s: " fmt, (#v), (v))
#define BOOLLOG(v) TraceLog(LOG_INFO, "%s: %s", #v, v ? "True" : "False")

#define KXD_TODO(message) do { fprintf(stderr, "%s:%d: TODO: %s\n", __FILE__, __LINE__, message); abort(); } while (0)

// clang-format off
#define BITFIELD8LOG(v)                                                         \
        TraceLog(                                                               \
        LOG_INFO,                                                               \
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

// clang-format off

#define LOG_INF(...)    ;
#define LOG_ERR(...)    ;
#define VARLOG(v, fmt)  ;
#define BOOLLOG(v)      ;
#define BITFIELD8LOG(v) ;
#define KXD_TODO(m)     ;
// clang-format on
#endif /* KXD_DEBUG */

#ifndef KXD_ARRAY_LEN
#define KXD_ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))
#endif /* KXD_ARRAY_LEN */

#endif /* KXD_DEBUG_H */

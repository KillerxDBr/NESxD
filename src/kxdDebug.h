#ifndef KXD_DEBUG_H
#define KXD_DEBUG_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

#define HEX8  "0x%02X"
#define HEX16 "0x%04X"

#define XSTR(A) #A
#define STR(A) XSTR(A)

#define BIT8 "%d%d%d%d%d%d%d%d"

#define BITN(v, N) (v & (1 << N))

#define LOG_INF(...)   TraceLog(LOG_INFO, __VA_ARGS__)
#define LOG_ERR(...)   TraceLog(LOG_ERROR, __VA_ARGS__)
#define VARLOG(v, fmt) TraceLog(LOG_INFO, "%s: " fmt, (#v), (v))
#define BOOLLOG(v)     TraceLog(LOG_INFO, "%s: %s", #v, v ? "True" : "False")

#define BITFIELD8LOG(v)                                                                                                                \
    TraceLog(LOG_INFO, "%s: " BIT8, #v, BITN(v, 7), BITN(v, 6), BITN(v, 5), BITN(v, 4), BITN(v, 3), BITN(v, 2), BITN(v, 1), BITN(v, 0))

static inline void *callocWrapper(size_t n, size_t sz) {
    void *ptr = calloc(n, sz);
    if (ptr == NULL) {
        fprintf(stderr, "\nCould not allocate memory (%zu bytes) in \'%s:%d\', exiting...\n\n", n * sz, __FILE__, __LINE__);
        exit(1);
    }
    return ptr;
}

#endif /* KXD_DEBUG_H */

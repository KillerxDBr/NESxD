#ifndef KXD_DEBUG_H
#define KXD_DEBUG_H

#include "raylib.h"

#define LOG_INF(...)   TraceLog(LOG_INFO,  __VA_ARGS__)
#define LOG_ERR(...)   TraceLog(LOG_ERROR, __VA_ARGS__)
#define VARLOG(v, fmt) TraceLog(LOG_INFO,  "%s: " fmt, #v, v)

#define HEX8  "0x%02X"
#define HEX16 "0x%04X"

#endif /* KXD_DEBUG_H */

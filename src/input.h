#pragma once
#ifndef KXD_INPUT_H
#define KXD_INPUT_H

#include "kxdDebug.h"
#include "kxdTypes.h"

#include "raylib.h"

// #define RAYMATH_STATIC_INLINE
// #include "raymath.h"

#define BUTTON_A      (1 << 7)
#define BUTTON_B      (1 << 6)
#define BUTTON_SELECT (1 << 5)
#define BUTTON_START  (1 << 4)
#define BUTTON_UP     (1 << 3)
#define BUTTON_DOWN   (1 << 2)
#define BUTTON_LEFT   (1 << 1)
#define BUTTON_RIGHT  (1 << 0)

void registerInput(nes_t *nes);

#endif /* KXD_INPUT_H */

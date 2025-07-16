#pragma once
#ifndef KXD_TYPES_H
#define KXD_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#include "raylib.h"

// resolution 256 x 224
#define NES_W  256
#define NES_H  224
#define FACTOR 2

#define NES_AR ((float)NES_W / (float)NES_H)

#define NOP_CMD  "-NOP"
#define TEST_CMD "-T"

#define MENU_BAR_SIZE 20

#define MB(n) ((size_t)(n * 1048576ULL))
#define KB(n) ((size_t)(n * 1024ULL))
// #define MEMSIZE (KB(2))
#define MEMSIZE (KB(64))

typedef struct u8Vec2 {
    uint8_t x, y;
} u8Vec2;

typedef struct i8Vec2 {
    int8_t x, y;
} i8Vec2;

typedef struct u16Vec2 {
    uint16_t x, y;
} u16Vec2;

typedef struct i16Vec2 {
    int16_t x, y;
} i16Vec2;

typedef struct u32Vec2 {
    uint32_t x, y;
} u32Vec2;

typedef struct i32Vec2 {
    int32_t x, y;
} i32Vec2;

typedef struct f32Vec2 {
    float x, y;
} f32Vec2;

typedef struct f64Vec2 {
    double x, y;
} f64Vec2;

#ifndef V2
#define V2(x, y) (CLITERAL(Vector2){(float)(x), (float)(y)})
#endif /* V2 */

#ifndef f32V2
#define f32V2(x, y) (CLITERAL(f32Vec2){(float)(x), (float)(y)})
#endif /* f32V2 */

#ifndef I32V2
#define I32V2(x, y) (CLITERAL(i32Vec2){(int32_t)(x), (int32_t)(y)})
#endif /* I32V2 */

#ifndef U32V2
#define U32V2(x, y) (CLITERAL(u32Vec2){(uint32_t)(x), (uint32_t)(y)})
#endif /* U32V2 */

#ifndef f64V2
#define f64V2(x, y) (CLITERAL(f64Vec2){(double)(x), (double)(y)})
#endif /* f64V2 */

typedef struct {
    uint16_t ButtonUp;
    uint16_t ButtonDown;
    uint16_t ButtonLeft;
    uint16_t ButtonRight;

    uint16_t ButtonA;
    uint16_t ButtonB;

    uint16_t ButtonStart;
    uint16_t ButtonSelect;
} controller_t;

typedef struct {
    uint16_t PC;
    uint8_t SP;

    uint8_t A;
    uint8_t X;
    uint8_t Y;

    // Processor Status
    bool C;
    bool Z;
    bool I;
    bool D;
    bool B;
    bool V;
    bool N;

    // RAM Memory
    uint8_t mem[MEMSIZE];
} cpu_t;

#ifndef NOPPU
typedef struct {
    uint8_t mem[MEMSIZE];
} ppu_t;
#endif /* NOPPU */

typedef struct {
    cpu_t cpu;

#ifndef NOPPU
    ppu_t ppu;
#endif /* NOPPU */

    controller_t controller; // controller Struct

    uint8_t *rom;
    size_t romSize;
    bool nes2format;

    uint8_t *PRG;
    size_t PRGSize;

    uint8_t *CHR;
    size_t CHRSize;

    bool isPaused;
} nes_t;

typedef struct {
    const char *base;

    char code[2];
    char country[2];

    const char *name;
    const char *author;
    const char *text_paused;
    const char *menu_file;
    const char *menu_file_open;
    const char *menu_file_quit;
} lang_t;

typedef struct {
    const char *fileName;
    bool hasConfig;

    int activeTheme;

    uint16_t fastForwardKey;
    uint16_t pauseKey;
    const char *lang;
} config_t;

typedef struct {
    bool openMenu;
    bool openFile;
} menu_t;

typedef struct {
    // Screen
    uint16_t screenW, screenH;
    Rectangle sourceRec, destRec;
    RenderTexture2D screen;

    // NES
    nes_t nes;
    config_t config;
    menu_t menu;

    // MISC
    const char *program;
    lang_t lang;
    bool quit;
} app_t;

#endif /* KXD_TYPES_H */

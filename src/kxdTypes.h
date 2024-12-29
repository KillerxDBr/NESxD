#pragma once
#ifndef KXD_TYPES_H
#define KXD_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#include "raylib.h"

// resolution 256 x 224
#define NES_W 256
#define NES_H 224
#define FACTOR 2

#define NES_AR ((float)NES_W / (float)NES_H)

#define NOP_CMD "-NOP"
#define TEST_CMD "-T"

#define MENU_BAR_SIZE 20

#define MB(n) (n * 1048576)
#define KB(n) (n * 1024)
// #define MEMSIZE (KB(2))
#define MEMSIZE (KB(64))

typedef struct IVector2 {
    int x, y;
} IVector2;

typedef struct DVector2 {
    double x, y;
} DVector2;

#ifndef V2
#define V2(x, y) (CLITERAL(Vector2){ (float)(x), (float)(y) })
#endif /* V2 */

#ifndef IV2
#define IV2(x, y) (CLITERAL(IVector2){ (int)(x), (int)(y) })
#endif /* IV2 */

#ifndef DV2
#define DV2(x, y) (CLITERAL(DVector2){ (double)(x), (double)(y) })
#endif /* IV2 */

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
    char *fileName;
    bool hasConfig;

    int activeTheme;

    uint16_t fastForwardKey;
    uint16_t pauseKey;
} config_t;

typedef struct {
    bool openMenu;
    bool openFile;
} menu_t;

typedef struct {
    // Screen
    size_t screenW, screenH;
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

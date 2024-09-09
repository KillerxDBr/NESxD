#ifndef KXD_MAIN_H
#define KXD_MAIN_H

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

#include "InsFlags.h"
#include "config.h"
#include "kxdDebug.h"
#include "kxdTypes.h"

#define RAYMATH_STATIC_INLINE
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "../include/raygui.h"

#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include "gui_window_file_dialog.h"

#include "bundle.h"

// #define KXD_DEBUG
#define NOPPU
// #define NOVID

// resolution 256 x 224
#define NES_W 256
#define NES_H 224

#define NES_AR (float)NES_W / (float)NES_H

#define NOP_CMD "-NOP"

#define KXD_BG                                                                                                                             \
    CLITERAL(Color) { 0x38, 0x38, 0x38, 0xFF }

#define V2(x, y)                                                                                                                           \
    CLITERAL(Vector2) { (float)x, (float)y }

// index = y * W + x
#define XY2Index(x, y, w) ((y * w) + x)

#define CHECK_ROM_HEADER(rom) assert(rom[0] == 'N' && rom[1] == 'E' && rom[2] == 'S' && rom[3] == 0x1A)

// void *callocWrapper(size_t n, size_t sz);
void processInstruction(cpu_t *cpu);
void memDmp(cpu_t *cpu, size_t memSize);
void addToMem(uint8_t *mem, size_t loc, uint64_t value);
void addMultipleToMem(uint8_t *mem, size_t loc, uint8_t *values, size_t valuesSize);
void loadRom(nes_t *nes, const char *fileName);
void loadRomFromMem(nes_t *nes, const char *fileName);
void unloadRom(nes_t *nes);
void processRomHeader(nes_t *nes);
void debugCPU(cpu_t *cpu);

#endif /* KXD_MAIN_H */

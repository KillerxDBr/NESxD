#ifndef KXD_MAIN_H
#define KXD_MAIN_H

#include <assert.h>
#include <errno.h>
#include <locale.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif /* PLATFORM_WEB */

#include "raylib.h"

#define RAYMATH_STATIC_INLINE
#include "raymath.h"

#ifndef PLATFORM_WEB
#include "WindowsHeader/WindowsHeader.h"
#include "tinyfiledialogs/tinyfiledialogs.h"
#endif /* PLATFORM_WEB */

#include "InsFlags.h"
#include "config.h"
#include "gui.h"
#include "input.h"
#include "kxdDebug.h"
#include "kxdTypes.h"

#ifdef KXD_DEBUG
#include "test.h"
#endif

#include "bundle.h"

#ifndef CP_UTF8
#define CP_UTF8 65001
#endif /* CP_UTF8 */

// #define KXD_DEBUG
#define NOPPU
// #define NOVID

// resolution 256 x 224
#define NES_W 256
#define NES_H 224
#define FACTOR 2

#define NES_AR ((float)NES_W / (float)NES_H)

#define NOP_CMD "-NOP"
#define TEST_CMD "-T"

#define KXD_BG (CLITERAL(Color){ 0x38, 0x38, 0x38, 0xFF })

// index = y * W + x
#define XY2Index(x, y, w) ((y * w) + x)

#define CHECK_ROM_HEADER(rom) assert(rom[0] == 'N' && rom[1] == 'E' && rom[2] == 'S' && rom[3] == 0x1A)

void processInstruction(cpu_t *cpu);
void memDmp(cpu_t *cpu, size_t memSize);
void addToMem(uint8_t *mem, size_t loc, uint64_t value);
void addMultipleToMem(uint8_t *mem, size_t loc, uint8_t *values, size_t valuesSize);
void loadRom(nes_t *nes, const char *fileName);
void loadRomFromMem(nes_t *nes, const char *fileName);
void unloadRom(nes_t *nes);
void processRomHeader(nes_t *nes);
void debugCPU(cpu_t *cpu);

#ifdef PLATFORM_WEB
void mainLoop(void *app_ptr);
#else
static inline void mainLoop(void *app_ptr);
#endif

#endif /* KXD_MAIN_H */

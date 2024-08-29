#ifndef KXD_MAIN_H
#define KXD_MAIN_H

#include "InsFlags.h"
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <raylib.h>

#include "../bundle.h"

// #define KXD_DEBUG

// resolution 256 x 224
#define PX_SZ 2
#define NESW 256
#define NESH 224

#define KXDBG                                                                                                                              \
    CLITERAL(Color) { 0x38, 0x38, 0x38, 0xFF }

#define LOG_INFO(...) TraceLog(LOG_INFO, __VA_ARGS__)
#define LOG_ERR(...) TraceLog(LOG_ERROR, __VA_ARGS__)
#define VARLOG(v, fmt) TraceLog(LOG_INFO, "%s: " fmt, #v, v)

// index = y * N + x
#define XY2Index(x, y, w) ((y * w) + x)

#define MEMSIZE 2048ULL

#define CHECK_ROM_HEADER(rom) assert(rom[0] == 'N' && rom[1] == 'E' && rom[2] == 'S')

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

typedef struct {
    uint8_t mem[MEMSIZE];
} ppu_t;

typedef struct {
    cpu_t cpu;
    ppu_t ppu;
    uint8_t *rom;
    size_t romSize;
} nes_t;

typedef struct {
    size_t screenW, screenH;
    nes_t nes;
    bool quit;
} app_t;

void *callocWrapper(size_t n, size_t sz);
void processInstruction(cpu_t *cpu);
void memDmp(cpu_t *cpu, size_t memSize);
void addToMem(uint8_t *mem, size_t loc, uint64_t value);
void addMultipleToMem(uint8_t *mem, size_t loc, uint8_t *values, size_t valuesSize);
void loadRom(nes_t *nes, const char *fileName);
void loadRomFromMem(nes_t *nes, const char *fileName);
#ifdef KXD_DEBUG
void debugCPU(cpu_t *cpu);
#endif /* KXD_DEBUG */

#endif /* KXD_MAIN_H */

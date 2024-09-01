#ifndef KXD_MAIN_H
#define KXD_MAIN_H

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "InsFlags.h"
#include "kxdTypes.h"

#include "raylib.h"

#include "bundle.h"

// #define KXD_DEBUG
#define NOPPU
#define NOVID

// resolution 256 x 224
#define PX_SZ 2
#define NESW 256
#define NESH 224

#define KXDBG                                                                                                                              \
    CLITERAL(Color) { 0x38, 0x38, 0x38, 0xFF }

#include "kxdDebug.h"

// #define BIN8 "0b%08B"
// #define BIN16 "0b%016B"

// index = y * N + x
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

static inline void *callocWrapper(size_t n, size_t sz) {
    void *ptr = calloc(n, sz);
    if (ptr == NULL) {
        fprintf(stderr, "\nCould not allocate memory (%zu bytes) in \'%s:%d\', exiting...\n\n", n * sz, __FILE__, __LINE__);
        exit(1);
    }
    return ptr;
}

#endif /* KXD_MAIN_H */

#ifndef KXD_TYPES_H
#define KXD_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#define MEMSIZE 2048ULL

typedef struct {
    uint16_t ButtonUp;
    uint16_t ButtonDown;
    uint16_t ButtonLeft;
    uint16_t ButtonRight;

    uint16_t ButtonA;
    uint16_t ButtonB;

    uint16_t ButtonStart;
    uint16_t ButtonSelect;

    // uint16_t ButtonL;
    // uint16_t ButtonR;

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

typedef struct {
    uint8_t mem[MEMSIZE];
} ppu_t;

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
} nes_t;

typedef struct {
    char *fileName;
    bool hasConfig;
} config_t;

typedef struct {
    bool openFile;
} menu_t;

typedef struct {
    size_t screenW, screenH;
    nes_t nes;
    config_t config;
    bool quit;
    menu_t menu;
} app_t;

#endif /* KXD_TYPES_H */

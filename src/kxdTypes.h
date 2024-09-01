#ifndef KXD_TYPES_H
#define KXD_TYPES_H

#define MEMSIZE 2048ULL

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
#endif
typedef struct {
    cpu_t cpu;
#ifndef NOPPU
    ppu_t ppu;
#endif
    uint8_t *rom;
    size_t romSize;
    bool nes2format;
    uint8_t *PRG;
    size_t PRGSize;
    uint8_t *CHR;
    size_t CHRSize;
} nes_t;

typedef struct {
    size_t screenW, screenH;
    nes_t nes;
    bool quit;
} app_t;


#endif /* KXD_TYPES_H */

#pragma once
#ifndef KXD_6502_H
#define KXD_6502_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "InsFlags.h"
#include "kxdDebug.h"
#include "kxdMem.h"
#include "kxdTypes.h"

// non-maskable interrupt handler ($FFFA/B), the power on reset location ($FFFC/D) and
// the BRK/interrupt request handler ($FFFE/F)

#define INTERRUPT_HANDLER         0xFFFA
#define RESET_VECTOR              0xFFFC
#define INTERRUPT_REQUEST_HANDLER 0xFFFE

void processInstruction(cpu_t *cpu);
void debugCPU(cpu_t *cpu);
bool initCPU(cpu_t *cpu);

#ifdef KXD_6502_FILE
static inline uint16_t incrementPC(cpu_t *cpu) {
    cpu->PC = (cpu->PC + 1) & (MEMSIZE - 1);
    return cpu->PC;
}

static inline void writeWord(cpu_t *cpu, uint16_t value, size_t loc) {
    assert(loc < MEMSIZE);
    cpu->mem[loc] = value & 0xFF;

    cpu->mem[(loc + 1) % MEMSIZE] = (value >> 8) & 0xFF;
}

static inline uint16_t readWord(cpu_t *cpu, size_t loc) {
    assert(loc < MEMSIZE);
    return cpu->mem[loc] + (cpu->mem[(loc + 1) % MEMSIZE] << 8);
}

static inline void writeByte(cpu_t *cpu, uint8_t value, size_t loc) {
    assert(loc < MEMSIZE);
    cpu->mem[loc] = value;
}

static inline uint8_t readByte(cpu_t *cpu, size_t loc) {
    assert(loc < MEMSIZE);
    return cpu->mem[loc];
}

#endif /* KXD_6502_FILE */

#endif /* KXD_6502_H */

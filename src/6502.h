#ifndef KXD_6502_H
#define KXD_6502_H

#include <stdbool.h>
#include <stdint.h>

#include "InsFlags.h"
#include "kxdDebug.h"
#include "kxdTypes.h"

void processInstruction(cpu_t *cpu);
void debugCPU(cpu_t *cpu);

static inline uint16_t incrementPC(cpu_t *cpu) {
    cpu->PC = (cpu->PC + 1) & (MEMSIZE - 1);
    return cpu->PC;
}

#endif /* KXD_6502_H */

#ifndef KXD_6502_H
#define KXD_6502_H

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "InsFlags.h"
#include "kxdDebug.h"
#include "kxdMem.h"
#include "kxdTypes.h"

// non-maskable interrupt handler ($FFFA/B), the power on reset location ($FFFC/D) and the BRK/interrupt request handler ($FFFE/F)

#define INTERRUPT_HANDLER 0xFFFA
#define RESET_VECTOR 0xFFFC
#define INTERRUPT_REQUEST_HANDLER 0xFFFE

void processInstruction(cpu_t *cpu);
void debugCPU(cpu_t *cpu);
bool initCPU(cpu_t *cpu);

#ifdef KXD_6502_FILE
static inline uint16_t incrementPC(cpu_t *cpu) {
    cpu->PC = (cpu->PC + 1) & (MEMSIZE - 1);
    return cpu->PC;
}
#endif /* KXD_6502_FILE */

#endif /* KXD_6502_H */

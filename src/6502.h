#ifndef KXD_6502_H
#define KXD_6502_H

#include <stdint.h>
#include <stdbool.h>

#include "InsFlags.h"
#include "kxdDebug.h"
#include "kxdTypes.h"

void processInstruction(cpu_t *cpu);
void debugCPU(cpu_t *cpu);

#endif /* KXD_6502_H */

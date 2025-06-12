#include "kxdMem.h"

void addToMem(uint8_t *mem, size_t loc, uint64_t value) {
    size_t valueSize = 0;

    if (value <= UINT8_MAX)
        valueSize = sizeof(uint8_t);
    else if (value <= UINT16_MAX)
        valueSize = sizeof(uint16_t);
    else if (value <= UINT32_MAX)
        valueSize = sizeof(uint32_t);
    else
        valueSize = sizeof(uint64_t);
    // if (valueSize == 0)
    //     return;
    assert(loc + valueSize < MEMSIZE);
    for (size_t i = 0; i < valueSize; i++) {
        mem[loc + i] = (value & 0xFF);
        value        = value >> 8;
    }
}

void addMultipleToMem(uint8_t *mem, size_t loc, uint8_t *values, size_t valuesSize) {
    if (valuesSize == 0)
        return;
    if (valuesSize == 1) {
        addToMem(mem, loc, values[0]);
        return;
    }
    // VARLOG(loc, "%zu");
    // VARLOG(valuesSize, "%zu");
    // VARLOG(loc + valuesSize, "%zu");
    // VARLOG(MEMSIZE, "%zu");
    assert((loc + valuesSize) <= MEMSIZE);
    for (size_t i = 0; i < valuesSize; i++) {
        mem[loc + i] = values[i];
    }
}

void memDmp(cpu_t *cpu, size_t memSize) {
    const char *logPath = "memDmp.log";
    FILE *log           = fopen(logPath, "wt");

    if (log == NULL) {
        LOG_ERR("Could not open \'%s\': %s, Dumping log to STDOUT...", logPath, strerror(errno));
        log = stdout;
    }

    fprintf(log, "Registers\nPC: 0x%04X | SP: 0x%02X | A: 0x%02X | X: 0x%02X | Y: 0x%02X\n",
            cpu->PC, cpu->SP, cpu->A, cpu->X, cpu->Y);
    fprintf(log, "Status Registers:\n");
    fprintf(log, "    NV1B DIZC\n");
    fprintf(log, "    %d%d%d%d %d%d%d%d\n", cpu->N, cpu->V, 1, cpu->B, cpu->D, cpu->I, cpu->Z,
            cpu->C);

    for (size_t i = 0; i < 85; i++) {
        fputc('_', log);
    }
    fprintf(log, "\n");

    fprintf(log, "XXXX  ");
    for (int i = 0; i < 16; ++i) {
        fprintf(log, "%04X ", i);
    }
    fprintf(log, "\n0000: ");
    for (size_t i = 0; i < memSize; ++i) {
        if (i > 0 && i % 16 == 0) {
            fprintf(log, "\n%04zX: ", i);
        }
        fprintf(log, "0x%02X ", cpu->mem[i]);
    }
    fprintf(log, "\n");
    fflush(log);
    fclose(log);
}

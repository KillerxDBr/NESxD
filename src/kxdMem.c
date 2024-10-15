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
    if (valueSize == 0)
        return;
    assert(loc + valueSize < MEMSIZE);
    for (size_t i = 0; i < valueSize; i++) {
        mem[loc + i] = (value & 0xFF);
        value = value >> 8;
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
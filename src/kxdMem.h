#ifndef KXD_MEM_H
#define KXD_MEM_H

#include <assert.h>
#include "kxdTypes.h"

void addToMem(uint8_t *mem, size_t loc, uint64_t value);
void addMultipleToMem(uint8_t *mem, size_t loc, uint8_t *values, size_t valuesSize);

#endif /* KXD_MEM_H */

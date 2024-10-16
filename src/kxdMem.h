#ifndef KXD_MEM_H
#define KXD_MEM_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "kxdDebug.h"
#include "kxdTypes.h"

void addToMem(uint8_t *mem, size_t loc, uint64_t value);
void addMultipleToMem(uint8_t *mem, size_t loc, uint8_t *values, size_t valuesSize);
void memDmp(cpu_t *cpu, size_t memSize);

static inline void *callocWrapperFunc(size_t n, size_t sz, const char *fileName, const int lineNum) {
    void *ptr = calloc(n, sz);
    if (ptr == NULL) {
        LOG_ERR("%s:%d:  Could not allocate memory (%zu bytes), exiting...", fileName, lineNum, n * sz);
        // fprintf(stderr, "%s:%d: ERROR: Could not allocate memory (%zu bytes), exiting...\n\n", fileName, lineNum, n * sz);
        exit(1);
    }
    return ptr;
}
// void *callocWrapper(size_t _NumOfElements, size_t _SizeOfElements)
#define callocWrapper(n, sz) callocWrapperFunc((n), (sz), (__FILE__), (__LINE__))

#endif /* KXD_MEM_H */

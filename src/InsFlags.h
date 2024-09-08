#ifndef KXD_INS_FLAGS_H
#define KXD_INS_FLAGS_H

#include "instructions.h"

#define BITN(v, N) (v & (1 << N))

#define LD_FLAGS(reg)                                                                                                                      \
    do {                                                                                                                                   \
        cpu->Z = ((reg) == 0);                                                                                                             \
        cpu->N = BITN((reg), 7);                                                                                                     \
    } while (0)

#endif /* KXD_INS_FLAGS_H */


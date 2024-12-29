#pragma once
#ifndef KXD_INS_FLAGS_H
#define KXD_INS_FLAGS_H

// Instructions Enum separated for python script to be able to read file
#include "instructions.h"

#define BITN(v, N) ((bool)(v & (1 << N)))

/*
// Flags
#define FLAG_Z(value) (cpu->Z = ((value) == 0))
#define FLAG_N(value) (cpu->N = BITN((value), 7))
#define FLAG_C(oldValue, valueToAdd)                                                                                                       \
    do {                                                                                                                                   \
        if (cpu->D) {                                                                                                                      \
            cpu->C = (((oldValue) + (valueToAdd)) > 99);                                                                                   \
        } else {                                                                                                                           \
            cpu->C = (((oldValue) + (valueToAdd)) > 0xFF);                                                                                 \
        }                                                                                                                                  \
    } while (0)
#define FLAG_V(reg, oldValue, valueToAdd) (cpu->V = (BITN((reg), 7) != BITN(((oldValue) + (valueToAdd)), 7)))
*/

// Z, N Flags
#define LD_FLAGS(reg)                                                                                                                      \
    do {                                                                                                                                   \
        cpu->Z = ((reg) == 0);                                                                                                             \
        cpu->N = BITN((reg), 7);                                                                                                           \
    } while (0)

// Z, N Flags
#define DEC_FLAGS(memOrReg) LD_FLAGS(memOrReg)

// C, V, Z, N Flags
#define ADC_FLAGS(reg, oldValue, valueToAdd)                                                                                               \
    do {                                                                                                                                   \
        if (cpu->D) {                                                                                                                      \
            cpu->C = (((oldValue) + (valueToAdd)) > 99);                                                                                   \
        } else {                                                                                                                           \
            cpu->C = (((oldValue) + (valueToAdd)) > 0xFF);                                                                                 \
        }                                                                                                                                  \
        cpu->V = (BITN((reg), 7) != BITN((oldValue), 7));                                                                                  \
        LD_FLAGS((reg));                                                                                                                   \
    } while (0)

#define CLEAR_BIT(v, N)                                                                                                                    \
    do {                                                                                                                                   \
        if (BITN(v, N))                                                                                                                    \
            v -= (1 << N);                                                                                                                 \
    } while (0)

#endif /* KXD_INS_FLAGS_H */

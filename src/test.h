#ifndef KXD_TEST_H
#define KXD_TEST_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "nob.h"

#include "InsFlags.h"
#include "kxdDebug.h"
#include "kxdTypes.h"

#define TEST_BIN "test.bin"

bool InstructionTest(app_t *app, cpu_t *final);

#endif /* KXD_TEST_H */

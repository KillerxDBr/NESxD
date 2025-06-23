#pragma once
#ifndef KXD_TEST_H
#define KXD_TEST_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "InsFlags.h"
#include "kxdDebug.h"
#include "kxdTypes.h"

#include "nob.h"

#define TEST_BIN "test.bin"

bool InstructionTest(app_t *app, cpu_t *final);

#endif /* KXD_TEST_H */

#pragma once
#ifndef KXD_LANG_H
#define KXD_LANG_H

#include <string.h>

#include "kxdDebug.h"
#include "kxdTypes.h"

#include "nob.h"

#include "stb_ds.h"

void loadDefaultLang(lang_t *lang);

typedef struct {
    char *key;
    char *value;
} Hash;

typedef struct {
    Hash *items;
    size_t count, capacity;
} Languages;

#endif // KXD_LANG_H

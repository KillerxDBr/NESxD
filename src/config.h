#ifndef KXD_CONFIG_H
#define KXD_CONFIG_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>

#include "kxdTypes.h"
#include "kxdDebug.h"
#include "raylib.h"

#define CONFIG_FILE "config.bin"

void loadConfig(app_t *app);
void configController(app_t *app, uint16_t *config);
void configControllerDefault(app_t *app);
bool saveConfig(app_t *app);

#endif /* KXD_CONFIG_H */

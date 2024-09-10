#ifndef KXD_CONFIG_H
#define KXD_CONFIG_H

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "kxdDebug.h"
#include "kxdTypes.h"

#include "raylib.h"

#define CONFIG_FILE "config.ini"

void loadConfig(app_t *app);
void configController(app_t *app, uint16_t *config);
void configControllerDefault(app_t *app);

#ifndef NORINI
void saveConfig(app_t *app);
#else  /* NORINI */
bool saveConfig(app_t *app);
#endif /* NORINI */

#endif /* KXD_CONFIG_H */

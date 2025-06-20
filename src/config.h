#pragma once
#ifndef KXD_CONFIG_H
#define KXD_CONFIG_H

#include "nob.h"

#include "kxdDebug.h"
#include "kxdMem.h"
#include "kxdTypes.h"

#include "raylib.h"

#define RINI_VALUE_DELIMITER '='

#ifdef KXD_CONFIG_FILE
#define RINI_IMPLEMENTATION
#endif
#include "rini.h"

#ifndef PLATFORM_WEB
#include "WindowsHeader/WindowsHeader.h"
#endif /* PLATFORM_WEB */

#define CONFIG_FILE "config.ini"

void loadConfig(app_t *app);
void loadDefaultConfigs(app_t *app);
void configControllerDefault(app_t *app);

void saveConfig(app_t *app);

/*
"NESxD.selectedTheme",
"NESxD.fastForwardKey",
"NESxD.pauseKey",
"controller.ButtonUp",
"controller.ButtonDown",
"controller.ButtonLeft",
"controller.ButtonRight",
"controller.ButtonA",
"controller.ButtonB",
"controller.ButtonStart",
"controller.ButtonSelect",
*/

#define NESxD_LANG           "NESxD.lang"
#define NESxD_SELECTED_THEME "NESxD.selectedTheme"
#define NESxD_FAST_FORWARD   "NESxD.fastForwardKey"
#define NESxD_PAUSE          "NESxD.pauseKey"

#define CTRL_BT_UP     "controller.ButtonUp"
#define CTRL_BT_DOWN   "controller.ButtonDown"
#define CTRL_BT_LEFT   "controller.ButtonLeft"
#define CTRL_BT_RIGHT  "controller.ButtonRight"
#define CTRL_BT_A      "controller.ButtonA"
#define CTRL_BT_B      "controller.ButtonB"
#define CTRL_BT_START  "controller.ButtonStart"
#define CTRL_BT_SELECT "controller.ButtonSelect"

#endif /* KXD_CONFIG_H */

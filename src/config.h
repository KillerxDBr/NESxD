#ifndef KXD_CONFIG_H
#define KXD_CONFIG_H

#include "kxdDebug.h"
#include "kxdTypes.h"

#include "raylib.h"

#define RINI_VALUE_DELIMITER '='

#ifdef KXD_CONFIG_FILE
#define RINI_IMPLEMENTATION
#endif
#include "rini.h"

#include "nob.h"

#include "WindowsHeader/WindowsHeader.h"

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

// clang-format off
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
// clang-format on

#endif /* KXD_CONFIG_H */

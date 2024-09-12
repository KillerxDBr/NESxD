#ifndef KXD_CONFIG_H
#define KXD_CONFIG_H

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "kxdDebug.h"
#include "kxdTypes.h"

#include "raylib.h"

#define RINI_VALUE_DELIMITER '='
#include "rini.h"

#include "nob.h"

#define CONFIG_FILE "config.ini"

void loadConfig(app_t *app);
void loadDefaultConfigs(app_t *app);
// void configController(app_t *app, uint16_t *config);
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
#define NESxD_SELECTED_THEME "NESxD.selectedTheme"
#define NESxD_FAST_FORWARD   "NESxD.fastForwardKey"
#define NESxD_PAUSE          "NESxD.pauseKey"

#define CTRL_BT_UP      "controller.ButtonUp"              
#define CTRL_BT_DOWN    "controller.ButtonDown"            
#define CTRL_BT_LEFT    "controller.ButtonLeft"            
#define CTRL_BT_RIGHT   "controller.ButtonRight"           
#define CTRL_BT_A       "controller.ButtonA"               
#define CTRL_BT_B       "controller.ButtonB"               
#define CTRL_BT_START   "controller.ButtonStart"           
#define CTRL_BT_SELECT  "controller.ButtonSelect"           


#endif /* KXD_CONFIG_H */

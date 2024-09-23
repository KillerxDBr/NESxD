#ifndef KXD_GUI_H
#define KXD_GUI_H

#include <stdlib.h> // malloc
#include <string.h> // memcpy

#include "raylib.h"

#ifdef KXD_GUI_FILE

#define RAYMATH_STATIC_INLINE
#include "raymath.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define MAX_GUI_STYLES_AVAILABLE 12 // NOTE: Included light style
#include "ashes/style_ashes.h"
#include "bluish/style_bluish.h"
#include "candy/style_candy.h"
#include "cherry/style_cherry.h"
#include "cyber/style_cyber.h"
#include "dark/style_dark.h"
#include "enefete/style_enefete.h"
#include "jungle/style_jungle.h"
#include "lavanda/style_lavanda.h"
#include "sunny/style_sunny.h"
#include "terminal/style_terminal.h"

const char *themeNames[MAX_GUI_STYLES_AVAILABLE] = {
    "Light", "Ashes", "Bluish", "Candy", "Cherry", "Cyber", "Dark", "Enefete", "Jungle", "Lavanda", "Sunny", "Terminal",
};

#else
#include "raymath.h"

#include "raygui.h"
#endif /* KXD_GUI_FILE */

#include "kxdDebug.h"
#include "kxdTypes.h"

void KxDGui(app_t *app);
void initGui(app_t *app);
void updateTheme(app_t *app);

#endif /* KXD_GUI_H */

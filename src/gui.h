#ifndef KXD_GUI_H
#define KXD_GUI_H

#include <stdlib.h> // malloc
#include <string.h> // memcpy

#include "raylib.h"

#ifdef KXD_GUI_FILE

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define MAX_GUI_STYLES_AVAILABLE 12 // NOTE: Included light style
#include "../styles/ashes/style_ashes.h"
#include "../styles/bluish/style_bluish.h"
#include "../styles/candy/style_candy.h"
#include "../styles/cherry/style_cherry.h"
#include "../styles/cyber/style_cyber.h"
#include "../styles/dark/style_dark.h"
#include "../styles/enefete/style_enefete.h"
#include "../styles/jungle/style_jungle.h"
#include "../styles/lavanda/style_lavanda.h"
#include "../styles/sunny/style_sunny.h"
#include "../styles/terminal/style_terminal.h"

const char *themeNames[MAX_GUI_STYLES_AVAILABLE] = {
    "Light", "Ashes", "Bluish", "Candy", "Cherry", "Cyber", "Dark", "Enefete", "Jungle", "Lavanda", "Sunny", "Terminal",
};

#else
#include "raygui.h"
#endif /* KXD_GUI_FILE */

#include "kxdDebug.h"
#include "kxdTypes.h"

void KxDGui(app_t *app);
void initGui(app_t *app);
void updateTheme(app_t *app);

#endif /* KXD_GUI_H */

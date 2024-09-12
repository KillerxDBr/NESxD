#include "gui.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

void KxDGui(app_t *app) {
    (void)app;
    if (GuiButton(CLITERAL(Rectangle){ 10, 10, 100, 20 }, "Press Me!!!")) {
        LOG_INF("Button Pressed!!!");
    }
}

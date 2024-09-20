#include "gui.h"

// force loading dark theme for now
void initGui(app_t *app) {
    if (app->config.activeTheme != 0) {
        updateTheme(app);
    }
}

void updateTheme(app_t *app) {
    GuiLoadStyleDefault();
    // clang-format off
    switch (app->config.activeTheme) {
        case  1: GuiLoadStyleAshes();    break;
        case  2: GuiLoadStyleBluish();   break;
        case  3: GuiLoadStyleCandy();    break;
        case  4: GuiLoadStyleCherry();   break;
        case  5: GuiLoadStyleCyber();    break;
        case  6: GuiLoadStyleDark();     break;
        case  7: GuiLoadStyleEnefete();  break;
        case  8: GuiLoadStyleJungle();   break;
        case  9: GuiLoadStyleLavanda();  break;
        case 10: GuiLoadStyleSunny();    break;
        case 11: GuiLoadStyleTerminal(); break;
        default: break;
    }
    // clang-format on
}

void KxDGui(app_t *app) {
    app->menu.openFile = true;
    if (app->menu.openFile) {
        if (GuiButton(CLITERAL(Rectangle){ 10, 10, 100, 20 }, "Press Me!!!")) {
            // LOG_INF("Button Pressed!!!");
            app->config.activeTheme = (app->config.activeTheme + 1) & ((sizeof(themeNames) / sizeof(themeNames[0])) - 1);
            LOG_INF("Theme changed to %s (%u)", themeNames[app->config.activeTheme], app->config.activeTheme);
            updateTheme(app);
        }
    }

    const float spacing = 5.0f;
    const char *themeText = TextFormat("Active Theme: %s (%u)", themeNames[app->config.activeTheme], app->config.activeTheme);

    const Vector2 textSize = MeasureTextEx(GetFontDefault(), themeText, 15, 1);
    const Rectangle rect
        = { (app->screenW * .6f) - spacing, (app->screenH * .85f) - spacing, textSize.x + (spacing * 2), textSize.y + (spacing * 2) };

    // LOG_INF("TextSize: " V2_FMT, V2_ARGS(textSize));
    // LOG_INF("Screen Size: (%zu, %zu)", app->screenW, app->screenH);
    // LOG_INF("rect.x + rect.width = %f", rect.x + rect.width);
    // LOG_INF("rect.y + rect.height = %f", rect.y + rect.height);
    // assert(rect.x + rect.width <= app->screenW && rect.y + rect.height <= app->screenH);
    // LOG_INF("Drawing Rectangle at: " RECT_FMT, RECT_ARGS(rect));

    DrawRectangleRec(rect, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    DrawText(themeText, rect.x, rect.y, 15, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));
}

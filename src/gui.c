#include "gui.h"

// force loading dark theme for now
void initGui(app_t *app) {
    updateTheme(app); //
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
    if (GuiButton(CLITERAL(Rectangle){10, 10 + MENU_BAR_SIZE, 100, 20}, "Press Me!!!")) {
        app->config.activeTheme = (app->config.activeTheme + 1) % KXD_ARRAY_LEN(themeNames);
        LOG_INF("Theme changed to \"%s\" (%" PRId32 ")", themeNames[app->config.activeTheme],
                app->config.activeTheme + 1);
        updateTheme(app);
    }

    const Vector2 spacing = V2(app->screenW * .01f, app->screenH * .01f);
    const Font font       = GuiGetFont();
    const float FontSize  = font.baseSize;
    const int textSpacing = GuiGetStyle(DEFAULT, TEXT_SPACING);
    const char *themeText =
        TextFormat("Active Theme: %s (%" PRId32 ")", themeNames[app->config.activeTheme], app->config.activeTheme + 1);

    const Vector2 textSize = MeasureTextEx(font, themeText, FontSize, textSpacing);

    Rectangle rect = {
        .x      = (app->screenW * .6f) - spacing.x,
        .y      = (app->screenH * .85f) - spacing.y,
        .width  = textSize.x + (spacing.x * 2),
        .height = textSize.y + (spacing.y * 2),
    };

    if ((rect.x + rect.width) >= (app->screenW * .9f))
        rect.x -= ((rect.x + rect.width) - (app->screenW * .9f));

    if ((rect.y + rect.height) >= (app->screenH * .9f))
        rect.y -= ((rect.y + rect.height) - (app->screenH * .9f));

    DrawRectangleRec(rect, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    const Color textColor = GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL));
    DrawRectangleLinesEx(rect, rect.height * .05f, textColor);

    DrawTextPro(font, themeText, V2(rect.x + spacing.x, rect.y + spacing.y), Vector2Zero(), 0, FontSize, textSpacing,
                textColor);
}

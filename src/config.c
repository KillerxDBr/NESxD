#include "config.h"

void loadConfig(app_t *app) {
    LOG_INF("Loading config from '%s'", app->config.fileName);

    loadDefaultConfigs(app);

    if (!FileExists(app->config.fileName)) {
        LOG_INF("File '%s' dont exist!", app->config.fileName);
        saveConfig(app);
    } else {
        LOG_INF("File '%s' exist!", app->config.fileName);
        rini_config cfg = rini_load_config(app->config.fileName);

        // clang-format off
        app->config.activeTheme    = rini_get_config_value_fallback(cfg, NESxD_SELECTED_THEME, app->config.activeTheme);
        app->config.fastForwardKey = rini_get_config_value_fallback(cfg, NESxD_FAST_FORWARD,   app->config.fastForwardKey);
        app->config.pauseKey       = rini_get_config_value_fallback(cfg, NESxD_PAUSE,          app->config.pauseKey);

        app->nes.controller.ButtonUp    = rini_get_config_value_fallback(cfg, CTRL_BT_UP,    app->nes.controller.ButtonUp);
        app->nes.controller.ButtonDown  = rini_get_config_value_fallback(cfg, CTRL_BT_DOWN,  app->nes.controller.ButtonDown);
        app->nes.controller.ButtonLeft  = rini_get_config_value_fallback(cfg, CTRL_BT_LEFT,  app->nes.controller.ButtonLeft);
        app->nes.controller.ButtonRight = rini_get_config_value_fallback(cfg, CTRL_BT_RIGHT, app->nes.controller.ButtonRight);

        app->nes.controller.ButtonA = rini_get_config_value_fallback(cfg, CTRL_BT_A, app->nes.controller.ButtonA);
        app->nes.controller.ButtonB = rini_get_config_value_fallback(cfg, CTRL_BT_B, app->nes.controller.ButtonB);

        app->nes.controller.ButtonStart  = rini_get_config_value_fallback(cfg, CTRL_BT_START,  app->nes.controller.ButtonStart);
        app->nes.controller.ButtonSelect = rini_get_config_value_fallback(cfg, CTRL_BT_SELECT, app->nes.controller.ButtonSelect);

        VARLOG(app->config.activeTheme,    "%u");
        VARLOG(app->config.fastForwardKey, "%u");
        VARLOG(app->config.pauseKey,       "%u");

        VARLOG(app->nes.controller.ButtonUp,    "%u");
        VARLOG(app->nes.controller.ButtonDown,  "%u");
        VARLOG(app->nes.controller.ButtonLeft,  "%u");
        VARLOG(app->nes.controller.ButtonRight, "%u");

        VARLOG(app->nes.controller.ButtonA, "%u");
        VARLOG(app->nes.controller.ButtonB, "%u");

        VARLOG(app->nes.controller.ButtonStart,  "%u");
        VARLOG(app->nes.controller.ButtonSelect, "%u");
        // clang-format on

        rini_unload_config(&cfg);
    }
}

void saveConfig(app_t *app) {
    Nob_String_Builder sb = { 0 };

    nob_da_append(&sb, RINI_LINE_COMMENT_DELIMITER);
    nob_sb_append_cstr(&sb, " NESxD ================================================================\n");
    nob_da_append(&sb, RINI_LINE_COMMENT_DELIMITER);
    nob_sb_append_cstr(&sb, " Config File for 'program'\n");
    nob_da_append(&sb, RINI_LINE_COMMENT_DELIMITER);
    nob_da_append(&sb, '\n');
    nob_da_append(&sb, RINI_LINE_COMMENT_DELIMITER);
    nob_sb_append_cstr(&sb, " Check readme.txt for key and theme numbers\n");
    nob_da_append(&sb, RINI_LINE_COMMENT_DELIMITER);
    nob_da_append(&sb, '\n');
    nob_sb_append_null(&sb);

    // sprintf(header,header,RINI_LINE_COMMENT_DELIMITER,RINI_LINE_COMMENT_DELIMITER,RINI_LINE_COMMENT_DELIMITER,RINI_LINE_COMMENT_DELIMITER,RINI_LINE_COMMENT_DELIMITER);

    rini_config cfg = rini_load_config(NULL);

    // clang-format off
    rini_set_config_value(&cfg, NESxD_SELECTED_THEME, app->config.activeTheme,    "GUI Theme");
    rini_set_config_value(&cfg, NESxD_FAST_FORWARD,   app->config.fastForwardKey, "Fast Forward Button");
    rini_set_config_value(&cfg, NESxD_PAUSE,          app->config.pauseKey,       "Pause Button");

    rini_set_config_value(&cfg, CTRL_BT_UP,    app->nes.controller.ButtonUp,    "DPad Up");
    rini_set_config_value(&cfg, CTRL_BT_DOWN,  app->nes.controller.ButtonDown,  "DPad Down");
    rini_set_config_value(&cfg, CTRL_BT_LEFT,  app->nes.controller.ButtonLeft,  "DPad Left");
    rini_set_config_value(&cfg, CTRL_BT_RIGHT, app->nes.controller.ButtonRight, "DPad Right");

    rini_set_config_value(&cfg, CTRL_BT_A, app->nes.controller.ButtonA, "A Button");
    rini_set_config_value(&cfg, CTRL_BT_B, app->nes.controller.ButtonB, "B Button");

    rini_set_config_value(&cfg, CTRL_BT_START,  app->nes.controller.ButtonStart,  "Start Button");
    rini_set_config_value(&cfg, CTRL_BT_SELECT, app->nes.controller.ButtonSelect, "Select Button");
    // clang-format on

    LOG_INF("Saving config to '%s'", app->config.fileName);
    rini_save_config(cfg, app->config.fileName, sb.items);

    rini_unload_config(&cfg);

    nob_sb_free(sb);
}

void configControllerDefault(app_t *app) {
    // clang-format off
    app->nes.controller.ButtonUp    = KEY_W;
    app->nes.controller.ButtonDown  = KEY_S;
    app->nes.controller.ButtonLeft  = KEY_A;
    app->nes.controller.ButtonRight = KEY_D;

    app->nes.controller.ButtonA = KEY_Z;
    app->nes.controller.ButtonB = KEY_X;

    app->nes.controller.ButtonStart  = KEY_E;
    app->nes.controller.ButtonSelect = KEY_Q;
    // clang-format on
}

void loadDefaultConfigs(app_t *app) {
    configControllerDefault(app);
    unsigned long size = 10;
    char *result = callocWrapper(size, 1);

    bool darkTheme;
#if defined(_WIN32) && !defined(PLATFORM_WEB)
    if (WinH_RegGetValueA(WIN_H_HKEY_CURRENT_USER, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                          "SystemUsesLightTheme", WIN_H_REG_DWORD, NULL, result, &size) == 0) {
        darkTheme = (result[3] << 24 | result[2] << 16 | result[1] << 8 | result[0]) == 0;
    } else {
        darkTheme = false;
    }
#else
    darkTheme = true; // Forcing Dark Theme for other platforms by now
#endif /* defined(_WIN32) && !defined(PLATFORM_WEB) */

    // clang-format off
    app->config.activeTheme    = darkTheme ? 6 : 0;
    app->config.fastForwardKey = KEY_SPACE;
    app->config.pauseKey       = KEY_P;
    // clang-format on

    free(result);
}

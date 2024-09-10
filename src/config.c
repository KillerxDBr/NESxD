#include "config.h"


// Including rini.h here to avoid redefinitions
#ifndef NORINI

#define RINI_VALUE_DELIMITER '='

#define RINI_IMPLEMENTATION
#include "rini.h"

#endif /* NORINI */

#ifdef NORINI
void loadConfig(app_t *app) {
    FILE *cfgFile = NULL;
    uint16_t *config = callocWrapper(8, sizeof(uint16_t));
    app->config.hasConfig = FileExists(app->config.fileName);

    if (FileExists(app->config.fileName)) {
        LOG_INF("File Exists: %s", app->config.fileName);

        cfgFile = fopen(app->config.fileName, "rb");

        fseek(cfgFile, 0, SEEK_END);
        size_t cfgSize = ftell(cfgFile);
        VARLOG(cfgSize, "%zu");
        rewind(cfgFile);
        if (cfgSize >= (8 * sizeof(uint16_t)))
            configController(app, config);
        else
            configControllerDefault(app);
    }

    if (cfgFile)
        fclose(cfgFile);
}

void configController(app_t *app, uint16_t *config) {
    app->nes.controller.ButtonUp    = config[0];
    app->nes.controller.ButtonDown  = config[1];
    app->nes.controller.ButtonLeft  = config[2];
    app->nes.controller.ButtonRight = config[3];

    app->nes.controller.ButtonA = config[4];
    app->nes.controller.ButtonB = config[5];

    app->nes.controller.ButtonStart  = config[6];
    app->nes.controller.ButtonSelect = config[7];
}

bool saveConfig(app_t *app) {
    FILE *cfgFile = fopen(app->config.fileName, "wb");

    LOG_INF("Saving config file...");

    if (cfgFile != NULL) {
        LOG_INF("Writing to file");
        fwrite(&app->nes.controller.ButtonUp,    sizeof(uint16_t), 1, cfgFile);
        fwrite(&app->nes.controller.ButtonDown,  sizeof(uint16_t), 1, cfgFile);
        fwrite(&app->nes.controller.ButtonLeft,  sizeof(uint16_t), 1, cfgFile);
        fwrite(&app->nes.controller.ButtonRight, sizeof(uint16_t), 1, cfgFile);

        fwrite(&app->nes.controller.ButtonA, sizeof(uint16_t), 1, cfgFile);
        fwrite(&app->nes.controller.ButtonB, sizeof(uint16_t), 1, cfgFile);

        fwrite(&app->nes.controller.ButtonStart,  sizeof(uint16_t), 1, cfgFile);
        fwrite(&app->nes.controller.ButtonSelect, sizeof(uint16_t), 1, cfgFile);
    } else {
        LOG_ERR("Could not write config to \'%s\': %s", app->config.fileName, strerror(errno));
    }
    return false;

    if (cfgFile)
        fclose(cfgFile);

    return true;
}
#else
void loadConfig(app_t *app) {
    LOG_INF("Loading config from '%s'", app->config.fileName);

    configControllerDefault(app);

    if (!FileExists(app->config.fileName)) {
        LOG_INF("File '%s' dont exist!",app->config.fileName);
        saveConfig(app);
    } else {
        LOG_INF("File '%s' exist!",app->config.fileName);
        rini_config cfg = rini_load_config(app->config.fileName);

        app->nes.controller.ButtonUp     = rini_get_config_value_fallback(cfg, "controller.ButtonUp",     app->nes.controller.ButtonUp);
        app->nes.controller.ButtonDown   = rini_get_config_value_fallback(cfg, "controller.ButtonDown",   app->nes.controller.ButtonDown);
        app->nes.controller.ButtonLeft   = rini_get_config_value_fallback(cfg, "controller.ButtonLeft",   app->nes.controller.ButtonLeft);
        app->nes.controller.ButtonRight  = rini_get_config_value_fallback(cfg, "controller.ButtonRight",  app->nes.controller.ButtonRight);

        app->nes.controller.ButtonA      = rini_get_config_value_fallback(cfg, "controller.ButtonA",      app->nes.controller.ButtonA);
        app->nes.controller.ButtonB      = rini_get_config_value_fallback(cfg, "controller.ButtonB",      app->nes.controller.ButtonB);

        app->nes.controller.ButtonStart  = rini_get_config_value_fallback(cfg, "controller.ButtonStart",  app->nes.controller.ButtonStart);
        app->nes.controller.ButtonSelect = rini_get_config_value_fallback(cfg, "controller.ButtonSelect", app->nes.controller.ButtonSelect);

#ifdef KXD_DEBUG
        VARLOG(app->nes.controller.ButtonUp,     "%u");
        VARLOG(app->nes.controller.ButtonDown,   "%u");
        VARLOG(app->nes.controller.ButtonLeft,   "%u");
        VARLOG(app->nes.controller.ButtonRight,  "%u");
        VARLOG(app->nes.controller.ButtonA,      "%u");
        VARLOG(app->nes.controller.ButtonB,      "%u");
        VARLOG(app->nes.controller.ButtonStart,  "%u");
        VARLOG(app->nes.controller.ButtonSelect, "%u");
#endif
        rini_unload_config(&cfg);
    }
    // exit(0);
}

void saveConfig(app_t *app) {
    const char *header =
    "# NESxD ================================================================\n"
    "# Config File for 'program'\n"
    "#\n"
    "# Check readme.txt for key numbers\n"
    "#\n";

    // sprintf(header,header,RINI_LINE_COMMENT_DELIMITER,RINI_LINE_COMMENT_DELIMITER,RINI_LINE_COMMENT_DELIMITER,RINI_LINE_COMMENT_DELIMITER,RINI_LINE_COMMENT_DELIMITER);

    rini_config cfg = rini_load_config(NULL);

    rini_set_config_value(&cfg, "controller.ButtonUp",     app->nes.controller.ButtonUp,     "DPad Up");
    rini_set_config_value(&cfg, "controller.ButtonDown",   app->nes.controller.ButtonDown,   "DPad Down");
    rini_set_config_value(&cfg, "controller.ButtonLeft",   app->nes.controller.ButtonLeft,   "DPad Left");
    rini_set_config_value(&cfg, "controller.ButtonRight",  app->nes.controller.ButtonRight,  "DPad Right");

    rini_set_config_value(&cfg, "controller.ButtonA",      app->nes.controller.ButtonA,      "A Button");
    rini_set_config_value(&cfg, "controller.ButtonB",      app->nes.controller.ButtonB,      "B Button");

    rini_set_config_value(&cfg, "controller.ButtonStart",  app->nes.controller.ButtonStart,  "Start Button");
    rini_set_config_value(&cfg, "controller.ButtonSelect", app->nes.controller.ButtonSelect, "Select Button");

    LOG_INF("Saving config to '%s'", app->config.fileName);
    rini_save_config(cfg, app->config.fileName, header);

    rini_unload_config(&cfg);
}
#endif

void configControllerDefault(app_t *app) {
    app->nes.controller.ButtonUp     = KEY_W;
    app->nes.controller.ButtonDown   = KEY_S;
    app->nes.controller.ButtonLeft   = KEY_A;
    app->nes.controller.ButtonRight  = KEY_D;

    app->nes.controller.ButtonA      = KEY_Z;
    app->nes.controller.ButtonB      = KEY_X;

    app->nes.controller.ButtonStart  = KEY_ENTER;
    app->nes.controller.ButtonSelect = KEY_BACKSPACE;
}

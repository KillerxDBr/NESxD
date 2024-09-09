#include "config.h"

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

void configControllerDefault(app_t *app) {
    app->nes.controller.ButtonUp    = KEY_W;
    app->nes.controller.ButtonDown  = KEY_S;
    app->nes.controller.ButtonLeft  = KEY_A;
    app->nes.controller.ButtonRight = KEY_D;

    app->nes.controller.ButtonA = KEY_Z;
    app->nes.controller.ButtonB = KEY_X;

    app->nes.controller.ButtonStart  = KEY_ENTER;
    app->nes.controller.ButtonSelect = KEY_BACKSPACE;
}

bool saveConfig(app_t *app) {
    FILE *cfgFile = fopen(app->config.fileName, "wb");

    LOG_INF("Saving config file...");

    if (cfgFile != NULL) {
        LOG_INF("Writing to file");
        fwrite(&app->nes.controller.ButtonUp,     sizeof(uint16_t), 1, cfgFile);
        fwrite(&app->nes.controller.ButtonDown,   sizeof(uint16_t), 1, cfgFile);
        fwrite(&app->nes.controller.ButtonLeft,   sizeof(uint16_t), 1, cfgFile);
        fwrite(&app->nes.controller.ButtonRight,  sizeof(uint16_t), 1, cfgFile);
 
        fwrite(&app->nes.controller.ButtonA,      sizeof(uint16_t), 1, cfgFile);
        fwrite(&app->nes.controller.ButtonB,      sizeof(uint16_t), 1, cfgFile);

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

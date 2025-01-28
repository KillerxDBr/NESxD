#include "lang.h"

void loadDefaultLang(lang_t *lang) {
    lang->base = NULL;
    memcpy(lang->code,    "en", 2);
    memcpy(lang->country, "us", 2);
    lang->name           = "English (US)";
    lang->author         = "KxD";
    lang->text_paused    = "Paused...";
    lang->menu_file      = "File";
    lang->menu_file_open = "Open";
    lang->menu_file_quit = "Quit";
}

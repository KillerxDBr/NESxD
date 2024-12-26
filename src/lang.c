#include "lang.h"

void loadDefaultLang(lang_t *lang) {
    strcpy(lang->code,     "en_US");
    lang->name           = "English";
    lang->author         = "KxD";
    lang->text_paused    = "Paused...";
    lang->menu_file      = "File";
    lang->menu_file_open = "Open";
    lang->menu_file_quit = "Quit";
}

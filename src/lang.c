#define STB_DS_IMPLEMENTATION
#include "lang.h"

Languages langs = {};

void loadAllLanguages(void);

void loadDefaultLang(lang_t *lang) {
    memset(lang, 0, sizeof(lang_t));

    // clang-format off
    lang->base = NULL;
    memcpy(lang->code,     "en", 2);
    memcpy(lang->country,  "us", 2);
    lang->name           = "English (US)";
    lang->author         = "KxD";
    lang->text_paused    = "Paused...";
    lang->menu_file      = "File";
    lang->menu_file_open = "Open";
    lang->menu_file_quit = "Quit";
    // clang-format on

    loadAllLanguages();
}

#define LANG_DIR "../lang"

void loadAllLanguages(void) {
    LOG_INF("Checking If LANG Dir Exists");
    if (!nob_file_exists(LANG_DIR))
        return;

    Nob_File_Paths fp     = {};
    Nob_String_Builder sb = {};
    const size_t cp       = nob_temp_save();

    nob_read_entire_dir(LANG_DIR, &fp);

    LOG_INF("--------------------------------------------");
    for (size_t i = 0; i < fp.count; ++i) {
        if (fp.items[i][0] == '.' && (fp.items[i][1] == 0 || (fp.items[i][1] == '.' && fp.items[i][2] == 0)))
            continue;
        sb.count = 0;
        LOG_INF("Path %zu: %s", i - 1, fp.items[i]);
    }
    /*
        extern char *nob_temp_ptr;
        puts("Nob Temp Content:\n'");
        // printf(SV_Fmt"\n", (int)(nob_temp_save() - cp), nob_temp[cp-1]);
        size_t end = nob_temp_save();
        for (size_t i = 0; i < (end - cp); ++i) {
            char c = nob_temp_ptr[cp + i];
            if (isprint(c) != 0)
                putc(c, stdout);
            else
                putc('.', stdout);
        }
        printf("'\nContent printed: %zu\n", end - cp);
    */
    nob_temp_rewind(cp);
    nob_da_free(fp);
    nob_sb_free(sb);
}

bool loadSingleLanguage(const char *filePath, Nob_String_Builder *sb) {
    NOB_UNUSED(filePath);
    NOB_UNUSED(sb);
    return false;
}

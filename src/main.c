#include "main.h"

// clang-format off
void simple_logger(int logType, const char *text, va_list args) {
    if (logType < GetTraceLogLevel()) return;

#if defined(PLATFORM_ANDROID)
    switch (logType)
    {
        case LOG_TRACE: __android_log_vprint(ANDROID_LOG_VERBOSE, "raylib", text, args); break;
        case LOG_DEBUG: __android_log_vprint(ANDROID_LOG_DEBUG, "raylib", text, args); break;
        case LOG_INFO: __android_log_vprint(ANDROID_LOG_INFO, "raylib", text, args); break;
        case LOG_WARNING: __android_log_vprint(ANDROID_LOG_WARN, "raylib", text, args); break;
        case LOG_ERROR: __android_log_vprint(ANDROID_LOG_ERROR, "raylib", text, args); break;
        case LOG_FATAL: __android_log_vprint(ANDROID_LOG_FATAL, "raylib", text, args); break;
        default: break;
    }
    return;
#else
    switch (logType)
    {
        case LOG_TRACE:   printf("TRACE: "  ); break;
        case LOG_DEBUG:   printf("DEBUG: "  ); break;
        case LOG_INFO:    printf("INFO: "   ); break;
        case LOG_WARNING: printf("WARNING: "); break;
        case LOG_ERROR:   printf("ERROR: "  ); break;
        case LOG_FATAL:   printf("FATAL: "  ); break;
        default: break;
    }

    vprintf(text, args);
    putc('\n', stdout);
    fflush(stdout);
#endif

    if (logType == LOG_FATAL) exit(EXIT_FAILURE);  // If fatal logging, exit program

}
// clang-format on

int main(int argc, char **argv) {
    SetTraceLogCallback(&simple_logger);

#if defined(_WIN32) && !defined(__EMSCRIPTEN__)
    setlocale(LC_CTYPE, ".UTF8");
    SetErrorMode(SEM_FAILCRITICALERRORS);

    if (!WinH_SetConsoleOutputCP(CP_UTF8)) {
        TraceLog(LOG_WARNING, "Could not set console output to 'UTF-8'");
    }

    WVResp winVer = GetWindowsVersion();
    if (winVer >= WIN_10) {
        TraceLog(LOG_INFO, "Enabling buffer on console std outputs");

#define IOBUFFSZ 1024
        // on Windows 10+ we need buffering or console will get 1 byte at a time (screwing up utf-8 encoding)
        if (setvbuf(stderr, NULL, _IOFBF, IOBUFFSZ) != 0) {
            TraceLog(LOG_WARNING, "Could not set \"%s\" buffer to %d: %s", "stderr", IOBUFFSZ, strerror(errno));
        }

        if (setvbuf(stdout, NULL, _IOFBF, IOBUFFSZ) != 0) {
            TraceLog(LOG_WARNING, "Could not set \"%s\" buffer to %d: %s", "stdout", IOBUFFSZ, strerror(errno));
        }
    }

    int argc_bkp    = argc;
    char **argv_bkp = argv;
    if (!WinH_win32_uft8_cmdline_args(&argc, &argv)) {
        TraceLog(LOG_WARNING, "Could not generate Win32 UTF-8 Command Line Arguments, using default...");
        argc = argc_bkp;
        argv = argv_bkp;
    }

    // for(int i = 0; i < argc; ++i) {
    //     LOG_INF("%d: \"%s\"", i+1, argv[i]);
    // }
    // return 0;
#else
    setlocale(LC_CTYPE, "");
#endif // defined(_WIN32) && !defined(__EMSCRIPTEN__)

    LOG_INF("CTYPE Locale set to \"%s\"", setlocale(LC_CTYPE, NULL));

    bool *NOP  = flag_bool("NOP", false, "NOP");
    bool *TEST = flag_bool("TEST", false, "TEST");

    if (!flag_parse(argc, argv)) {
        // usage(stderr);
        flag_print_error(stderr);
        exit(1);
    }

    app_t *app = callocWrapper(1, app_t);
    // app->program = nob_shift(argv, argc);
    app->program = flag_program_name();

    initCPU(&app->nes.cpu);

#ifdef PLATFORM_WEB
    *NOP = true;
#endif /* PLATFORM_WEB */

    app->nes.isPaused = *NOP;

#ifndef PLATFORM_WEB

    app->config.fileName = callocWrapper(strlen(app->program) + sizeof(CONFIG_FILE), char);
    memcpy((void *)app->config.fileName, app->program, strlen(app->program));

    const char *exeName = nob_path_name(app->config.fileName);

    const size_t nameIndex = exeName - app->config.fileName;
    // char *slash = strrchr(app->config.fileName, '\\');
    // assert(slash != NULL);

    // slash++;

    memcpy((void *)exeName, CONFIG_FILE, sizeof(CONFIG_FILE));
    app->config.fileName = realloc((void *)app->config.fileName, strlen(app->config.fileName) + 1);

    VARLOG(app->config.fileName, "'%s'");

    char *cwd = strdup(app->program);
    memset(cwd + nameIndex - 1, 0, 1);

    for (size_t i = 0; i < strlen(cwd); ++i) {
        if (cwd[i] == '\\') {
            cwd[i] = '/';
        }
    }

    VARLOG(cwd, "'%s'");
    VARLOG(nob_path_name(cwd), "'%s'");

    if (DirectoryExists(TextFormat("%s/rom", cwd)) && strcmp(nob_path_name(cwd), "bin") == 0) {
        memset((char *)nob_path_name(cwd) - 1, 0, 1);
    }

    if (!ChangeDirectory(cwd)) {
        LOG_ERR("Could not change directory to '%s'", cwd);
        return 1;
    }

    free(cwd);

#endif
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow((NES_W * FACTOR), (NES_H * FACTOR) + MENU_BAR_SIZE, "NES_xD");

#ifndef PLATFORM_WEB
    SetWindowMinSize(NES_W, NES_H + MENU_BAR_SIZE);
    SetTargetFPS(60);
    loadIconFile();
#endif /* PLATFORM_WEB */

    app->screenW = GetRenderWidth();
    app->screenH = GetRenderHeight();
    LOG_INF("Screen Width:  %" PRIu16, app->screenW);
    LOG_INF("Screen Heigth: %" PRIu16, app->screenH);

#ifdef _WIN32
    KxD_Create_Tray(GetWindowHandle());
#endif

    // GuiLoadStyleDefault();

#ifndef NOVID

    puts("=================================");
    TraceLog(LOG_WARNING, "SEED IS FIXED");
    puts("=================================");

    SetRandomSeed(40028922U);

    int *seq = LoadRandomSequence(NES_W * NES_H, 0, 0xFFFFFF);
    for (size_t i = 0; i < NES_W * NES_H; ++i)
        seq[i] = (seq[i] << 8) + 0xFF;

    app->screen = LoadRenderTexture(NES_W, NES_H);

    app->sourceRec =
        CLITERAL(Rectangle){0.0f, 0.0f, (float)app->screen.texture.width, -(float)app->screen.texture.height};
    app->destRec =
        CLITERAL(Rectangle){0.0f, (float)MENU_BAR_SIZE, (float)app->screenW, (float)app->screenH - MENU_BAR_SIZE};

    /*
    if (app->screenW >= app->screenH * NES_AR) {
        destRec.width = app->screenH * NES_AR;
        destRec.height = destRec.width * (1 / NES_AR);
        destRec.x = (app->screenW * .5f) - (destRec.width * .5f);
        destRec.y = 0;
        if (destRec.x < 0) {
            destRec.x = 0;
        }
    } else {
        destRec.height = app->screenW * (1 / NES_AR);
        destRec.width = destRec.height * NES_AR;
        destRec.x = 0;
        destRec.y = (app->screenH * .5f) - (destRec.height * .5f);
        if (destRec.y < 0) {
            destRec.y = 0;
        }
    }
    */

    BeginTextureMode(app->screen);

    ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    for (int y = 0; y < app->screen.texture.height; ++y) {
        for (int x = 0; x < app->screen.texture.width; ++x) {
            DrawPixel(x, y, GetColor(seq[XY2Index(x, y, NES_W)]));
        }
    }

    // VARLOG(screen.texture.width, "%d");
    // VARLOG(screen.texture.height, "%d");

    for (int x = 0; x < app->screen.texture.width; x++) {
        DrawPixel(x, app->screen.texture.height - 1, CLITERAL(Color){255, 0, 0, 255});
        DrawPixel(x, 0, CLITERAL(Color){0, 255, 0, 255});
    }
    for (int y = 0; y < app->screen.texture.height; y++) {
        DrawPixel(0, y, CLITERAL(Color){0, 0, 255, 255});
        DrawPixel(app->screen.texture.width - 1, y, CLITERAL(Color){255, 255, 0, 255});
    }

    EndTextureMode();

    UnloadRandomSequence(seq);
#endif /* NOVID */

#ifndef PLATFORM_WEB
    cpu_t final = {};
#endif

#if defined(KXD_DEBUG)
    if (*TEST) {
#if !defined(PLATFORM_WEB)
        // final = callocWrapper(1, cpu_t);
        app->nes.isPaused = false;
        if (!InstructionTest(app, &final))
            return 1;
        LOG_INF("SUCCESS");
#endif // !defined(PLATFORM_WEB)
    } else {
#endif // defined(KXD_DEBUG)
        memset(app->nes.cpu.mem, INS_NOP, MEMSIZE);

        const char *memBinPath = "./mem.bin";

        if (!*NOP) {
            Nob_String_Builder memory = {};
            if (nob_read_entire_file(memBinPath, &memory)) {
                LOG_INF("Reading %zu instructions from \"%s\"", memory.count, memBinPath);
                addMultipleToMem(app->nes.cpu.mem, 0, (uint8_t *)memory.items, memory.count);
            } else {
                LOG_INF("Operating with \"INS_NOP\" only");
                *NOP = true;
            }

            nob_sb_free(memory);
            memset(&memory, 0, sizeof(memory));
        }
#if defined(KXD_DEBUG)
    }
#endif // defined(KXD_DEBUG)

#ifdef PLATFORM_WEB
    LOG_INF("[TODO] Load ROM from user PC to WASM version");
    LOG_INF("[TODO] Save config in cookies");
#else
    LOG_INF("[TODO] Move this part to a button action");

    const char *fileName = "./rom/smb.nes";
    loadRomFromMem(&app->nes, fileName);

    loadConfig(app);
#endif // PLATFORM_WEB

    loadDefaultLang(&app->lang);
    initGui(app);
    rlImGuiSetup(true);

    ImGuiIO *io = igGetIO();
    // io->ConfigFlags
    io->IniFilename = NULL;

#ifdef PLATFORM_WEB
    emscripten_set_main_loop_arg(mainLoop, app, 60, 1);
#else
    // while (!WindowShouldClose() && !app->quit)
    mainLoop(app);

#if 0
    if (TEST) {
        final.B = true; // Probable only necessary for this sample test
        printf("\n===========================\n");
        bool testResult
            = (memcmp(&app->nes.cpu, &final, (sizeof(cpu_t) - MEMSIZE - 1)) == 0) && (memcmp(&app->nes.cpu.mem, final.mem, MEMSIZE) == 0);

        for (size_t i = 0; i < sizeof(cpu_t) - MEMSIZE - 1; i++)
            LOG_INF("cpu: 0x%02X | final: 0x%02X", ((uint8_t *)&app->nes.cpu)[i], ((uint8_t *)&final)[i]);

        // exit(0);

        if (testResult) {
            LOG_INF("SUCESS!!! app->nes.cpu == final");
        } else {
            LOG_ERR("NOPE!!! app->nes.cpu != final");
        }
        LOG_INF("");
        LOG_INF("Test Log ------------------");
        LOG_INF("Program Counter:");
        LOG_INF("Value: %u | Expected %u", app->nes.cpu.PC, final.PC);
        LOG_INF("===========================");
        LOG_INF("Stack Pointer");
        LOG_INF("Value: %u | Expected %u", app->nes.cpu.SP, final.SP);
        LOG_INF("===========================");
        LOG_INF("Registers:");
        LOG_INF("A: Value: %5u | Expected %5u", app->nes.cpu.A, final.A);
        LOG_INF("X: Value: %5u | Expected %5u", app->nes.cpu.X, final.X);
        LOG_INF("Y: Value: %5u | Expected %5u", app->nes.cpu.Y, final.Y);
        LOG_INF("===========================");
        LOG_INF("Status Registers:");
        LOG_INF("          NV1B DIZC");
        LOG_INF("Value:    %d%d%d%d %d%d%d%d", app->nes.cpu.N, app->nes.cpu.V, 1, app->nes.cpu.B, app->nes.cpu.D, app->nes.cpu.I,
                app->nes.cpu.Z, app->nes.cpu.C);
        LOG_INF("===========================");
        LOG_INF("          NV1B DIZC");
        LOG_INF("Expected: %d%d%d%d %d%d%d%d", final.N, final.V, 1, final.B, final.D, final.I, final.Z, final.C);
        LOG_INF("===========================");
        LOG_INF("Memory:");
        LOG_INF("mem[%5u]: %3u", 2637, app->nes.cpu.mem[2637]);
        LOG_INF("mem[%5u]: %3u", 2638, app->nes.cpu.mem[2638]);
        LOG_INF("mem[%5u]: %3u", 2639, app->nes.cpu.mem[2639]);
        LOG_INF("mem[%5u]: %3u", 2640, app->nes.cpu.mem[2640]);
        LOG_INF("mem[%5u]: %3u", 20428, app->nes.cpu.mem[20428]);
        LOG_INF("Expected:");
        LOG_INF("final[%5u]: %3u", 2637, final.mem[2637]);
        LOG_INF("final[%5u]: %3u", 2638, final.mem[2638]);
        LOG_INF("final[%5u]: %3u", 2639, final.mem[2639]);
        LOG_INF("final[%5u]: %3u", 2640, final.mem[2640]);
        LOG_INF("final[%5u]: %3u", 20428, final.mem[20428]);
        printf("===========================\n\n");
    }
#endif // 0
    memDmp(&app->nes.cpu, MEMSIZE);
    // memDmp(final, MEMSIZE);

    saveConfig(app);
#endif /* PLATFORM_WEB */

    unloadRom(&app->nes);

#ifndef NOVID
    UnloadRenderTexture(app->screen);
#endif // NOVID

#ifdef _WIN32
    KxD_Destroy_Tray();
#endif

    rlImGuiShutdown();
    CloseWindow();

    free(app);
    return 0;
}

void loadRom(nes_t *nes, const char *fileName) {
    Nob_String_Builder sb = {};

    if (!nob_read_entire_file(fileName, &sb))
        goto errorLoadRom;

    nes->rom     = (uint8_t *)sb.items;
    nes->romSize = sb.count;
    LOG_INF("ROM Loaded with success");

    CHECK_ROM_HEADER(nes->rom);
    processRomHeader(nes);
    return;

errorLoadRom:
    nob_sb_free(sb);
    exit(1);
}

void loadRomFromMem(nes_t *nes, const char *fileName) {
#ifndef PLATFORM_WEB
    for (size_t i = 0; i < resources_count; ++i) {
        if (strcmp(fileName, resources[i].file_path) == 0) {
            nes->romSize = resources[i].size;
            LOG_INF("File: \"%s\" (%zu bytes)", fileName, nes->romSize);

            nes->rom = callocWrapper(nes->romSize, uint8_t);
            memcpy(nes->rom, &bundle[resources[i].offset], nes->romSize);
            CHECK_ROM_HEADER(nes->rom);

            processRomHeader(nes);
            return;
        }
    }

    LOG_INF("File \"%s\" not found in bundled assets, trying from disk...", fileName);
#endif
    loadRom(nes, fileName);
    return;
}

void unloadRom(nes_t *nes) {
    if (nes->PRG) {
        free(nes->PRG);
        nes->PRGSize = 0;
    }

    if (nes->CHR) {
        free(nes->CHR);
        nes->CHRSize = 0;
    }

    if (nes->rom) {
        free(nes->rom);
        nes->romSize = 0;
    }

    nes->nes2format = false;
}

void processRomHeader(nes_t *nes) {
    nes->nes2format = (nes->rom[7] & 0x0C) == 0x08;

    uint8_t LSB, MSB;

    // PRG ROM
    LSB = nes->rom[4];
    MSB = (nes->rom[9] & 0x0F); // 00003210

    if (MSB == 0x0F) {
        uint8_t mul = LSB & 0b00000011;
        uint8_t exp = LSB & 0b11111100;

        nes->PRGSize = (2 ^ exp * (mul * 2 + 1)) * KB(16); // Multipling result by 16 KiB in bytes
    } else {
        nes->PRGSize = (LSB + (MSB << 8)) * KB(16); // Multipling result by 16 KiB in bytes
    }
    // TODO: Check for other types of hardware
    // CHR ROM/RAM?
    LSB = nes->rom[5];
    MSB = nes->rom[9] >> 4; // 76540000 -> 00007654

    if (MSB == 0x0F) {
        uint8_t mul = LSB & 0b00000011;
        uint8_t exp = LSB & 0b11111100;

        nes->CHRSize = (2 ^ exp * (mul * 2 + 1)) * KB(8); // Multipling result by 8 KiB in bytes
    } else {
        nes->CHRSize = (LSB + (MSB << 8)) * KB(8); // Multipling result by 8 KiB in bytes
    }

    // Allocating

    LOG_INF("Allocating %zu bytes for PRG-ROM...", nes->PRGSize);
    nes->PRG = callocWrapper(nes->PRGSize, uint8_t);

    LOG_INF("Allocating %zu bytes for CHR-ROM...", nes->CHRSize);
    nes->CHR = callocWrapper(nes->CHRSize, uint8_t);
}

void mainLoop(void *app_ptr) {
    app_t *app = app_ptr;
#ifndef PLATFORM_WEB
    while (!WindowShouldClose() && !app->quit) {
#endif // PLATFORM_WEB
        KxD_Handle_Tray();

        if (IsWindowResized()) {
            LOG_INF("Window Resized...");

            app->screenW = GetRenderWidth();
            app->screenH = GetRenderHeight();
            LOG_INF("New Size: " V2_CFMT("%" PRIu16), app->screenW, app->screenH);
            calcScreenPos(app);
            LOG_INF("app->destRec: " RECT_FMT, RECT_ARGS(app->destRec));
        }

        // registerInput(&app->nes);
        if (IsKeyPressed(app->config.pauseKey)) {
            app->nes.isPaused = !app->nes.isPaused;
        }

        if (!app->nes.isPaused) {
            processInstruction(&app->nes.cpu);
            if (app->nes.cpu.B)
                return;
        }

        BeginDrawing();

        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

#ifndef NOVID
        DrawTexturePro(app->screen.texture, app->sourceRec, app->destRec, Vector2Zero(), 0, WHITE);
#endif

        DrawLine(0, (app->screenH / 2), app->screenW, (app->screenH / 2), GREEN);
        DrawLine((app->screenW / 2), 0, (app->screenW / 2), app->screenH, GREEN);

        DrawRectangle(4, 4 + MENU_BAR_SIZE, 75, 20, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
        DrawFPS(5, 5 + MENU_BAR_SIZE);

        if (app->nes.isPaused) {
            const Font font         = GuiGetFont();
            const Vector2 pauseSize = MeasureTextEx(font, app->lang.text_paused, app->screenW * .1f, font.baseSize);

            DrawRectangle((app->screenW * .5f) - (pauseSize.x * .6f), (app->screenH * .5f) - (pauseSize.y * .6f),
                          pauseSize.x + (pauseSize.x * .2f), pauseSize.y + (pauseSize.y * .2f),
                          GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

            DrawRectangleLinesEx(
                CLITERAL(Rectangle){(app->screenW * .5f) - (pauseSize.x * .6f) - 1,
                                    (app->screenH * .5f) - (pauseSize.y * .6f), pauseSize.x + (pauseSize.x * .2f) + 1,
                                    pauseSize.y + (pauseSize.y * .2f)},
                (pauseSize.y + (pauseSize.y * .2f) + 1) * .05f, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));

            // DrawText(app->lang.text_paused, (app->screenW * .5f) - (pauseSize.x * .5f),
            //          (app->screenH * .5f) - (pauseSize.y * .5f), app->screenW * .1f,
            //          GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));

            DrawTextPro(font, app->lang.text_paused,
                        V2((app->screenW * .5f) - (pauseSize.x * .5f), (app->screenH * .5f) - (pauseSize.y * .5f)),
                        Vector2Zero(), 0, app->screenW * .1f, font.baseSize,
                        GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));
        }

#ifdef KXD_DEBUG
        if (IsKeyPressed(KEY_J)) {
            app->menu.openMenu = !app->menu.openMenu;
        }
        // app->menu.openFile = true;

        if (app->menu.openMenu) {
            app->menu.openFile = IsKeyPressed(KEY_O);
#ifndef PLATFORM_WEB
            if (app->menu.openFile) {
                static char *selectedFile          = NULL;
                static const char *const filters[] = {"*.nes"};

                selectedFile = tinyfd_openFileDialog("Open...", ".\\", NOB_ARRAY_LEN(filters), filters,
                                                     "NES ROM File (*.nes)", false);

                if (selectedFile)
                    tinyfd_messageBox("Selected File...", selectedFile, "ok", "info", 0);
                else
                    tinyfd_messageBox("Error...", "No File Selected", "ok", "error", 0);

                app->menu.openFile = false;
            }
#endif // !PLATFORM_WEB
            KxDGui(app);
        }
#endif // KXD_DEBUG
        rlImGuiBegin();
        {
            // static bool opt_open = true;
            // static bool opt_quit = false;

            if (igBeginMainMenuBar()) {
                if (igBeginMenu(app->lang.menu_file, true)) {
                    if (igMenuItemEx(app->lang.menu_file_open, NULL, "Ctrl+O", NULL, true)) {
                        LOG_INF("Clicked: %s", app->lang.menu_file_open);
                    }
                    if (igMenuItem_BoolPtr(app->lang.menu_file_quit, "Alt+F4", NULL, true)) {
                        LOG_INF("Clicked: %s", app->lang.menu_file_quit);
                    }
                    igEndMenu();
                }
                igEndMainMenuBar();
            }

            static bool show_demo_window = true;

            if (show_demo_window)
                igShowDemoWindow(&show_demo_window);

            static bool show_another_window = true;
            static float f                  = 0.0f;
            static int counter              = 0;

            igBegin("Hello, world!", NULL, 0);

            // Edit 1 float using a slider from 0.0f to 1.0f
            igSliderFloat("float", &f, 0.0f, 1.0f, NULL, 0);

            // Buttons return true when clicked (most widgets return true when edited/activated)
            if (igSmallButton("Button"))
                counter++;

            igSameLine(0, 5);
            igText("counter = %d", counter);

            if (igSmallButton("show_another_window"))
                show_another_window = true;

            igEnd();

            if (show_another_window) {
                // Pass a pointer to our bool variable
                // (the window will have a closing button that will clear the bool when clicked)
                igBegin("Another Window", &show_another_window, 0);
                igText("Hello from another window!");
                if (igSmallButton("Close Me"))
                    show_another_window = false;
                igEnd();
            }
        }

        rlImGuiEnd();
        EndDrawing();
#ifndef PLATFORM_WEB
    }
#endif // PLATFORM_WEB
}

void calcScreenPos(app_t *app) {
    if (app->screenW >= ((app->screenH - MENU_BAR_SIZE) * NES_AR)) {
        app->destRec.width  = (app->screenH - MENU_BAR_SIZE) * NES_AR;
        app->destRec.height = app->destRec.width * (1 / NES_AR);
        app->destRec.x      = (app->screenW * .5f) - (app->destRec.width * .5f);
        app->destRec.y      = MENU_BAR_SIZE;
        if (app->destRec.x < 0) {
            app->destRec.x = 0;
        }
    } else {
        app->destRec.height = app->screenW * (1 / NES_AR);
        app->destRec.width  = app->destRec.height * NES_AR;
        app->destRec.x      = 0;
        app->destRec.y      = (app->screenH * .5f) - (app->destRec.height * .5f) + (MENU_BAR_SIZE * .5f);
        if (app->destRec.y < 0) {
            app->destRec.y = 0;
        }
    }
}

void loadIconFile(void) {
    Image img = {};

    img.data   = (void *)icoImg;
    img.width  = icoWid;
    img.height = icoHei;
    img.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;

    SetWindowIcon(img);
}

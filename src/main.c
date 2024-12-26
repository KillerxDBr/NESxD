#include "main.h"

const char *PausedText = "Paused...";

#ifdef C3_EXPORT
extern void c3_teste(void);
#endif // C3_EXPORT

int main(int argc, char **argv) {
#if defined(_WIN32)
    if (!WinH_SetConsoleOutputCP(CP_UTF8)) {
        LOG_ERR("Could not set console output to 'UTF-8'");
        return 1;
    }

    // Should be Win10+ only, but methods to detect windows versions are unreliable...
    LOG_INF("Enabling buffer on console std outputs");

    // on Windows 10+ we need buffering or console will get 1 byte at a time (screwing up utf-8 encoding)
    if (setvbuf(stderr, NULL, _IOFBF, 1024) != 0) {
        LOG_ERR("Could not set \"%s\" buffer to %d: %s", "stderr", 1024, strerror(errno));
        return 1;
    }

    if (setvbuf(stdout, NULL, _IOFBF, 1024) != 0) {
        LOG_ERR("Could not set \"%s\" buffer to %d: %s", "stdout", 1024, strerror(errno));
        return 1;
    }

#ifdef _UCRT // ucrtbase.dll Windows
    setlocale(LC_ALL, ".UTF-8");
#else  // msvcrt.dll Windows
    setlocale(LC_ALL, "C");
#endif // _UCRT

#else // Non Windows
#ifdef PLATFORM_WEB
    setlocale(LC_ALL, ""); // Web version
#else
    setlocale(LC_ALL, "C.UTF-8");
#endif // PLATFORM_WEB

#endif // defined(_WIN32)

    LOG_INF("Locale set to \"%s\"", setlocale(LC_ALL, NULL));

    bool NOP = false;
    bool TEST = false;

    app_t *app = callocWrapper(1, app_t);
    app->program = nob_shift(argv, argc);

    initCPU(&app->nes.cpu);

#ifndef PLATFORM_WEB
    for (int i = 0; i < argc; i++) {
        NOP = (strcmp(argv[i], NOP_CMD) == 0) || NOP;
        TEST = (strcmp(argv[i], TEST_CMD) == 0) || TEST;
        if (NOP && TEST)
            break;
    }
#else
    NOP = true;
#endif /* PLATFORM_WEB */

    app->nes.isPaused = NOP;

#ifndef PLATFORM_WEB

    app->config.fileName = callocWrapper(strlen(app->program) + sizeof(CONFIG_FILE), char);
    strcpy(app->config.fileName, app->program);

    const char *exeName = nob_path_name(app->config.fileName);
    // char *slash = strrchr(app->config.fileName, '\\');
    // assert(slash != NULL);

    // slash++;

    strcpy((char *)exeName, CONFIG_FILE);
    app->config.fileName = realloc(app->config.fileName, strlen(app->config.fileName) + 1);

    LOG_INF("app->config.fileName: '%s'", app->config.fileName);
#else
    NOB_UNUSED(program);
#endif
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow((NES_W * FACTOR), (NES_H * FACTOR) + MENU_BAR_SIZE, "NES_xD");

#ifndef PLATFORM_WEB
    SetWindowMinSize(NES_W, NES_H + MENU_BAR_SIZE);
    SetTargetFPS(60);
#endif /* PLATFORM_WEB */

    app->screenW = GetRenderWidth();
    app->screenH = GetRenderHeight();
    LOG_INF("Screen Width:  %zu", app->screenW);
    LOG_INF("Screen Heigth: %zu", app->screenH);

#ifdef _WIN32
    KxD_Create_Tray(GetWindowHandle());
#endif

    // GuiLoadStyleDefault();

#ifndef NOVID
    SetRandomSeed(40028922U);

    int *seq = LoadRandomSequence(NES_W * NES_H, 0, 0xFFFFFF);
    for (size_t i = 0; i < NES_W * NES_H; ++i)
        seq[i] = (seq[i] << 8) + 0xFF;

    app->screen = LoadRenderTexture(NES_W, NES_H);

    app->sourceRec = CLITERAL(Rectangle){ 0.0f, 0.0f, (float)app->screen.texture.width, -(float)app->screen.texture.height };
    app->destRec = CLITERAL(Rectangle){ 0.0f, MENU_BAR_SIZE, app->screenW, app->screenH - MENU_BAR_SIZE };

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
        DrawPixel(x, app->screen.texture.height - 1, CLITERAL(Color){ 255, 0, 0, 255 });
        DrawPixel(x, 0, CLITERAL(Color){ 0, 255, 0, 255 });
    }
    for (int y = 0; y < app->screen.texture.height; y++) {
        DrawPixel(0, y, CLITERAL(Color){ 0, 0, 255, 255 });
        DrawPixel(app->screen.texture.width - 1, y, CLITERAL(Color){ 255, 255, 0, 255 });
    }

    EndTextureMode();

    UnloadRandomSequence(seq);
#endif /* NOVID */

#ifndef PLATFORM_WEB
    cpu_t final = { 0 };
#endif

#if defined(KXD_DEBUG)
    if (TEST) {
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

        if (!NOP) {
            Nob_String_Builder memory = { 0 };

            if (nob_read_entire_file(memBinPath, &memory)) {
                LOG_INF("Reading %zu instructions from \"%s\"", memory.count, memBinPath);
                addMultipleToMem(app->nes.cpu.mem, 0, (uint8_t *)memory.items, memory.count);
            } else {
                LOG_INF("Operating with \"INS_NOP\" only");
                NOP = true;
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

#ifdef C3_EXPORT
    c3_teste(); // c3c.exe static-lib .\libc3teste.c3 --target mingw-x64
#endif          // C3_EXPORT

    loadDefaultLang(&app->lang);
    initGui(app);
    rlImGuiSetup(true);

    ImGuiIO *io = igGetIO();
    // io->ConfigFlags
    io->IniFilename = NULL;

#ifdef PLATFORM_WEB
    emscripten_set_main_loop_arg(mainLoop, app, 60, 1);
#else
    while (!WindowShouldClose() && !app->quit) {
        mainLoop(app);
    }
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
    Nob_String_Builder sb = { 0 };

    if (!nob_read_entire_file(fileName, &sb))
        goto errorLoadRom;

    nes->rom = (uint8_t *)sb.items;
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

    LOG_INF("Allocating %d bytes for PRG-ROM...", nes->PRGSize);
    nes->PRG = callocWrapper(nes->PRGSize, uint8_t);

    LOG_INF("Allocating %d bytes for CHR-ROM...", nes->CHRSize);
    nes->CHR = callocWrapper(nes->CHRSize, uint8_t);
}

void mainLoop(void *app_ptr) {
    app_t *app = app_ptr;
    // while (!WindowShouldClose() && !app->quit) {
    KxD_Handle_Tray();

    if (IsWindowResized()) {
        LOG_INF("Window Resized...");

        app->screenW = GetRenderWidth();
        app->screenH = GetRenderHeight();
        LOG_INF("New Size: " V2_CFMT("%zu"), app->screenW, app->screenH);
        calcScreenPos(app);
        LOG_INF("app->destRec: " RECT_FMT, RECT_ARGS(app->destRec));
    }

    // registerInput(&app->nes);

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
        const Font font = GuiGetFont();
        const Vector2 pauseSize = MeasureTextEx(font, PausedText, app->screenW * .1f, font.baseSize);

        DrawRectangle((app->screenW * .5f) - (pauseSize.x * .6f), (app->screenH * .5f) - (pauseSize.y * .6f),
                      pauseSize.x + (pauseSize.x * .2f), pauseSize.y + (pauseSize.y * .2f),
                      GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        DrawRectangleLinesEx(CLITERAL(Rectangle){ (app->screenW * .5f) - (pauseSize.x * .6f) - 1,
                                                  (app->screenH * .5f) - (pauseSize.y * .6f), pauseSize.x + (pauseSize.x * .2f) + 1,
                                                  pauseSize.y + (pauseSize.y * .2f) },
                             (pauseSize.y + (pauseSize.y * .2f) + 1) * .05f, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));

        // DrawText(PausedText, (app->screenW * .5f) - (pauseSize.x * .5f), (app->screenH * .5f) - (pauseSize.y * .5f), app->screenW *
        // .1f,
        //          GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));

        DrawTextPro(font, PausedText, V2((app->screenW * .5f) - (pauseSize.x * .5f), (app->screenH * .5f) - (pauseSize.y * .5f)),
                    Vector2Zero(), 0, app->screenW * .1f, font.baseSize, GetColor(GuiGetStyle(DEFAULT, TEXT_COLOR_NORMAL)));
    }

#ifdef KXD_DEBUG
    if (IsKeyPressed(KEY_J)) {
        app->menu.openMenu = !app->menu.openMenu;
    }
    // app->menu.openFile = true;

    // if (app->menu.openMenu) {
    //     // char *selectedFile = NULL;

    //     // const char *filters[1] = { "*.nes" };

    //     // selectedFile = tinyfd_openFileDialog("Open...", ".\\", 1, filters, "NES ROM File (*.nes)", false);

    //     // if (selectedFile) {
    //     //     tinyfd_messageBox("Selected File...", selectedFile, "ok", "info", 0);
    //     // // tinyfd_notifyPopupW(L"Selected File...", tinyfd_utf8to16(selectedFile), L"info");
    //     // }
    //     // else {
    //     //     tinyfd_messageBox("Error...", "No File Selected", "ok", "error", 0);
    //     // }
    //     // app->menu.openFile = false;
    //     KxDGui(app);
    // }
#endif
    rlImGuiBegin();
    {
        static bool opt_open = true;
        static bool opt_quit = false;

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
        static float f = 0.0f;
        static int counter = 0;

        igBegin("Hello, world!", NULL, 0);

        igSliderFloat("float", &f, 0.0f, 1.0f, NULL, 0); // Edit 1 float using a slider from 0.0f to 1.0f

        if (igSmallButton("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;

        igSameLine(0, 5);
        igText("counter = %d", counter);

        if (igSmallButton("show_another_window"))
            show_another_window = true;

        igEnd();

        if (show_another_window) {
            igBegin("Another Window", &show_another_window,
                    0); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            igText("Hello from another window!");
            if (igSmallButton("Close Me"))
                show_another_window = false;
            igEnd();
        }
    }

    rlImGuiEnd();
    EndDrawing();
    if (IsKeyPressed(app->config.pauseKey)) {
        app->nes.isPaused = !app->nes.isPaused;
    }

    if (!app->nes.isPaused) {
        processInstruction(&app->nes.cpu);
        if (app->nes.cpu.B)
            return;
    }
    // }
}

void calcScreenPos(app_t *app) {
    if (app->screenW >= ((app->screenH - MENU_BAR_SIZE) * NES_AR)) {
        app->destRec.width = (app->screenH - MENU_BAR_SIZE) * NES_AR;
        app->destRec.height = app->destRec.width * (1 / NES_AR);
        app->destRec.x = (app->screenW * .5f) - (app->destRec.width * .5f);
        app->destRec.y = MENU_BAR_SIZE;
        if (app->destRec.x < 0) {
            app->destRec.x = 0;
        }
    } else {
        app->destRec.height = app->screenW * (1 / NES_AR);
        app->destRec.width = app->destRec.height * NES_AR;
        app->destRec.x = 0;
        app->destRec.y = (app->screenH * .5f) - (app->destRec.height * .5f) + (MENU_BAR_SIZE * .5f);
        if (app->destRec.y < 0) {
            app->destRec.y = 0;
        }
    }
}

#include "main.h"

int main(void) {
    app_t *app = callocWrapper(1, sizeof(app_t));

    InitWindow(NESW * PX_SZ, NESH * PX_SZ, "NES_xD");
    SetTargetFPS(60);

#ifndef NOVID
    SetRandomSeed(40028922U);
    RenderTexture2D screen = LoadRenderTexture(NESW * PX_SZ, NESH * PX_SZ);

    int *seq = LoadRandomSequence(NESW * NESH, 0, 0xFFFFFF);
    for (size_t i = 0; i < NESW * NESH; ++i)
        seq[i] = (seq[i] << 8) + 0xFF;

    BeginTextureMode(screen);
    for (int x = 0; x < NESW; ++x) {
        for (int y = 0; y < NESH; ++y) {
            DrawRectangle(x * PX_SZ, y * PX_SZ, PX_SZ, PX_SZ, GetColor(seq[XY2Index(x, y, NESW)]));
        }
    }
    EndTextureMode();

    UnloadRandomSequence(seq);
#endif

    memset(app->nes.cpu.mem, INS_NOP, MEMSIZE);

    const char *memBinPath = "./mem.bin";
    size_t insSize = 0;
    FILE *memBin = fopen(memBinPath, "rb");

    if (memBin == NULL) {
        LOG_ERR("Could not open \"%s\": %s", memBinPath, strerror(errno));
        exit(1);
    }

    fseek(memBin, 0, SEEK_END);
    insSize = ftell(memBin);
    rewind(memBin);

    if (insSize == 0) {
        LOG_ERR("No bytes to read from \"%s\", exiting...", memBinPath);
        exit(1);
    }

    uint8_t *instructions = callocWrapper(insSize, sizeof(uint8_t));

    LOG_INF("Reading %zu instructions from \"%s\"", insSize, memBinPath);
    fread(instructions, 1, insSize, memBin);
    fclose(memBin);

    addMultipleToMem(app->nes.cpu.mem, 0, instructions, insSize);
    addToMem(app->nes.cpu.mem, 0xDD, 0x0101);
    free(instructions);

    const char *fileName = "./rom/smb.nes";
    loadRomFromMem(&app->nes, fileName);

    while (!WindowShouldClose() && !app->quit) {
        // SetRandomSeed(0);

        BeginDrawing();

#ifndef NOVID
        DrawTexture(screen.texture, 0, 0, WHITE);
#else
        ClearBackground(KXDBG);
#endif

        DrawRectangle(4, 4, 75, 20, KXDBG);
        DrawFPS(5, 5);
        EndDrawing();
        processInstruction(&app->nes.cpu);
        if (app->nes.cpu.B)
            break;
    }
    memDmp(&app->nes.cpu, MEMSIZE);
#ifndef NOVID
    UnloadRenderTexture(screen);
#endif
    unloadRom(&app->nes);
    free(app);
    return 0;
}

void memDmp(cpu_t *cpu, size_t memSize) {
    FILE *f = fopen("memDmp.log", "wt");
    fprintf(f, "Registers\nPC: 0x%04X | SP: 0x%02X | A: 0x%02X | X: 0x%02X | Y: 0x%02X\n", cpu->PC, cpu->SP, cpu->A, cpu->X, cpu->Y);
    fprintf(f, "Status Registers:\n");
    fprintf(f, "    CZIDBVN\n");
    fprintf(f, "    %d%d%d%d%d%d%d\n", cpu->C, cpu->Z, cpu->I, cpu->D, cpu->B, cpu->V, cpu->N);

    for (size_t i = 0; i < 85; i++) {
        fputc('_', f);
    }
    fprintf(f, "\n");

    fprintf(f, "----  ");
    for (int i = 0; i < 16; ++i) {
        fprintf(f, "%04X ", i);
    }
    fprintf(f, "\n0000: ");
    for (size_t i = 0; i < memSize; ++i) {
        if (i > 0 && i % 16 == 0) {
            fprintf(f, "\n%04zX: ", i);
        }
        fprintf(f, "0x%02X ", cpu->mem[i]);
    }
    fprintf(f, "\n");
    fclose(f);
}

void addToMem(uint8_t *mem, size_t loc, uint64_t value) {
    size_t valueSize = 0;

    if (value <= UINT8_MAX)
        valueSize = sizeof(uint8_t);
    else if (value <= UINT16_MAX)
        valueSize = sizeof(uint16_t);
    else if (value <= UINT32_MAX)
        valueSize = sizeof(uint32_t);
    else
        valueSize = sizeof(uint64_t);
    if (valueSize == 0)
        return;
    assert(loc + valueSize < MEMSIZE);
    for (size_t i = 0; i < valueSize; i++) {
        mem[loc + i] = (value & 0xFF);
        value = value >> 8;
    }
}

void addMultipleToMem(uint8_t *mem, size_t loc, uint8_t *values, size_t valuesSize) {
    if (valuesSize == 0)
        return;
    if (valuesSize == 1) {
        addToMem(mem, loc, values[0]);
        return;
    }
    assert(loc + valuesSize < MEMSIZE);
    for (size_t i = 0; i < valuesSize; i++) {
        mem[loc + i] = values[i];
    }
}

void loadRom(nes_t *nes, const char *fileName) {
    FILE *rom = fopen(fileName, "rb");

    if (rom == NULL) {
        LOG_ERR("Could not open \"%s\": %s", fileName, strerror(errno));
        exit(1);
    }

#ifdef KXD_DEBUG
    LOG_INF("ROM Loaded with success");
#endif

    fseek(rom, 0, SEEK_END);
    nes->romSize = ftell(rom);
    rewind(rom);

    nes->rom = callocWrapper(nes->romSize, 1);
    fread(nes->rom, 1, nes->romSize, rom);
    fclose(rom);

    CHECK_ROM_HEADER(nes->rom);
    processRomHeader(nes);
}

void loadRomFromMem(nes_t *nes, const char *fileName) {
    for (size_t i = 0; i < resources_count; ++i) {
        if (strcmp(fileName, resources[i].file_path) == 0) {
            nes->romSize = resources[i].size;
#ifdef KXD_DEBUG
            LOG_INF("File: \"%s\" (%zu bytes)", fileName, nes->romSize);
#endif
            nes->rom = callocWrapper(nes->romSize, 1);
            memcpy(nes->rom, &bundle[resources[i].offset], nes->romSize);
            CHECK_ROM_HEADER(nes->rom);

            processRomHeader(nes);

            return;
        }
    }

    LOG_INF("File \"%s\" not found in bundled assets, trying from disk...", fileName);
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

        nes->PRGSize = (2 ^ exp * (mul * 2 + 1)) * 16384; // Multipling result by 16 KiB in bytes
    } else {
        nes->PRGSize = (LSB + (MSB << 8)) * 16384; // Multipling result by 16 KiB in bytes
    }
    // TODO: Check for other types of hardware
    // CHR ROM/RAM?
    LSB = nes->rom[5];
    MSB = nes->rom[9] >> 4; // 76540000 -> 00007654

    if (MSB == 0x0F) {
        uint8_t mul = LSB & 0b00000011;
        uint8_t exp = LSB & 0b11111100;

        nes->CHRSize = (2 ^ exp * (mul * 2 + 1)) * 8192; // Multipling result by 8 KiB in bytes
    } else {
        nes->CHRSize = (LSB + (MSB << 8)) * 8192; // Multipling result by 8 KiB in bytes
    }

    // Allocating

#ifdef KXD_DEBUG
    LOG_INF("Allocating %d bytes for PRG-ROM...", nes->PRGSize);
#endif
    nes->PRG = callocWrapper(1, nes->PRGSize);
#ifdef KXD_DEBUG
    LOG_INF("Allocating %d bytes for CHR-ROM...", nes->CHRSize);
#endif
    nes->CHR = callocWrapper(1, nes->CHRSize);

#if 0
    unloadRom(nes);
    exit(0);
#endif
}

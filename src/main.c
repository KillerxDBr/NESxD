#include "main.h"

int main(void) {
    app_t *app = callocWrapper(1, sizeof(app_t));

    InitWindow(NESW * PX_SZ, NESH * PX_SZ, "NES_xD");
    SetTargetFPS(60);

#ifdef NOVID
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

    uint8_t instructions[] = { INS_LDA_INX, 0x34, INS_BRK };
    addToMem(app->nes.cpu.mem, 0x34, 0x0115);
    // uint8_t instructions[] = { INS_LDX_IM, 0xEA, 0x8E, 0xD0, 0x07, 0xA0, 0x03, 0xB1, 0x06 };
    // uint8_t instructions[] = { 0x20, 0xCD, 0x07 };
    addMultipleToMem(app->nes.cpu.mem, 0, instructions, sizeof(instructions));

    app->nes.cpu.Y = 0x1;
    addToMem(app->nes.cpu.mem, 0x0116, 0xAE);

    const char *fileName = "./rom/smb.nes";
    loadRomFromMem(&app->nes, fileName);

    while (!WindowShouldClose() && !app->quit) {
        // SetRandomSeed(0);

        BeginDrawing();

#ifdef NOVID
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
        // break;
    }
    memDmp(&app->nes.cpu, MEMSIZE);
#ifdef NOVID
    UnloadRenderTexture(screen);
#endif
    free(app->nes.rom);
    free(app);
    return 0;
}

void *callocWrapper(size_t n, size_t sz) {
    void *ptr = calloc(n, sz);
    if (ptr == NULL) {
        fprintf(stderr, "\nCould not allocate memory (%zu bytes), exiting...\n\n", n * sz);
        exit(1);
    }
    return ptr;
}

void processInstruction(cpu_t *cpu) {
    uint16_t value16;
    uint8_t value8;

#ifdef KXD_DEBUG
    uint16_t oldPC = cpu->PC;
#endif
    switch (cpu->mem[cpu->PC]) {
    case INS_BRK:
        cpu->B = true;
        cpu->PC++;
        LOG_INFO("BRK");
        break;
        
    case INS_NOP:
#ifdef KXD_DEBUG
        LOG_INFO("NOP");
#endif
        cpu->PC++;
        break;
        
    case INS_CLC:
        cpu->C = false;
        cpu->PC++;
        break;
        
    case INS_SEC:
        cpu->C = true;
        cpu->PC++;
        break;
        
    case INS_LDA_IM: // Immediate mode, Load the next byte to A
        cpu->A = cpu->mem[++cpu->PC];
        LDA_FLAGS;
        cpu->PC++;
        break;
        
    case INS_LDA_Z: // Zero Page Mode, Use next byte as Index to load byte in A
        cpu->A = cpu->mem[cpu->mem[++cpu->PC]];
        LDA_FLAGS;
        cpu->PC++;
        break;
        
    case INS_LDA_ZX:
        cpu->A = cpu->mem[cpu->mem[++cpu->PC] + cpu->X];
        LDA_FLAGS;
        cpu->PC++;
        break;
        
    case INS_LDA_A:
        cpu->A = cpu->mem[cpu->mem[++cpu->PC] + (cpu->mem[++cpu->PC] << 8)];
        LDA_FLAGS;
        cpu->PC++;
        break;
        
    case INS_LDA_AX:
        cpu->A = cpu->mem[cpu->mem[++cpu->PC] + (cpu->mem[++cpu->PC] << 8) + cpu->X];
        LDA_FLAGS;
        cpu->PC++;
        break;
        
    case INS_LDA_AY:
        cpu->A = cpu->mem[cpu->mem[++cpu->PC] + (cpu->mem[++cpu->PC] << 8) + cpu->Y];
        LDA_FLAGS;
        cpu->PC++;
        break;
        
    case INS_LDA_INX:
        value16 = cpu->mem[++cpu->PC];
        VARLOG(value16, "0x%X");
        // exit(0);
        // assert(0 && "Not Implemented");
        LDA_FLAGS;
        cpu->PC++;
        break;
        
    case INS_LDA_INY:
        cpu->A = cpu->mem[cpu->mem[cpu->mem[++cpu->PC]] + (cpu->mem[cpu->mem[cpu->PC] + 1] << 8) + cpu->Y];
        VARLOG(cpu->A, "0x%02X");
        LDA_FLAGS;
        cpu->PC++;
        break;
        
    case INS_LDX_IM:
        cpu->X = cpu->mem[++cpu->PC];
        LDX_FLAGS;
        cpu->PC++;
        break;

    case INS_JSR:
        cpu->PC = cpu->mem[++cpu->PC] + (cpu->mem[++cpu->PC] << 8);
        break;

    default:
        LOG_INFO("Unhandled Instruction: mem[0x%04X] -> 0x%02X", cpu->PC, cpu->mem[cpu->PC]);
        cpu->PC++;
        break;
    }
#ifdef KXD_DEBUG
    debugCPU(cpu);
    LOG_INFO("Cycles used in execution: %u", cpu->PC - oldPC);
#endif
    (void)value16;
    (void)value8;
}
#ifdef KXD_DEBUG
void debugCPU(cpu_t *cpu) {
    LOG_INFO("========================");
    VARLOG(cpu->PC, "0x%04X");
    VARLOG(cpu->SP, "0x%02X");
    VARLOG(cpu->A, " 0x%02X");
    VARLOG(cpu->X, " 0x%02X");
    VARLOG(cpu->Y, " 0x%02X");
    LOG_INFO("Status Registers========");
    VARLOG(cpu->C, " %d");
    VARLOG(cpu->Z, " %d");
    VARLOG(cpu->I, " %d");
    VARLOG(cpu->D, " %d");
    VARLOG(cpu->B, " %d");
    VARLOG(cpu->V, " %d");
    VARLOG(cpu->N, " %d");
    LOG_INFO("Memory==================");
    VARLOG(cpu->mem[cpu->PC], "0x%02X");
    LOG_INFO("");
}
#endif

void memDmp(cpu_t *cpu, size_t memSize) {
    FILE *f = fopen("memDmp.log", "wt");
    fprintf(f, "Registers\nPC: 0x%04X | SP: 0x%02X | A: 0x%02X | X: 0x%02X | Y: 0x%02X\n", cpu->PC, cpu->SP, cpu->A, cpu->X, cpu->Y);
    fprintf(f, "Status Registers:\n");
    fprintf(f, "    CZIDBVN\n");
    fprintf(f, "    %d%d%d%d%d%d%d\n", cpu->C, cpu->Z, cpu->I, cpu->D, cpu->B, cpu->V, cpu->N);
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
    LOG_INFO("ROM Loaded with success");
#endif
    fseek(rom, 0, SEEK_END);
    nes->romSize = ftell(rom);
    rewind(rom);

    nes->rom = callocWrapper(nes->romSize, 1);
    fread(nes->rom, 1, nes->romSize, rom);
    fclose(rom);

    CHECK_ROM_HEADER(nes->rom);
}

void loadRomFromMem(nes_t *nes, const char *fileName) {
    for (size_t i = 0; i < resources_count; ++i) {
        if (strcmp(fileName, resources[i].file_path) == 0) {
            nes->romSize = resources[i].size;
#ifdef KXD_DEBUG
            LOG_INFO("File: '%s' (%zu bytes)", fileName, nes->romSize);
#endif
            nes->rom = callocWrapper(nes->romSize, 1);
            memcpy(nes->rom, &bundle[resources[i].offset], nes->romSize);
            CHECK_ROM_HEADER(nes->rom);
            return;
        }
    }
    LOG_INFO("File '%s' not found in bundled assets, exiting...", fileName);
    exit(1);
}

#include "main.h"

int main(void) {
    app_t *app = callocWrapper(1, sizeof(app_t));

    InitWindow(NESW * PX_SZ, NESH * PX_SZ, "NES_xD");
    SetTargetFPS(60);

    // SetRandomSeed(40028922U);
    // for (size_t i = 0; i < MEMSIZE; ++i) {
    //     app->nes.cpu.mem[i] = GetRandomValue(0, UINT8_MAX);
    // }

    memset(app->nes.cpu.mem, INS_NOP, MEMSIZE);

    app->nes.cpu.mem[0] = INS_LDA_A;

    addToMem(app->nes.cpu.mem, 1, 0x07FD);
    addToMem(app->nes.cpu.mem, 0x07FD, 0xAE);
    addToMem(app->nes.cpu.mem, 7, -1234);

    app->nes.cpu.mem[4] = INS_LDA_IM;
    app->nes.cpu.mem[5] = 0;
    app->nes.cpu.mem[54] = 176;
    // app->nes.cpu.mem[2047] = 0xFF;

    const char *fileName = "./rom/smb.nes";
    loadRomFromMem(&app->nes, fileName);

    while (!WindowShouldClose() && !app->quit) {
        SetRandomSeed(0);

        BeginDrawing();
        ClearBackground(KXDBG);

        for (int x = 0; x < NESW; ++x) {
            for (int y = 0; y < NESH; ++y) {
                DrawRectangle(x * PX_SZ, y * PX_SZ, PX_SZ, PX_SZ,
                              GetColor((GetRandomValue(0, 0xFFFFFF) << 8) + 0xFF));
            }
        }
        DrawRectangle(4, 4, 75, 20, KXDBG);
        DrawFPS(5, 5);
        EndDrawing();
        processInstruction(&app->nes.cpu);
        exit(0);
    }
    // memDmp(app->nes.cpu.mem, MEMSIZE);
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
#ifdef KXD_DEBUG
    uint16_t oldPC = cpu->PC;
#endif
    switch (cpu->mem[cpu->PC]) {
    case INS_BRK:
        cpu->B = true;
        break;
    case INS_NOP:

#ifdef KXD_DEBUG
        LOG("NOP");
#endif

        break;
    case INS_CLC:
        cpu->C = false;
        break;
    case INS_SEC:
        cpu->C = true;
        break;
    case INS_LDA_IM: // Immediate mode, Load the next byte to A
        cpu->A = cpu->mem[++cpu->PC];
        LDA_FLAGS;
        break;
    case INS_LDA_Z: // Zero Page Mode, Use next byte as Index to load byte in A
        cpu->A = cpu->mem[cpu->mem[++cpu->PC]];
        LDA_FLAGS;
        break;
    case INS_LDA_ZX:
        cpu->A = cpu->mem[cpu->mem[++cpu->PC] + cpu->X];
        LDA_FLAGS;
        break;
    case INS_LDA_A:
        uint16_t index = cpu->mem[++cpu->PC] + (cpu->mem[++cpu->PC] << 8);
        cpu->A = cpu->mem[index];
        LDA_FLAGS;
        VARLOG(index, "0x%04X");
        break;
    default:
#ifdef KXD_DEBUG
        LOG("Unhandled Instruction: mem[0x%04X] -> 0x%02X", cpu->PC, cpu->mem[cpu->PC]);
#endif
        break;
    }
    cpu->PC++;
#ifdef KXD_DEBUG
    debugCPU(cpu);
    LOG("Cycles used in execution: %u", cpu->PC - oldPC);
    // exit(0);
#endif
}
#ifdef KXD_DEBUG
void debugCPU(cpu_t *cpu) {
    LOG("========================");
    VARLOG(cpu->PC, "0x%04X");
    VARLOG(cpu->SP, "0x%02X");
    VARLOG(cpu->A, " 0x%02X");
    VARLOG(cpu->X, " 0x%02X");
    VARLOG(cpu->Y, " 0x%02X");
    LOG("Status Registers========");
    VARLOG(cpu->C, " %d");
    VARLOG(cpu->Z, " %d");
    VARLOG(cpu->I, " %d");
    VARLOG(cpu->D, " %d");
    VARLOG(cpu->B, " %d");
    VARLOG(cpu->V, " %d");
    VARLOG(cpu->N, " %d");
    LOG("Memory==================");
    VARLOG(cpu->mem[cpu->PC], "0x%02X");
    LOG("");
}
#endif

void memDmp(uint8_t *mem, size_t memSize) {
    FILE *f = fopen("memDmp.log", "wt");
    // printf("\t");
    fprintf(f, "0000: ");
    for (size_t i = 0; i < memSize; ++i) {
        if (i > 0 && i % 16 == 0) {
            fprintf(f, "\n%04zX: ", i);
        }
        fprintf(f, "0x%02X ", mem[i]);
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
        TraceLog(LOG_ERROR, "Could not open \"%s\": %s", fileName, strerror(errno));
        exit(1);
    } else
        LOG("Sucess");

    fseek(rom, 0, SEEK_END);
    nes->romSize = ftell(rom);
    rewind(rom);
    nes->rom = callocWrapper(nes->romSize, 1);
    fread(nes->rom, 1, nes->romSize, rom);
    fclose(rom);
    assert(nes->rom[0] == 'N' && nes->rom[1] == 'E' && nes->rom[2] == 'S');
}

void loadRomFromMem(nes_t *nes, const char *fileName){
    for (size_t i = 0; i < resources_count; ++i)
    {
        if(strcmp(fileName, resources[i].file_path) == 0){
            nes->romSize = resources[i].size;
            #ifdef KXD_DEBUG
            VARLOG(nes->romSize, "%zu bytes");
            #endif
            nes->rom = callocWrapper(nes->romSize, 1);
            memcpy(nes->rom, &bundle[resources[i].offset], nes->romSize);
            assert(nes->rom[0] == 'N' && nes->rom[1] == 'E' && nes->rom[2] == 'S');
            return;
        }
    }
    LOG("File '%s' not found in bundled assets, exiting...", fileName);
    exit(1);
}
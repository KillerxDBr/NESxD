#include "test.h"

bool InstructionTest(app_t *app, cpu_t *final) {
    bool result           = true;
    Nob_String_Builder sb = {};

    if (!nob_read_entire_file(TEST_BIN, &sb))
        nob_return_defer(false);

    LOG_INF("Bytes read from file \"" TEST_BIN "\": %zu", sb.count);

    assert(sb.count == (sizeof(cpu_t) * 2));

    memcpy(&app->nes.cpu, sb.items, sizeof(cpu_t));
    memcpy(final, &sb.items[sizeof(cpu_t)], sizeof(cpu_t));

#if 0
    LOG_INF("Inicial ------------------");
    LOG_INF("Registers -> PC: 0x%04X(%u) | SP: 0x%02X | A: 0x%02X | X: 0x%02X | Y: 0x%02X", app->nes.cpu.PC, app->nes.cpu.PC,
            app->nes.cpu.SP, app->nes.cpu.A, app->nes.cpu.X, app->nes.cpu.Y);
    LOG_INF("Status Registers:");
    LOG_INF("    NV1B-DIZC");
    LOG_INF("    %d%d%d%d-%d%d%d%d", app->nes.cpu.N, app->nes.cpu.V, 1, app->nes.cpu.B, app->nes.cpu.D, app->nes.cpu.I, app->nes.cpu.Z,
            app->nes.cpu.C);
    for (size_t i = 0; i < MEMSIZE; ++i) {
        if (app->nes.cpu.mem[i] != 0)
            LOG_INF("Memory at 0x%04X (%zu) = %u", i, i, app->nes.cpu.mem[i]);
    }

    LOG_INF("------------------------");
    LOG_INF("Final ------------------");
    LOG_INF("Registers -> PC: 0x%04X(%u) | SP: 0x%02X | A: 0x%02X | X: 0x%02X | Y: 0x%02X", final->PC, final->PC, final->SP, final->A,
            final->X, final->Y);
    LOG_INF("Status Registers:");
    LOG_INF("    NV1B-DIZC");
    LOG_INF("    %d%d%d%d-%d%d%d%d", final->N, final->V, 1, final->B, final->D, final->I, final->Z, final->C);
    for (size_t i = 0; i < MEMSIZE; ++i) {
        if (final->mem[i] != 0)
            LOG_INF("Memory at 0x%04X (%zu) = %u", i, i, final->mem[i]);
    }
#endif

defer:
    if (errno)
        LOG_ERR("%s (%d)", strerror(errno), errno);

    nob_sb_free(sb);

    return result;
}

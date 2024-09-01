#include "6502.h"

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
#ifdef KXD_DEBUG
        LOG_INF("BRK");
#endif
        break;

    case INS_NOP:
#ifdef KXD_DEBUG
        LOG_INF("NOP");
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
#ifdef KXD_DEBUG
        VARLOG(cpu->A, HEX8);
#endif
        LD_FLAGS(cpu->A);
        cpu->PC++;
        break;

    case INS_LDA_Z: // Zero Page Mode, Use next byte as Index to load byte in A
        cpu->A = cpu->mem[cpu->mem[++cpu->PC]];
#ifdef KXD_DEBUG
        VARLOG(cpu->A, HEX8);
#endif
        LD_FLAGS(cpu->A);
        cpu->PC++;
        break;

    case INS_LDA_ZX:
        cpu->A = cpu->mem[cpu->mem[++cpu->PC] + cpu->X];
#ifdef KXD_DEBUG
        VARLOG(cpu->A, HEX8);
#endif
        LD_FLAGS(cpu->A);
        cpu->PC++;
        break;

    case INS_LDA_A:
        cpu->A = cpu->mem[cpu->mem[++cpu->PC] + (cpu->mem[++cpu->PC] << 8)];
#ifdef KXD_DEBUG
        VARLOG(cpu->A, HEX8);
#endif
        LD_FLAGS(cpu->A);
        cpu->PC++;
        break;

    case INS_LDA_AX:
        cpu->A = cpu->mem[cpu->mem[++cpu->PC] + (cpu->mem[++cpu->PC] << 8) + cpu->X];
#ifdef KXD_DEBUG
        VARLOG(cpu->A, HEX8);
#endif
        LD_FLAGS(cpu->A);
        cpu->PC++;
        break;

    case INS_LDA_AY:
        cpu->A = cpu->mem[cpu->mem[++cpu->PC] + (cpu->mem[++cpu->PC] << 8) + cpu->Y];
#ifdef KXD_DEBUG
        VARLOG(cpu->A, HEX8);
#endif
        LD_FLAGS(cpu->A);
        cpu->PC++;
        break;

    case INS_LDA_INX:
        cpu->A = cpu->mem[cpu->mem[cpu->mem[++cpu->PC] + cpu->X] + (cpu->mem[cpu->mem[cpu->PC] + cpu->X + 1] << 8)];
#ifdef KXD_DEBUG
        VARLOG(cpu->A, HEX8);
#endif
        LD_FLAGS(cpu->A);
        cpu->PC++;
        break;

    case INS_LDA_INY:
        cpu->A = cpu->mem[cpu->mem[cpu->mem[++cpu->PC]] + (cpu->mem[cpu->mem[cpu->PC] + 1] << 8) + cpu->Y];
#ifdef KXD_DEBUG
        VARLOG(cpu->A, HEX8);
#endif
        LD_FLAGS(cpu->A);
        cpu->PC++;
        break;

    case INS_STA_A:
        cpu->mem[cpu->mem[++cpu->PC] + (cpu->mem[++cpu->PC] << 8)] = cpu->A;
#ifdef KXD_DEBUG
        VARLOG(cpu->mem[cpu->mem[cpu->PC - 1] + (cpu->mem[cpu->PC] << 8)], HEX8);
#endif
        cpu->PC++;
        break;

    case INS_STA_AX:
        // value16 = cpu->mem[++cpu->PC] + (cpu->mem[++cpu->PC] << 8) + cpu->X;
        // VARLOG(value16, HEX16);
        // VARLOG(value16, "%u");
        cpu->mem[cpu->mem[++cpu->PC] + (cpu->mem[++cpu->PC] << 8) + cpu->X] = cpu->A;
#ifdef KXD_DEBUG
        VARLOG(cpu->mem[cpu->mem[cpu->PC - 1] + (cpu->mem[cpu->PC] << 8) + cpu->X], HEX8);
#endif
        cpu->PC++;
        break;

    case INS_STA_Z:
        cpu->mem[cpu->mem[++cpu->PC]] = cpu->A;
#ifdef KXD_DEBUG
        VARLOG(cpu->mem[cpu->mem[cpu->PC]], HEX8);
#endif
        cpu->PC++;
        break;

    case INS_STA_ZX:
        cpu->mem[cpu->mem[++cpu->PC] + cpu->X] = cpu->A;
#ifdef KXD_DEBUG
        VARLOG(cpu->mem[cpu->mem[cpu->PC] + cpu->X], HEX8);
#endif
        cpu->PC++;
        break;

    case INS_STA_INX:
        value8 = cpu->mem[++cpu->PC] + cpu->X;
        value16 = cpu->mem[value8] + (cpu->mem[value8 + 1] << 8);
#ifdef KXD_DEBUG
        VARLOG(cpu->A, HEX8);
        VARLOG(cpu->X, HEX8);
        VARLOG(value8, HEX8);
        VARLOG(value16, HEX16);
#endif
        cpu->mem[value16] = cpu->A;
        cpu->PC++;
        break;

    case INS_LDX_IM:
        cpu->X = cpu->mem[++cpu->PC];
#ifdef KXD_DEBUG
        VARLOG(cpu->X, HEX8);
#endif
        LD_FLAGS(cpu->X);
        cpu->PC++;
        break;

    case INS_LDX_Z:
        cpu->X = cpu->mem[cpu->mem[++cpu->PC]];
#ifdef KXD_DEBUG
        VARLOG(cpu->X, HEX8);
#endif
        LD_FLAGS(cpu->X);
        cpu->PC++;
        break;

    case INS_LDX_ZY:
        cpu->X = cpu->mem[cpu->mem[++cpu->PC] + cpu->Y];
#ifdef KXD_DEBUG
        VARLOG(cpu->X, HEX8);
#endif
        LD_FLAGS(cpu->X);
        cpu->PC++;
        break;

    case INS_LDX_A:
        cpu->X = cpu->mem[cpu->mem[++cpu->PC] + (cpu->mem[++cpu->PC] << 8)];
#ifdef KXD_DEBUG
        VARLOG(cpu->X, HEX8);
#endif
        LD_FLAGS(cpu->X);
        cpu->PC++;
        break;

    case INS_LDX_AY:
        cpu->X = cpu->mem[cpu->mem[++cpu->PC] + (cpu->mem[++cpu->PC] << 8) + cpu->Y];
#ifdef KXD_DEBUG
        VARLOG(cpu->X, HEX8);
#endif
        LD_FLAGS(cpu->X);
        cpu->PC++;
        break;

    case INS_LDY_IM:
        cpu->Y = cpu->mem[++cpu->PC];
#ifdef KXD_DEBUG
        VARLOG(cpu->Y, HEX8);
#endif
        LD_FLAGS(cpu->Y);
        cpu->PC++;
        break;

    case INS_LDY_Z:
        cpu->Y = cpu->mem[cpu->mem[++cpu->PC]];
#ifdef KXD_DEBUG
        VARLOG(cpu->Y, HEX8);
#endif
        LD_FLAGS(cpu->Y);
        cpu->PC++;
        break;

    case INS_LDY_ZX:
        cpu->Y = cpu->mem[cpu->mem[++cpu->PC] + cpu->X];
#ifdef KXD_DEBUG
        VARLOG(cpu->Y, HEX8);
#endif
        LD_FLAGS(cpu->Y);
        cpu->PC++;
        break;

    case INS_LDY_A:
        cpu->Y = cpu->mem[cpu->mem[++cpu->PC] + (cpu->mem[++cpu->PC] << 8)];
#ifdef KXD_DEBUG
        VARLOG(cpu->Y, HEX8);
#endif
        LD_FLAGS(cpu->Y);
        cpu->PC++;
        break;

    case INS_LDY_AX:
        cpu->Y = cpu->mem[cpu->mem[++cpu->PC] + (cpu->mem[++cpu->PC] << 8) + cpu->X];
#ifdef KXD_DEBUG
        VARLOG(cpu->Y, HEX8);
#endif
        LD_FLAGS(cpu->Y);
        cpu->PC++;
        break;

    case INS_JSR:
        cpu->PC = cpu->mem[++cpu->PC] + (cpu->mem[++cpu->PC] << 8);
        break;

    default:
        LOG_INF("Unhandled Instruction: mem[0x%04X] -> 0x%02X, Skipping...", cpu->PC, cpu->mem[cpu->PC]);
        cpu->PC++;
        break;
    }
#ifdef KXD_DEBUG
    // debugCPU(cpu);
    LOG_INF("Cycles used in execution: %u", cpu->PC - oldPC);
#endif
    (void)value16;
    (void)value8;
}

void debugCPU(cpu_t *cpu) {
#ifdef KXD_DEBUG
    LOG_INF("========================");
    VARLOG(cpu->PC, "0x%04X");
    VARLOG(cpu->SP, "0x%02X");
    VARLOG(cpu->A, " 0x%02X");
    VARLOG(cpu->X, " 0x%02X");
    VARLOG(cpu->Y, " 0x%02X");
    LOG_INF("Status Registers========");
    VARLOG(cpu->C, " %d");
    VARLOG(cpu->Z, " %d");
    VARLOG(cpu->I, " %d");
    VARLOG(cpu->D, " %d");
    VARLOG(cpu->B, " %d");
    VARLOG(cpu->V, " %d");
    VARLOG(cpu->N, " %d");
    LOG_INF("Memory==================");
    VARLOG(cpu->mem[cpu->PC], "0x%02X");
    LOG_INF("");
#else
    (void)cpu;
#endif
}

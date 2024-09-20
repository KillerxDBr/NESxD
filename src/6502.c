#include "6502.h"

void processInstruction(cpu_t *cpu) {
    uint16_t value16;
    uint8_t value8, oldValue;
#ifdef KXD_DEBUG
    uint16_t oldPC = cpu->PC;
#endif
    switch (cpu->mem[cpu->PC]) {
    case INS_BRK:
        cpu->B = true;
        incrementPC(cpu);

        LOG_INF("BRK");

        break;

    case INS_NOP:

        LOG_INF("NOP");

        incrementPC(cpu);
        break;

    case INS_CLC:
        cpu->C = false;
        incrementPC(cpu);
        break;

    case INS_SEC:
        cpu->C = true;
        incrementPC(cpu);
        break;

    case INS_CLI:
        cpu->I = false;
        incrementPC(cpu);
        break;

    case INS_SEI:
        cpu->I = true;
        incrementPC(cpu);
        break;

    case INS_CLV:
        cpu->V = false;
        incrementPC(cpu);
        break;

    case INS_CLD:
        cpu->D = false;
        incrementPC(cpu);
        break;

    case INS_SED:
        cpu->D = true;
        incrementPC(cpu);
        break;

    case INS_ADC_IM:
        oldValue = cpu->A;
        value8 = cpu->mem[incrementPC(cpu)];
        cpu->A += value8;
        ADC_FLAGS(cpu->A, oldValue, value8);
        incrementPC(cpu);
        break;

    case INS_ADC_Z:
        oldValue = cpu->A;
        value8 = cpu->mem[cpu->mem[incrementPC(cpu)]];

        VARLOG(value8, HEX8);

        cpu->A += value8;
        ADC_FLAGS(cpu->A, oldValue, value8);
        incrementPC(cpu);
        break;

    case INS_ADC_ZX:
        assert(0 && "TODO: INS_ADC_ZX");
        oldValue = cpu->A;
        ADC_FLAGS(cpu->A, oldValue, value8);
        incrementPC(cpu);
        break;

    case INS_ADC_A:
        assert(0 && "TODO: INS_ADC_A");
        oldValue = cpu->A;
        ADC_FLAGS(cpu->A, oldValue, value8);
        incrementPC(cpu);
        break;

    case INS_ADC_AX:
        assert(0 && "TODO: INS_ADC_AX");
        oldValue = cpu->A;
        ADC_FLAGS(cpu->A, oldValue, value8);
        incrementPC(cpu);
        break;

    case INS_ADC_AY:
        assert(0 && "TODO: INS_ADC_AY");
        oldValue = cpu->A;
        ADC_FLAGS(cpu->A, oldValue, value8);
        incrementPC(cpu);
        break;

    case INS_ADC_INX:
        assert(0 && "TODO: INS_ADC_INX");
        oldValue = cpu->A;
        ADC_FLAGS(cpu->A, oldValue, value8);
        incrementPC(cpu);
        break;

    case INS_ADC_INY:
        assert(0 && "TODO: INS_ADC_INY");
        oldValue = cpu->A;
        ADC_FLAGS(cpu->A, oldValue, value8);
        incrementPC(cpu);
        break;

    case INS_LDA_IM: // Immediate mode, Load the next byte to A
        cpu->A = cpu->mem[incrementPC(cpu)];

        VARLOG(cpu->A, HEX8);

        LD_FLAGS(cpu->A);
        incrementPC(cpu);
        break;

    case INS_LDA_Z: // Zero Page Mode, Use next byte as Index to load byte in A
        cpu->A = cpu->mem[cpu->mem[incrementPC(cpu)]];

        VARLOG(cpu->A, HEX8);

        LD_FLAGS(cpu->A);
        incrementPC(cpu);
        break;

    case INS_LDA_ZX:
        cpu->A = cpu->mem[cpu->mem[incrementPC(cpu)] + cpu->X];

        VARLOG(cpu->A, HEX8);

        LD_FLAGS(cpu->A);
        incrementPC(cpu);
        break;

    case INS_LDA_A:
        cpu->A = cpu->mem[cpu->mem[incrementPC(cpu)] + (cpu->mem[incrementPC(cpu)] << 8)];

        VARLOG(cpu->A, HEX8);

        LD_FLAGS(cpu->A);
        incrementPC(cpu);
        break;

    case INS_LDA_AX:
        cpu->A = cpu->mem[cpu->mem[incrementPC(cpu)] + (cpu->mem[incrementPC(cpu)] << 8) + cpu->X];

        VARLOG(cpu->A, HEX8);

        LD_FLAGS(cpu->A);
        incrementPC(cpu);
        break;

    case INS_LDA_AY:
        cpu->A = cpu->mem[cpu->mem[incrementPC(cpu)] + (cpu->mem[incrementPC(cpu)] << 8) + cpu->Y];

        VARLOG(cpu->A, HEX8);

        LD_FLAGS(cpu->A);
        incrementPC(cpu);
        break;

    case INS_LDA_INX:
        cpu->A = cpu->mem[cpu->mem[cpu->mem[incrementPC(cpu)] + cpu->X] + (cpu->mem[cpu->mem[cpu->PC] + cpu->X + 1] << 8)];

        VARLOG(cpu->A, HEX8);

        LD_FLAGS(cpu->A);
        incrementPC(cpu);
        break;

    case INS_LDA_INY:
        cpu->A = cpu->mem[cpu->mem[cpu->mem[incrementPC(cpu)]] + (cpu->mem[cpu->mem[cpu->PC] + 1] << 8) + cpu->Y];

        VARLOG(cpu->A, HEX8);

        LD_FLAGS(cpu->A);
        incrementPC(cpu);
        break;

    case INS_STA_A:
        cpu->mem[cpu->mem[incrementPC(cpu)] + (cpu->mem[incrementPC(cpu)] << 8)] = cpu->A;

        VARLOG(cpu->mem[cpu->mem[cpu->PC - 1] + (cpu->mem[cpu->PC] << 8)], HEX8);

        incrementPC(cpu);
        break;

    case INS_STA_AX:
        // value16 = cpu->mem[incrementPC(cpu)] + (cpu->mem[incrementPC(cpu)] << 8) + cpu->X;
        // VARLOG(value16, HEX16);
        // VARLOG(value16, "%u");
        cpu->mem[cpu->mem[incrementPC(cpu)] + (cpu->mem[incrementPC(cpu)] << 8) + cpu->X] = cpu->A;

        VARLOG(cpu->mem[cpu->mem[cpu->PC - 1] + (cpu->mem[cpu->PC] << 8) + cpu->X], HEX8);

        incrementPC(cpu);
        break;

    case INS_STA_Z:
        cpu->mem[cpu->mem[incrementPC(cpu)]] = cpu->A;

        VARLOG(cpu->mem[cpu->mem[cpu->PC]], HEX8);

        incrementPC(cpu);
        break;

    case INS_STA_ZX:
        cpu->mem[cpu->mem[incrementPC(cpu)] + cpu->X] = cpu->A;

        VARLOG(cpu->mem[cpu->mem[cpu->PC] + cpu->X], HEX8);

        incrementPC(cpu);
        break;

    case INS_STA_INX:
        value8 = cpu->mem[incrementPC(cpu)] + cpu->X;
        value16 = cpu->mem[value8] + (cpu->mem[value8 + 1] << 8);

        VARLOG(cpu->A, HEX8);
        VARLOG(cpu->X, HEX8);
        VARLOG(value8, HEX8);
        VARLOG(value16, HEX16);

        cpu->mem[value16] = cpu->A;
        incrementPC(cpu);
        break;

    case INS_STA_INY:
        value8 = cpu->mem[incrementPC(cpu)];
        value16 = cpu->mem[value8] + (cpu->mem[value8 + 1] << 8) + cpu->Y;

        VARLOG(cpu->A, HEX8);
        VARLOG(cpu->Y, HEX8);
        VARLOG(value8, HEX8);
        VARLOG(value16, HEX16);

        cpu->mem[value16] = cpu->A;
        incrementPC(cpu);
        break;

    case INS_LDX_IM:
        cpu->X = cpu->mem[incrementPC(cpu)];

        VARLOG(cpu->X, HEX8);

        LD_FLAGS(cpu->X);
        incrementPC(cpu);
        break;

    case INS_LDX_Z:
        cpu->X = cpu->mem[cpu->mem[incrementPC(cpu)]];

        VARLOG(cpu->X, HEX8);

        LD_FLAGS(cpu->X);
        incrementPC(cpu);
        break;

    case INS_LDX_ZY:
        cpu->X = cpu->mem[cpu->mem[incrementPC(cpu)] + cpu->Y];

        VARLOG(cpu->X, HEX8);

        LD_FLAGS(cpu->X);
        incrementPC(cpu);
        break;

    case INS_LDX_A:
        cpu->X = cpu->mem[cpu->mem[incrementPC(cpu)] + (cpu->mem[incrementPC(cpu)] << 8)];

        VARLOG(cpu->X, HEX8);

        LD_FLAGS(cpu->X);
        incrementPC(cpu);
        break;

    case INS_LDX_AY:
        cpu->X = cpu->mem[cpu->mem[incrementPC(cpu)] + (cpu->mem[incrementPC(cpu)] << 8) + cpu->Y];

        VARLOG(cpu->X, HEX8);

        LD_FLAGS(cpu->X);
        incrementPC(cpu);
        break;

    case INS_LDY_IM:
        cpu->Y = cpu->mem[incrementPC(cpu)];

        VARLOG(cpu->Y, HEX8);

        LD_FLAGS(cpu->Y);
        incrementPC(cpu);
        break;

    case INS_LDY_Z:
        cpu->Y = cpu->mem[cpu->mem[incrementPC(cpu)]];

        VARLOG(cpu->Y, HEX8);

        LD_FLAGS(cpu->Y);
        incrementPC(cpu);
        break;

    case INS_LDY_ZX:
        cpu->Y = cpu->mem[cpu->mem[incrementPC(cpu)] + cpu->X];

        VARLOG(cpu->Y, HEX8);

        LD_FLAGS(cpu->Y);
        incrementPC(cpu);
        break;

    case INS_LDY_A:
        cpu->Y = cpu->mem[cpu->mem[incrementPC(cpu)] + (cpu->mem[incrementPC(cpu)] << 8)];

        VARLOG(cpu->Y, HEX8);

        LD_FLAGS(cpu->Y);
        incrementPC(cpu);
        break;

    case INS_LDY_AX:
        cpu->Y = cpu->mem[cpu->mem[incrementPC(cpu)] + (cpu->mem[incrementPC(cpu)] << 8) + cpu->X];

        VARLOG(cpu->Y, HEX8);

        LD_FLAGS(cpu->Y);
        incrementPC(cpu);
        break;

    case INS_JSR:
        cpu->PC = cpu->mem[incrementPC(cpu)] + (cpu->mem[incrementPC(cpu)] << 8);
        break;

    case INS_DEC_A:
        value16 = cpu->mem[incrementPC(cpu)] + (cpu->mem[incrementPC(cpu)] << 8);
        cpu->mem[value16] -= 1;
        DEC_FLAGS(cpu->mem[value16]);
        // assert(0 && "TODO: DEC A");
        incrementPC(cpu);
        break;

    case INS_DEC_AX:
        assert(0 && "TODO: DEC AX");
        incrementPC(cpu);
        break;

    case INS_DEC_Z:
        value8 = cpu->mem[incrementPC(cpu)];
        value16 = cpu->mem[value8] + (cpu->mem[value8 + 1] << 8);
        cpu->mem[value16] -= 1;
        DEC_FLAGS(cpu->mem[value16]);
        incrementPC(cpu);
        break;

    case INS_DEC_ZX:
        assert(0 && "TODO: DEC ZX");
        incrementPC(cpu);
        break;

    case INS_DEX:
        cpu->X -= 1;
        DEC_FLAGS(cpu->X);
        incrementPC(cpu);
        break;

    case INS_DEY:
        cpu->Y -= 1;
        DEC_FLAGS(cpu->Y);
        incrementPC(cpu);
        break;

    default:
        LOG_INF("Unhandled Instruction: mem[" HEX16 "] -> " HEX8 ", Skipping...", cpu->PC, cpu->mem[cpu->PC]);
        incrementPC(cpu);
        break;
    }
#ifdef KXD_DEBUG
    debugCPU(cpu);
    LOG_INF("Cycles used in execution: %u", cpu->PC - oldPC);
    LOG_INF("------------------------------------------------");
#endif
    //     cpu->PC = cpu->PC & (MEMSIZE - 1);

    (void)value16;
    (void)value8;
    (void)oldValue;
}

void debugCPU(cpu_t *cpu) {

    LOG_INF("========================");
    VARLOG(cpu->PC, HEX16);
    VARLOG(cpu->SP, HEX8);
    VARLOG(cpu->A, " " HEX8);
    VARLOG(cpu->X, " " HEX8);
    VARLOG(cpu->Y, " " HEX8);
    LOG_INF("Status Registers========");
    BOOLLOG(cpu->C);
    BOOLLOG(cpu->Z);
    BOOLLOG(cpu->I);
    BOOLLOG(cpu->D);
    BOOLLOG(cpu->B);
    BOOLLOG(cpu->V);
    BOOLLOG(cpu->N);
    LOG_INF("Memory==================");
    VARLOG(cpu->mem[cpu->PC], "0x%02X");
    LOG_INF("");
}

#include "6502.h"

void processInstruction(cpu_t *cpu) {
    uint16_t value16;
    uint8_t value8, oldValue;
#ifdef KXD_DEBUG
    const uint16_t oldPC = cpu->PC;
#endif
    switch ((KxD_Instructions)cpu->mem[cpu->PC]) { // casting for "-Wswitch-enum" Warnings
    case INS_BRK:
        LOG_INF("BRK");

        cpu->B = true;
        incrementPC(cpu);
        break;

    case INS_NOP:
        LOG_INF("NOP");

        incrementPC(cpu);
        break;

    case INS_CLC:
        cpu->C = false;
        BOOLLOG(cpu->C);
        incrementPC(cpu);
        break;

    case INS_SEC:
        cpu->C = true;
        BOOLLOG(cpu->C);
        incrementPC(cpu);
        break;

    case INS_CLI:
        cpu->I = false;
        BOOLLOG(cpu->I);
        incrementPC(cpu);
        break;

    case INS_SEI:
        cpu->I = true;
        BOOLLOG(cpu->I);
        incrementPC(cpu);
        break;

    case INS_CLV:
        cpu->V = false;
        BOOLLOG(cpu->V);
        incrementPC(cpu);
        break;

    case INS_CLD:
        cpu->D = false;
        BOOLLOG(cpu->D);
        incrementPC(cpu);
        break;

    case INS_SED:
        cpu->D = true;
        BOOLLOG(cpu->V);
        incrementPC(cpu);
        break;

    case INS_ADC_IM:
        oldValue = cpu->A;
        value8   = cpu->mem[incrementPC(cpu)];
        cpu->A += value8;
        ADC_FLAGS(cpu->A, oldValue, value8);
        incrementPC(cpu);
        break;

    case INS_ADC_Z:
        oldValue = cpu->A;
        value8   = cpu->mem[cpu->mem[incrementPC(cpu)]];

        VARLOG(value8, HEX8);

        cpu->A += value8;
        ADC_FLAGS(cpu->A, oldValue, value8);
        incrementPC(cpu);
        break;

    case INS_ADC_ZX:
        oldValue = cpu->A;
        value8   = cpu->mem[cpu->mem[incrementPC(cpu)] + cpu->X];

        VARLOG(value8, HEX8);

        cpu->A += value8;
        ADC_FLAGS(cpu->A, oldValue, value8);
        incrementPC(cpu);
        break;

    case INS_ADC_A:
        oldValue = cpu->A;
        value8   = cpu->mem[cpu->mem[incrementPC(cpu)] + (cpu->mem[incrementPC(cpu)] << 8)];

        VARLOG(value8, HEX8);

        cpu->A += value8;
        ADC_FLAGS(cpu->A, oldValue, value8);
        incrementPC(cpu);
        break;

    case INS_ADC_AX:
        oldValue = cpu->A;
        value8   = cpu->mem[cpu->mem[incrementPC(cpu)] + (cpu->mem[incrementPC(cpu)] << 8) + cpu->X];

        VARLOG(value8, HEX8);

        cpu->A += value8;
        ADC_FLAGS(cpu->A, oldValue, value8);
        incrementPC(cpu);
        break;

    case INS_ADC_AY:
        oldValue = cpu->A;
        value8   = cpu->mem[cpu->mem[incrementPC(cpu)] + (cpu->mem[incrementPC(cpu)] << 8) + cpu->Y];

        VARLOG(value8, HEX8);

        cpu->A += value8;
        ADC_FLAGS(cpu->A, oldValue, value8);
        incrementPC(cpu);
        break;

        // case INS_ADC_INX:
        // KXD_TODO("INS_ADC_INX");
        // oldValue = cpu->A;
        // ADC_FLAGS(cpu->A, oldValue, value8);
        // incrementPC(cpu);
        // break;

        // case INS_ADC_INY:
        // KXD_TODO("INS_ADC_INY");
        // oldValue = cpu->A;
        // ADC_FLAGS(cpu->A, oldValue, value8);
        // incrementPC(cpu);
        // break;

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
        value8 = (cpu->mem[incrementPC(cpu)] + cpu->X) % 256;
        cpu->A = cpu->mem[cpu->mem[value8] + (cpu->mem[(value8 + 1) % 256] << 8)];

        VARLOG(cpu->A, HEX8);

        LD_FLAGS(cpu->A);
        incrementPC(cpu);
        break;

    case INS_LDA_INY:
        value8 = cpu->mem[incrementPC(cpu)];
        cpu->A = cpu->mem[cpu->mem[value8] + (cpu->mem[(value8 + 1) % 256] << 8) + cpu->Y];

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
        value8  = (cpu->mem[incrementPC(cpu)] + cpu->X) % 256;
        value16 = cpu->mem[value8] + (cpu->mem[(value8 + 1) % 256] << 8);

        VARLOG(cpu->A, HEX8);
        VARLOG(cpu->X, HEX8);
        VARLOG(value8, HEX8);
        VARLOG(value16, HEX16);

        cpu->mem[value16] = cpu->A;
        incrementPC(cpu);
        break;

    case INS_STA_INY:
        value8  = cpu->mem[incrementPC(cpu)];
        value16 = cpu->mem[value8] + (cpu->mem[(value8 + 1) % 256] << 8) + cpu->Y;

        VARLOG(cpu->A, HEX8);
        VARLOG(cpu->Y, HEX8);
        VARLOG(value8, HEX8);
        VARLOG(value16, HEX16);

        cpu->mem[value16] = cpu->A;
        incrementPC(cpu);
        break;

        // case INS_STX_A:
        //     KXD_TODO("INS_STX_A");
        //     incrementPC(cpu);
        //     break;

    case INS_STX_Z:
        cpu->mem[cpu->mem[incrementPC(cpu)]] = cpu->X;

        VARLOG(cpu->mem[cpu->mem[cpu->PC]], HEX8);

        incrementPC(cpu);
        break;

    case INS_STX_ZY:
        cpu->mem[cpu->mem[incrementPC(cpu)] + cpu->Y] = cpu->X;

        VARLOG(cpu->mem[cpu->mem[cpu->PC] + cpu->Y], HEX8);

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
        cpu->mem[value16]--;
        DEC_FLAGS(cpu->mem[value16]);
        incrementPC(cpu);
        break;

    case INS_DEC_AX:
        value16 = cpu->mem[incrementPC(cpu)] + (cpu->mem[incrementPC(cpu)] << 8) + cpu->X;
        cpu->mem[value16]--;
        DEC_FLAGS(cpu->mem[value16]);
        incrementPC(cpu);
        break;

    case INS_DEC_Z:
        value8  = cpu->mem[incrementPC(cpu)];
        value16 = cpu->mem[value8] + (cpu->mem[(value8 + 1) % 256] << 8);
        cpu->mem[value16]--;
        DEC_FLAGS(cpu->mem[value16]);
        incrementPC(cpu);
        break;

    case INS_DEC_ZX:
        value8  = cpu->mem[incrementPC(cpu)] + cpu->X;
        value16 = cpu->mem[value8] + (cpu->mem[(value8 + 1) % 256] << 8);
        cpu->mem[value16]--;
        DEC_FLAGS(cpu->mem[value16]);
        incrementPC(cpu);
        break;

    case INS_DEX:
        cpu->X--;
        DEC_FLAGS(cpu->X);
        incrementPC(cpu);
        break;

    case INS_DEY:
        cpu->Y--;
        DEC_FLAGS(cpu->Y);
        incrementPC(cpu);
        break;

    case INS_JMP_A:
        value8  = incrementPC(cpu) & 0xFF;
        cpu->PC = cpu->mem[value8] + (cpu->mem[(value8 + 1) % 256] << 8);
        break;

    case INS_JMP_I:
        value8  = incrementPC(cpu) & 0xFF;
        value16 = (cpu->mem[value8] + (cpu->mem[(value8 + 1) % 256] << 8)) % 65536;
        cpu->PC = cpu->mem[value16] + (cpu->mem[(value16 + 1) % 65536] << 8);
        break;

    default:
#ifdef KXD_DEBUG
        LOG_ERR("Unhandled Instruction: mem[" HEX16 "] -> " HEX8 ", Skipping...", cpu->PC, cpu->mem[cpu->PC]);
        incrementPC(cpu);
#else
        assert(0 && "Unreachable!!!");
#endif
        break;
    }
#ifdef KXD_DEBUG
    debugCPU(cpu);
    LOG_INF("Cycles used in execution: %" PRIu16, cpu->PC - oldPC);
    LOG_INF("------------------------------------------------");
#endif
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
    VARLOG(cpu->mem[cpu->PC], HEX8);
}

void pushToStack(cpu_t *cpu) {
    (void)cpu;
    KXD_TODO("pushToStack");
}

void popFromStack(cpu_t *cpu) {
    (void)cpu;
    KXD_TODO("popFromStack");
}

bool initCPU(cpu_t *cpu) {

    // set Reset Vector
    // addToMem(cpu->mem, RESET_VECTOR, 0x1234);
    writeWord(cpu, 0x1234, RESET_VECTOR);

    cpu->PC = cpu->mem[RESET_VECTOR] + (cpu->mem[RESET_VECTOR + 1] << 8);
    cpu->SP = 0xFF;
    debugCPU(cpu);
    LOG_INF("========================");
    // exit(0);
    return true;
}

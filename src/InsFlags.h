typedef enum {
    INS_BRK = 0x00,   // Break
    INS_NOP = 0xEA,   // No Operation

    // Flag (Processor Status) Instructions
    INS_CLC = 0x18,   // Clear Carry Flag
    INS_SEC = 0x38,   // Set Carry Flag
    INS_CLI = 0x58,   // Clear Interrupt Disable
    INS_SEI = 0x78,   // Set Interrupt Disable
    INS_CLV = 0xB8,   // Clear Overflow Flag
    INS_CLD = 0xD8,   // Clear Decimal Mode
    INS_SED = 0xF8,   // Set Decimal Flag

    // LDA Instructions
    INS_LDA_IM  = 0xA9,   // LDA Immediate
    INS_LDA_Z   = 0xA5,   // LDA Zero Page
    INS_LDA_ZX  = 0xB5,   // LDA Zero Page, X
    INS_LDA_A   = 0xAD,   // LDA Absolute
    INS_LDA_AX  = 0xBD,   // LDA Absolute, X
    INS_LDA_AY  = 0xB9,   // LDA Absolute, Y
    INS_LDA_INX = 0xA1,   // LDA (Indirect, X)
    INS_LDA_INY = 0xB1,   // LDA (Indirect), Y

    // LDX Instructions
    INS_LDX_IM = 0xA2,   // LDX Immediate
    INS_LDX_Z  = 0xA6,   // LDX Zero Page
    INS_LDX_ZY = 0xB6,   // LDX Zero Page, Y
    INS_LDX_A  = 0xAE,   // LDX Absolute
    INS_LDX_AY = 0xBE,   // LDX Absolute, Y

    // LDY Instructions
    INS_LDY_IM = 0xA0,   // LDY Immediate
    INS_LDY_Z  = 0xA4,   // LDY Zero Page
    INS_LDY_ZX = 0xB4,   // LDY Zero Page, X
    INS_LDY_A  = 0xAC,   // LDY Absolute
    INS_LDY_AX = 0xBC,   // LDY Absolute, X
} KxD_Instructions;

#define LDA_FLAGS                                                                                 \
    do {                                                                                          \
        if (cpu->A == 0)                                                                          \
            cpu->Z = true;                                                                        \
        if (cpu->A & 0b10000000)                                                                  \
            cpu->N = true;                                                                        \
    } while (0)

#define LDX_FLAGS                                                                                 \
    do {                                                                                          \
        if (cpu->X == 0)                                                                          \
            cpu->Z = true;                                                                        \
        if (cpu->X & 0b10000000)                                                                  \
            cpu->N = true;                                                                        \
    } while (0)

#define LDY_FLAGS                                                                                 \
    do {                                                                                          \
        if (cpu->Y == 0)                                                                          \
            cpu->Z = true;                                                                        \
        if (cpu->Y & 0b10000000)                                                                  \
            cpu->N = true;                                                                        \
    } while (0)

#pragma once
#ifndef KXD_INSTRUCTIONS_H
#define KXD_INSTRUCTIONS_H

typedef enum {
    // clang-format off
    INS_BRK     = 0x00,   // Break
    INS_NOP     = 0xEA,   // No Operation

    // Flag (Processor Status) Instructions
    INS_CLC     = 0x18,   // Clear Carry Flag
    INS_SEC     = 0x38,   // Set Carry Flag
    INS_CLI     = 0x58,   // Clear Interrupt Disable
    INS_SEI     = 0x78,   // Set Interrupt Disable
    INS_CLV     = 0xB8,   // Clear Overflow Flag
    INS_CLD     = 0xD8,   // Clear Decimal Mode
    INS_SED     = 0xF8,   // Set Decimal Flag

    // ADC Instructions
    INS_ADC_IM  = 0x69,   // ADC Immediate
    INS_ADC_Z   = 0x65,   // ADC Zero Page
    INS_ADC_ZX  = 0x75,   // ADC Zero Page, X
    INS_ADC_A   = 0x6D,   // ADC Absolute
    INS_ADC_AX  = 0x7D,   // ADC Absolute, X
    INS_ADC_AY  = 0x79,   // ADC Absolute, Y
    INS_ADC_INX = 0x61,   // ADC (Indirect, X)
    INS_ADC_INY = 0x71,   // ADC (Indirect), Y

    // AND Instructions
    INS_AND_IM  = 0x29,   // Immediate
    INS_AND_Z   = 0x25,   // Zero Page
    INS_AND_ZX  = 0x35,   // Zero Page,X
    INS_AND_A   = 0x2D,   // Absolute
    INS_AND_AX  = 0x3D,   // Absolute,X
    INS_AND_AY  = 0x39,   // Absolute,Y
    INS_AND_INX = 0x21,   // (Indirect,X)
    INS_AND_INY = 0x31,   // (Indirect),Y

    // LDA Instructions
    INS_LDA_IM  = 0xA9,   // LDA Immediate
    INS_LDA_Z   = 0xA5,   // LDA Zero Page
    INS_LDA_ZX  = 0xB5,   // LDA Zero Page, X
    INS_LDA_A   = 0xAD,   // LDA Absolute
    INS_LDA_AX  = 0xBD,   // LDA Absolute, X
    INS_LDA_AY  = 0xB9,   // LDA Absolute, Y
    INS_LDA_INX = 0xA1,   // LDA (Indirect, X)
    INS_LDA_INY = 0xB1,   // LDA (Indirect), Y

    // STA Instructions
    INS_STA_A   = 0x8D,   // STA Absolute
    INS_STA_AX  = 0x9D,   // STA Absolute, X
    INS_STA_AY  = 0x99,   // STA Absolute, Y
    INS_STA_Z   = 0x85,   // STA Zero Page
    INS_STA_ZX  = 0x95,   // STA Zero Page, X
    INS_STA_INX = 0x81,   // STA (Indirect, X)
    INS_STA_INY = 0x91,   // STA (Indirect), Y

    // STX Instructions
    INS_STX_A   = 0x8E,   // STX Absolute
    INS_STX_Z   = 0x86,   // STX Zero Page
    INS_STX_ZY  = 0x96,   // STX Zero Page, Y

    // LDX Instructions
    INS_LDX_IM  = 0xA2,   // LDX Immediate
    INS_LDX_Z   = 0xA6,   // LDX Zero Page
    INS_LDX_ZY  = 0xB6,   // LDX Zero Page, Y
    INS_LDX_A   = 0xAE,   // LDX Absolute
    INS_LDX_AY  = 0xBE,   // LDX Absolute, Y

    // LDY Instructions
    INS_LDY_IM  = 0xA0,   // LDY Immediate
    INS_LDY_Z   = 0xA4,   // LDY Zero Page
    INS_LDY_ZX  = 0xB4,   // LDY Zero Page, X
    INS_LDY_A   = 0xAC,   // LDY Absolute
    INS_LDY_AX  = 0xBC,   // LDY Absolute, X

    // LSR Instructions
    INS_LSR_ACC = 0x4A,   // LSR Accumulator
    INS_LSR_A   = 0x4E,   // LSR Absolute
    INS_LSR_AX  = 0x5E,   // LSR Absolute, X
    INS_LSR_Z   = 0x46,   // LSR Zero Page
    INS_LSR_ZX  = 0x56,   // LSR Zero Page, X

    // DEC Instructions
    INS_DEC_A   = 0xCE,   // DEC Absolute
    INS_DEC_AX  = 0xDE,   // DEC Absolute, X
    INS_DEC_Z   = 0xC6,   // DEC Zero Page
    INS_DEC_ZX  = 0xD6,   // DEC Zero Page, X

    INS_DEX     = 0xCA,   // DEX Implied
    INS_DEY     = 0x88,   // DEY Implied

    // JSR - Jump to Subroutine
    INS_JSR     = 0x20,

    // JMP Instructions
    INS_JMP_A   = 0x4C,   // JMP Absolute
    INS_JMP_I   = 0x6C,   // JMP Indirect

    // clang-format on
} KxD_Instructions;

#endif /* KXD_INSTRUCTIONS_H */

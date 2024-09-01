import os
from sys import argv

INS_BRK     = 0x00   # Break
INS_NOP     = 0xEA   # No Operation

# Flag (Processor Status) Instructions
INS_CLC     = 0x18   # Clear Carry Flag
INS_SEC     = 0x38   # Set Carry Flag
INS_CLI     = 0x58   # Clear Interrupt Disable
INS_SEI     = 0x78   # Set Interrupt Disable
INS_CLV     = 0xB8   # Clear Overflow Flag
INS_CLD     = 0xD8   # Clear Decimal Mode
INS_SED     = 0xF8   # Set Decimal Flag

# LDA Instructions
INS_LDA_IM  = 0xA9   # LDA Immediate
INS_LDA_Z   = 0xA5   # LDA Zero Page
INS_LDA_ZX  = 0xB5   # LDA Zero Page, X
INS_LDA_A   = 0xAD   # LDA Absolute
INS_LDA_AX  = 0xBD   # LDA Absolute, X
INS_LDA_AY  = 0xB9   # LDA Absolute, Y
INS_LDA_INX = 0xA1   # LDA (Indirect, X)
INS_LDA_INY = 0xB1   # LDA (Indirect), Y

# STA Instructions
INS_STA_A   = 0x8D   # STA Absolute
INS_STA_AX  = 0x9D   # STA Absolute, X
INS_STA_AY  = 0x99   # STA Absolute, Y
INS_STA_Z   = 0x85   # STA Zero Page
INS_STA_ZX  = 0x95   # STA Zero Page, X
INS_STA_INX = 0x81   # STA (Indirect, X)
INS_STA_INY = 0x91   # STA (Indirect), Y

# LDX Instructions
INS_LDX_IM  = 0xA2   # LDX Immediate
INS_LDX_Z   = 0xA6   # LDX Zero Page
INS_LDX_ZY  = 0xB6   # LDX Zero Page, Y
INS_LDX_A   = 0xAE   # LDX Absolute
INS_LDX_AY  = 0xBE   # LDX Absolute, Y

# LDY Instructions
INS_LDY_IM  = 0xA0   # LDY Immediate
INS_LDY_Z   = 0xA4   # LDY Zero Page
INS_LDY_ZX  = 0xB4   # LDY Zero Page, X
INS_LDY_A   = 0xAC   # LDY Absolute
INS_LDY_AX  = 0xBC   # LDY Absolute, X

# JSR - Jump to Subroutine
INS_JSR     = 0x20

# JMP Instructions
INS_JMP_A   = 0x4C   # JMP Absolute
INS_JMP_I   = 0x6C   # JMP Indirect


def main():
    instructions = []
    if(len(argv) != 2):
        print(f"usage: {'py' if os.name == 'nt' else 'python3'} {argv[0]} \"<Instructions>\"")
        exit(1)
    argsInstructions = argv[1].replace(',', '').split(' ')   
    print(argsInstructions)
    # exit(0)
    for ins in argsInstructions:
        print(f'Processing instruction: \'{ins}\'', end='')
        if ins[:4] == 'INS_' or ins[:2] == '0x':
            n = eval(ins)
            if ins[:2] != '0x':
                print(f' -> 0x{n:02X}', end='')
            print('')
        else:
            n = int(f'0x{ins}',16)
        instructions.append(n)

    print(f'Writing {len(instructions)} instructions to mem.bin')
    instructions = bytes(instructions)

    with open('./mem.bin', 'wb') as f:
        f.write(instructions)
    
if __name__ == "__main__":
    main()

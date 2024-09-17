from sys import argv, executable, stderr
from cffi import FFI

HEADER  = "./src/instructions.h"
MEMORY  = "./mem.bin"
MEMSIZE = 2048
NOBREAK = '-NB'

def LOG_ERR(*args, **kwargs):
    print('[ERROR]:', *args, file=stderr, **kwargs)


def LOG_INFO(*args, **kwargs):
    print('[INFO]:', *args, **kwargs)


def main():
    argc: int = len(argv)
    helpUsage: bool = "-h" in argv or "--help" in argv or "-help" in argv
    # argc > 1 and (
    #     argv[1] == "-h" or argv[1] == "--help" or argv[1] == "-help"
    # )

    if argc <= 1 or helpUsage:
        logFunc = LOG_INFO if helpUsage else LOG_ERR
        logFunc(f"Usage: '{executable}' {argv[0]} <Instruction1, Instruction2...>")
        logFunc( "      Instructions Format Accepted: INS_LDA_A, 0xAD, AD")
        exit(0 if helpUsage else 1)

    NoBreak: bool = NOBREAK in argv
    if NoBreak:
        argv.remove(NOBREAK)

    with open(HEADER, "rt") as f:
        content = [s for s in f.read().splitlines() if s and s[0] != "#"]

    ffi = FFI()
    ffi.cdef("\n".join(content))
    ucrt = ffi.dlopen("ucrtbase.dll")

    InstructionsDict = dict()
    dictKeys = dir(ucrt)
    for key in dictKeys:
        InstructionsDict.update({key: getattr(ucrt, key)})

    ffi.dlclose(ucrt)

    argvInstructions = " ".join(argv[1:])

    InsKey = InstructionsDict.keys()
    InsVal = InstructionsDict.values()

    final = []

    for ins in argvInstructions.split(" "):
        n = None
        if ins in InsKey:
            n = InstructionsDict.get(ins)
        else:
            if ins[:2] == "0x":
                try:
                    n = eval(ins)
                except SyntaxError:
                    LOG_ERR(
                        f'Invalid hexadecimal literal, expected "0x00" to "0xFF" (0 to 255) range, received "{ins}"'
                    )
                    exit(2)
            else:
                try:
                    n = eval(f"0x{ins}")
                except SyntaxError:
                    LOG_ERR(
                        f'Invalid hexadecimal literal, expected "00" to "FF" (0 to 255) range, received "{ins}"'
                    )
                    exit(3)
            if n in InsVal:
                ins = None
                for i in InsKey:
                    if InstructionsDict[i] == n:
                        ins = i
                        break
        final.append(n)
        LOG_INFO(f"{ins}: 0x{n if n else 0:02X} ({n})")
        print(f'{"=" * 30}')

    if not NoBreak and final[-1] != 0:
        LOG_INFO('Adding "INS_BRK" (0x00) to end of sequence of instructions...')
        final.append(0)

    ins_count = len(final)
    if(ins_count > MEMSIZE):
        LOG_ERR(f"Too much instructions, received: {ins_count}, MAX: {MEMSIZE}")
        exit(4)

    LOG_INFO(f'Writing {ins_count} instructions to "{MEMORY}"')

    with open(MEMORY, "wb") as f:
        f.write(bytes(final))

    LOG_INFO("Finished...")


if __name__ == "__main__":
    main()

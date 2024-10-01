import json
from sys import argv
# from pprint import pprint

# from pprint import pprint
import ctypes as ct
# import struct as s

"""
#define KB(n) (n * 1024)
// #define MEMSIZE 2048ULL
#define MEMSIZE (KB(64))

typedef struct {
    uint16_t PC;
    uint8_t SP;

    uint8_t A;
    uint8_t X;
    uint8_t Y;

    // Processor Status
    bool C;
    bool Z;
    bool I;
    bool D;
    bool B;
    bool V;
    bool N;

    // RAM Memory
    uint8_t mem[MEMSIZE];
} cpu_t;
"""

MEMSIZE = 64 * 1024
MASK_C = 0b1
MASK_Z = 0b10
MASK_I = 0b100
MASK_D = 0b1000
MASK_B = 0b10000
#
MASK_V = 0b1000000
MASK_N = 0b10000000


class cpu_t(ct.Structure):
    _fields_ = [
        # ------------------------------
        ("PC", ct.c_uint16),
        ("SP", ct.c_uint8),
        #
        ("A", ct.c_uint8),
        ("X", ct.c_uint8),
        ("Y", ct.c_uint8),
        #
        ("C", ct.c_bool),  # 0
        ("Z", ct.c_bool),  # 1
        ("I", ct.c_bool),  # 2
        ("D", ct.c_bool),  # 3
        ("B", ct.c_bool),  # 4
        #                    5
        ("V", ct.c_bool),  # 6
        ("N", ct.c_bool),  # 7
        #
        ("mem", ct.c_uint8 * MEMSIZE),
    ]


def printStruct(struct: ct.Structure):
    for field_name, field_type in struct._fields_:
        ctype: str = str(field_type)
        if ctype == "<class 'ctypes.c_ushort'>":
            ctype = "uint16_t"
        elif ctype == "<class 'ctypes.c_ubyte'>":
            ctype = "uint8_t"
        elif ctype == "<class 'ctypes.c_bool'>":
            ctype = "bool"
        elif ctype == f"<class '_ctypes.array.c_ubyte_Array_{MEMSIZE}'>":
            ctype = "uint8_t[]"

        value = getattr(struct, field_name)
        if isinstance(value, ct.Array):
            value = list(value)
            for i, m in enumerate(value):
                if(m > 0):
                    print(f"mem[0x{i:04X}] = 0x{m:02X}")
            continue
        print(f"{ctype} {field_name}: {value}")
        if field_name in ["SP", "Y", "N"]:
            print("#")
    print('')


if len(argv) <= 1:
    print("Usage: py generateTest.py <test json file>")
    exit(1)

with open(argv[1], "rt") as f:
    jsonFile: dict = json.load(f)

inicial = cpu_t()
final = cpu_t()

print(f"sizeof(cpu_t) = {ct.sizeof(cpu_t)}")
print(f'{"-" * 30}')


def createCPUState(cpu: cpu_t, jsonState: dict):
    k: str
    for k, v in jsonState.items():
        if k == "p":
            cpu.C = v & MASK_C
            cpu.Z = v & MASK_Z
            cpu.I = v & MASK_I
            cpu.D = v & MASK_D
            cpu.B = v & MASK_B
            cpu.V = v & MASK_V
            cpu.N = v & MASK_N
        elif k == "ram":
            # continue
            for x in v:
                cpu.mem[x[0]] = x[1]
        else:
            if k == "s":
                k = "SP"
            else:
                k = k.upper()
            # print(k, v)
            setattr(cpu, k, v)
    # pprint(jsonState, sort_dicts=False)
    printStruct(cpu)
    # return cpu
    # pass

# for x in range(MEMSIZE):
#     inicial.mem[x] = 0xEA
#     final.mem[x] = 0xEA
createCPUState(inicial, jsonFile["initial"])
createCPUState(final,   jsonFile["final"])
# printStruct(inicial)
# print('')

TESTBIN = "./test.bin"
rst = bytes(inicial) + bytes(final)

with open(TESTBIN, 'wb') as f:
    f.write(rst)

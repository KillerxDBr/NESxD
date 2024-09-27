import json
from sys import argv

# from pprint import pprint
import ctypes as ct

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


class cpu_t(ct.Structure):
    _fields_ = [
        ("PC", ct.c_uint16),
        ("SP", ct.c_uint8),
        #
        ("A", ct.c_uint8),
        ("X", ct.c_uint8),
        ("Y", ct.c_uint8),
        #
        ("C", ct.c_bool),
        ("Z", ct.c_bool),
        ("I", ct.c_bool),
        ("D", ct.c_bool),
        ("B", ct.c_bool),
        ("V", ct.c_bool),
        ("N", ct.c_bool),
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
        # if isinstance(value, ct.Array):
        #     value = list(value)
        print(f"{ctype} {field_name}: {value}")


if len(argv) <= 1:
    print("Usage: py generateTest.py <test json file>")
    exit(1)

with open(argv[1], "rt") as f:
    jsonFile: dict = json.load(f)

inicial = cpu_t()
final = cpu_t()

print("Sizeof: ", ct.sizeof(inicial))
inicial.PC = 10
for x in range(MEMSIZE):
    inicial.mem[x] = 0xEA
    final.mem[x] = 0xEA

printStruct(inicial)
# pprint(jsonFile, sort_dicts=False)

# print(f'inicial: {inicial.PC}')

# print("NV1BDIZC")
# print(bin(jsonFile["initial"]["p"])[2:])
# print(bin(jsonFile["final"]["p"])[2:])

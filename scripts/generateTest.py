import json
from sys import argv
from pprint import pprint

assert len(argv) > 1

with open(argv[1], "rt") as f:
    jsonFile:dict = json.load(f)
    # content =

pprint(jsonFile, sort_dicts=False)
print("NV1BDIZC")
print(bin(jsonFile["initial"]["p"])[2:])
print(bin(jsonFile["final"]["p"])[2:])
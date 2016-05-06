#!/usr/bin/env python
import os
import sys
import json

with open(os.path.join(os.path.dirname(__file__), '..', 'src', 'query_backend.h')) as f:
    lines = f.readlines()

VERSION = sys.argv[1]

with open(os.path.join(os.path.dirname(__file__), '..', 'src', 'version.cc')) as f:
    version_lines = f.readlines()
    for i in range(0,len(version_lines)):
        if "describe()" in version_lines[i]:
            s = version_lines[i+1]
            #VERSION = s.split("return")[-1].strip().strip('\";')[:3]
            break

print(VERSION)

ALL_BACK = ["HDF5", "SQLite"]

ENUM_START = 0

for i in range(0,len(lines)):
    if "enum DbTypes {" in lines[i]:
        ENUM_START = i

ID = 0
outer = []
for i in range(ENUM_START+1,len(lines)):
    s = lines[i].split("//")[-1].strip()
    DB = lines[i].split("//")[0].strip().strip(",")
    if "[" in s:
        #s = "r"+s
        #print(s)
        x = json.loads(s)
        x.insert(0,ID)
        x.insert(1,DB)
        x.insert(5,VERSION)
        outer.append(x)
        ID += 1
    if "LIST" in lines[i]:
        break
#s = json.dumps(outer).partition("[")[2].rpartition("]")[0]
#s = s.replace("],","],\n    ")

s = ",\n    ".join(map(json.dumps, outer))
print(s)


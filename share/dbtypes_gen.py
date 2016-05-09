#!/usr/bin/env python
import os
import sys
import json

with open(os.path.join(os.path.dirname(__file__), '..', 'src', 'query_backend.h')) as f:
    lines = f.readlines()

VERSION = "v"+sys.argv[1]

#with open(os.path.join(os.path.dirname(__file__), '..', 'src', 'version.cc')) as f:
#    version_lines = f.readlines()
#    for i in range(0,len(version_lines)):
#        if "describe()" in version_lines[i]:
#            s = version_lines[i+1]
#            #VERSION = s.split("return")[-1].strip().strip('\";')[:3]
#            break

#print(VERSION)

ALL_BACK = ["SQLite", "HDF5"]

ENUM_START = 0

for i in range(0,len(lines)):
    if "enum DbTypes {" in lines[i]:
        ENUM_START = i

ID_INDEX = 0
DB_INDEX = 1
CPP_INDEX = 2
SHAPE_INDEX = 3
BACK_INDEX = 4
VERSION_INDEX = 5
SUPPORTED_INDEX = 6
CANON_INDEX = 7
VL_INDEX = 8

outer = []
for back in ALL_BACK:
    ID = 0
    inner = []
    for i in range(ENUM_START+1,len(lines)):
        s = lines[i].split("//")[-1].strip()
        DB = lines[i].split("//")[0].strip().strip(",")
        if "MAP_STRING_PAIR" in lines[i]:
            break
        if "[" in s:        
            x = json.loads(s)
            x.insert(ID_INDEX, ID)
            x.insert(DB_INDEX, DB)
            item_backends = x[BACK_INDEX]
            x[BACK_INDEX] = back
            x.insert(VERSION_INDEX,VERSION)
            x.insert(SUPPORTED_INDEX, 1 if back in item_backends else 0)
            inner.append(x)
            ID += 1
    outer += inner

#s = json.dumps(outer).partition("[")[2].rpartition("]")[0]
#s = s.replace("],","],\n    ")

s = ",\n    ".join(map(json.dumps, outer))
s = "    " + s
print(s)


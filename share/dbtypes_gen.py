#!/usr/bin/env python
from __future__ import print_function
import os
import re
import sys
import json


VERSION_READ_FAIL = "-1"

with open(os.path.join(os.path.dirname(__file__), '..', 'src', 'query_backend.h')) as f:
    lines = f.readlines()

if sys.argv[1] == VERSION_READ_FAIL:
    sys.exit()

ALL_BACKS = ["SQLite", "HDF5"]
ENUM_START = 0
VERSION = "v" + sys.argv[1]

for i in range(len(lines)):
    if lines[i].strip() == "enum DbTypes {":
        ENUM_START = i
        break
else:
    raise RuntimeError('could not find the start of the DbTypes enum')

ID_INDEX = 0
DB_INDEX = 1
CPP_INDEX = 2
SHAPE_INDEX = 3
BACK_INDEX = 4
VERSION_INDEX = 5
SUPPORTED_INDEX = 6
CANON_INDEX = 7
VL_INDEX = 8

RE_ENUM_ENTRY = re.compile('\s*\w+\s*?(=\s*?\d+)?,\s*?//.*')


def main():
    outer = []
    for back in ALL_BACKS:
        enum = 0
        inner = []
        for i in range(ENUM_START+1, len(lines)):
            if lines[i].strip() == '};':
                break
            m = RE_ENUM_ENTRY.match(lines[i])
            if m is None:
                continue
            s = lines[i].split("//")[-1].strip()
            #The extra strip statements keep the " = 0" out of the BOOL dbtype name.
            pre = lines[i].split("//")[0].strip().strip(",")
            db, eq, num = pre.partition("=")
            db = db.strip()
            if len(eq) > 0:
                enum = int(num.strip())
            if db == "":
                continue
            x = json.loads(s)
            x.insert(ID_INDEX, enum)
            x.insert(DB_INDEX, db)
            supported_backends = x[BACK_INDEX]
            x[BACK_INDEX] = back
            x.insert(VERSION_INDEX, VERSION)
            x.insert(SUPPORTED_INDEX, 1 if back in supported_backends else 0)
            inner.append(x)
            enum += 1
        outer += inner

    s = "," + ",\n    ".join(map(json.dumps, outer))
    print(s)


if __name__ == '__main__':
    main()

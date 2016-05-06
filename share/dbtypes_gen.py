#!/usr/bin/env python
import os
import json

with open(os.path.join(os.path.dirname(__file__), '..', 'src', 'query_backend.h')) as f:
    li = f.readlines()
    
ENUM_START = 0

for i in range(0,len(li)):
    if "enum DbTypes {" in li[i]:
        ENUM_START = i

for i in li[ENUM_START+1:]:
    s = i.split("//")[-1].strip()
    if "[" in s:
        #s = "r"+s
        #print(s)
        x = json.loads(s)
        print(x)
    if "VECTOR_BOOL" in i:
        break



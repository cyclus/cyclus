import os
import sys
import pprint
import json
import subprocess
from random import randint

from cyclus.lib import Hdf5Back, Recorder
import cyclus.typesystem as ts

import nose
from nose.tools import assert_equal, assert_true, assert_false, assert_raises

import pandas as pd
from pandas.util.testing import assert_frame_equal

cycdir = os.path.dirname(os.path.dirname(__file__))
sys.path.insert(0, os.path.join(cycdir, 'src'))

from hdf5_back_gen import resolve_unicode, convert_canonical, init_dicts, ORIGIN_DICT

#Call to hdf5_back_gen function 
init_dicts()

#Find supported HDF5 types.
with open(os.path.join(os.path.dirname(__file__), '..', 'share', 
                       'dbtypes.json')) as f:
    RAW_TABLE = resolve_unicode(json.load(f))

VERSION = ""
TABLE_START = 0
TABLE_END = 0
for row in range(len(RAW_TABLE)):
    current = tuple(RAW_TABLE[row])
    if current[4] == "HDF5":
        if current[5] > VERSION:
            VERSION = current[5]
            TABLE_START = row
        if current[5] == VERSION:
            TABLE_END = row    

TYPES_TABLE = list(tuple(row) for row in RAW_TABLE[TABLE_START:TABLE_END+1])

CANON_TYPES = []
CANON_TO_DB = {}      
CANON_TO_VL = {}
for row in TYPES_TABLE:
    if row[6] == 1 and row[4] == "HDF5" and row[5] == VERSION:        
        db = row[1]
        is_vl = row[8]
        canon = convert_canonical(row[7])
        if canon not in CANON_TYPES:
            CANON_TYPES.append(canon)
        CANON_TO_DB[canon] = db
        CANON_TO_VL[canon] = is_vl

VARIABLE_LENGTH_TYPES = ['LIST', 'VECTOR', 'MAP', 'SET', 'STRING'] 

CONTAINER_TYPES = {'LIST': 'LIST', 'VL_LIST': 'LIST', 
                   'VECTOR': 'VECTOR', 'VL_VECTOR': 'VECTOR',
                   'MAP': 'MAP', 'VL_MAP': 'MAP',
                   'SET': 'SET', 'VL_SET': 'SET',
                   'PAIR': 'PAIR'}

CREATE_FUNCTIONS = {'MAP': dict, 'LIST': list, 'SET': set, 'PAIR': list, 
                    'VECTOR': list, 'INT': int, 'DOUBLE': float, 'FLOAT': float,
                    'BOOL': bool, 'STRING': str, 'UUID': list, 'BLOB': str}

UNIQUE_TYPES = ['MAP', 'SET']

def make_bytes(string):
    return bytes(string.encode())

CONTAINER_MIN = 2
CONTAINER_MAX = 6

is_primitive = lambda canon: isinstance(canon, str)

TYPE_SHAPE = 0
TYPE_FUNCTION = 1
TYPE_CANON = 2
def generate_meta(canon, dist=0):
    meta_shape = []
    my_shape = None
    my_type = None
    origin = ORIGIN_DICT[canon]
    if is_primitive(canon):
        if origin in VARIABLE_LENGTH_TYPES:
            my_shape = randint(CONTAINER_MIN, CONTAINER_MAX)
        else:
            my_shape = -1
        my_type = CREATE_FUNCTIONS[origin]
        return (my_shape, my_type, canon)
    elif CANON_TO_VL[canon]:
        my_shape = -1
        my_type = CREATE_FUNCTIONS[origin[0]]
    elif origin[0] in VARIABLE_LENGTH_TYPES:
        my_shape = randint(CONTAINER_MIN, CONTAINER_MAX)
        my_type = CREATE_FUNCTIONS[origin[0]]
    else:
        my_shape = -1
        my_type = CREATE_FUNCTIONS[origin[0]]
    meta_shape.append((my_shape, my_type, canon))
    for i in canon[1:]:
        meta_shape.append(generate_meta(i, dist=dist+1))
    return meta_shape

chars = r'abcdefghijklmnopqrstuv1234567890`/\_.,-+@$#!% ' 
ok_bytes = [b'\x01', b'\x02', b'\x03', b'\x04', b'\x05', b'\x06', b'\x07']

def add_item(container, value):
    if len(value) == 1:
        try:
            container.append(value[0])
        except AttributeError:
            try:
                container.add(value[0])
            except AttributeError:
                print("Unsure how to add " + value[0] + " of type " 
                      + str(type(value[0])) + " to " + str(type(container)))
                sys.exit(1)
    else:
        try:
            container.update({value[0]: value[1]})
        except AttributeError:
            try:
                container = (value[0], value[1])
            except AttributeError:
                print("Unsure how to add " + str(value[0]) + " of type " 
                      + str(type(value[0])) + " and " + str(value[1]) 
                      + " of type " + str(type(value[1])) + " to " 
                      + str(type(container)))
                sys.exit(1)
    return container

def populate(meta):
    if isinstance(meta, tuple):
        current_type = meta
    else:
        current_type = meta[0]
    my_shape = current_type[TYPE_SHAPE]
    my_type = current_type[TYPE_FUNCTION]
    canon = current_type[TYPE_CANON]
    data = my_type()
    origin = ORIGIN_DICT[canon]
    if is_primitive(canon):
        if origin == 'STRING' or origin == 'BLOB':
            if my_shape > 0:
                for i in range(my_shape):
                    data += chars[randint(0, len(chars)-1)]
            else:
                for i in range(randint(CONTAINER_MIN, CONTAINER_MAX)):
                    data += chars[randint(0, len(chars)-1)]
            if origin == 'BLOB':
                data = make_bytes(data)
        #elif canon == 'BLOB':
        #    if my_shape < 0:
        #        my_shape = randint(CONTAINER_MIN, CONTAINER_MAX)
        #    for i in range(my_shape):
        #        data += ok_bytes[randint(0, len(ok_bytes)-1)]
        elif canon == 'BOOL':               
            data = bool(randint(0,1))
        elif canon == 'UUID':
            data = [1,1,1,1]
        else:
            data = my_type(randint(0, 9) * (data + 1))
    elif origin[0] in VARIABLE_LENGTH_TYPES:
        if my_shape > 0:
            item_count = my_shape
        else:
            item_count = randint(CONTAINER_MIN, CONTAINER_MAX)
        add_count = 0
        while add_count < item_count:
            #reset our current child
            child_data = []
            #fill with child values
            for j in meta[1:]:
                child_data.append(populate(j))
            if origin[0] in UNIQUE_TYPES and child_data[0] in data:
                continue
            else:
                #add to parent
                data = add_item(data, child_data)
                add_count += 1
    else:
        #pairs, etc.
        child_data = []
        for i in meta[1:]:
            child_data.append(populate(i))
        data = add_item(data, child_data)    
    return data

def get_shape(meta):
    shape = []
    if isinstance(meta, tuple):
        return [meta[TYPE_SHAPE]]
    else:
        current_type = meta[0]
        shape.append(current_type[TYPE_SHAPE])
        for i in meta[1:]:
            shape.extend(get_shape(i))
        return shape

ROW_NUM = 2
PATH = 'get_test_logs/{t}gen_db.h5'
def generate_and_test():
    #subprocess.run(['rm', PATH])
    for i in CANON_TYPES:
        print(i)
        MY_PATH = PATH.format(t=CANON_TO_DB[i])
        subprocess.run(['rm', MY_PATH])
        #print("\nCreating test for " + CANON_TO_DB[i])
        rec = Recorder(inject_sim_id=False)
        back = Hdf5Back(MY_PATH)
        rec.register_backend(back)
        data_meta = generate_meta(i)
        shape = get_shape(data_meta)
        data = []
        for j in range(ROW_NUM):
            data.append(populate(data_meta))
            d = rec.new_datum("test" + str(j))
            d.add_val("col0", data[j], shape=shape, type=ts.IDS[CANON_TO_DB[i]])
            d.record()
        rec.flush()
        
        for j in range(ROW_NUM):
            print("Row " + str(j) + " is " + str(data[j]))
            exp = pd.DataFrame({"col0": [data[j]]}, columns=['col0'])
            obs = back.query("test" + str(j))
            #print("---\n", obs)
            yield assert_frame_equal, exp, obs
        
        rec.close()
        

if __name__ == "__main__":
    nose.runmodule()
    

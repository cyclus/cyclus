import os
import sys
import json
import subprocess
from random import randint
import uuid
import nose
from nose.plugins.skip import SkipTest
import pandas as pd
from pandas.util.testing import assert_frame_equal

from cyclus.lib import Hdf5Back, Recorder
import cyclus.typesystem as ts

cycdir = os.path.dirname(os.path.dirname(__file__))
sys.path.insert(0, os.path.join(cycdir, 'src'))
import hdf5_back_gen
#from hdf5_back_gen import resolve_unicode, convert_canonical, setup, ORIGIN_DICT

#Call to hdf5_back_gen function 
hdf5_back_gen.setup()

is_primitive = lambda canon: isinstance(canon, str)

CHARS = r'abcdefghijklmnopqrstuv1234567890`/\_.,-+@$#!% '

CANON_TYPES = []
CANON_TO_DB = {}      
CANON_TO_VL = {}
def setup():
    with open(os.path.join(os.path.dirname(__file__), '..', 'share', 
                       'dbtypes.json')) as f:
        RAW_TABLE = hdf5_back_gen.resolve_unicode(json.load(f))
    
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
    for row in TYPES_TABLE:
        if row[6] == 1 and row[4] == "HDF5" and row[5] == VERSION:        
            db = row[1]
            is_vl = row[8]
            canon = hdf5_back_gen.convert_canonical(row[7])
            if canon not in CANON_TYPES:
                CANON_TYPES.append(canon)
            CANON_TO_DB[canon] = db
            CANON_TO_VL[canon] = is_vl

def make_bytes(string):
    return string.encode()

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

TYPE_SHAPE = 0
TYPE_FUNCTION = 1
TYPE_CANON = 2
CONTAINER_MIN = 4
CONTAINER_MAX = 8
def generate_meta(canon, depth=0):
    """Produces metadata about a type to be created.
    
    This includes a shape, constructor function, and canonical form. This 
    3-tuple can be passed to the populate function to create data in the form
    specified by the metadata.
    
    Parameters
    ----------
    canon : tuple or str
        Unique identifier of current data type
    depth : int, optional
        Recursive depth value
        
    Returns
    -------
    meta_shape : tuple or list
        Meta data specification for type
    """
    meta_shape = []
    my_shape = None
    my_type = None
    origin = hdf5_back_gen.ORIGIN_DICT[canon]
    if is_primitive(canon):
        if CANON_TO_VL[canon]:
            my_shape = -1
        elif origin in VARIABLE_LENGTH_TYPES:
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
        meta_shape.append(generate_meta(i, depth=depth+1))
    return meta_shape

def add_item(container, value):
    """Attempts to add a value to a container of unknown type.
    
    Parameters
    ----------
    container : iterable
        The type into which the value is inserted
    value : type
        Data point to insert
        
    Returns
    -------
    container : iterable
        The container with added value
    """
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
    """Generate data based upon the metadata for a type.
    
    Uses the shape values within meta to create data with the correct dimensions
    for adding to the table.
    
    Parameters
    ----------
    meta : tuple or list
        Metadata for type
    
    Returns
    -------
    data : type specified in meta
        Populated container or primitive
    """
    if isinstance(meta, tuple):
        current_type = meta
    else:
        current_type = meta[0]
    my_shape = current_type[TYPE_SHAPE]
    my_type = current_type[TYPE_FUNCTION]
    canon = current_type[TYPE_CANON]
    data = my_type()
    origin = hdf5_back_gen.ORIGIN_DICT[canon]
    if is_primitive(canon):
        if origin == 'STRING' or origin == 'BLOB':
            if my_shape > 0:
                for i in range(my_shape):
                    data += CHARS[randint(0, len(CHARS)-1)]
            else:
                for i in range(randint(CONTAINER_MIN, CONTAINER_MAX)):
                    data += CHARS[randint(0, len(CHARS)-1)]
            if origin == 'BLOB':
                data = make_bytes(data)
        elif canon == 'BOOL':               
            data = bool(randint(0,1))
        elif canon == 'UUID':
            data = uuid.uuid5(uuid.NAMESPACE_DNS, CHARS[randint(0,len(CHARS)-1)])
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
    """Make shape into a flat int list for the Hdf5 backend."""
    shape = []
    if isinstance(meta, tuple):
        return [meta[TYPE_SHAPE]]
    else:
        current_type = meta[0]
        shape.append(current_type[TYPE_SHAPE])
        for i in meta[1:]:
            shape.extend(get_shape(i))
        return shape

ROW_NUM = 3
PATH = 'gen_db.h5'
def generate_and_test():
    """Generate and run tests for supported Hdf5 datatypes."""
    if sys.version_info[0] == 2:
        msg = 'Hdf5 backend gen tests do not support Python 2.x'
        raise SkipTest(msg)
    if os.path.isfile(PATH):
        os.remove(PATH)
    for i in CANON_TYPES:
        print(CANON_TO_DB[i],'\n')                
        rec = Recorder(inject_sim_id=False)
        back = Hdf5Back(PATH)
        rec.register_backend(back)
        data_meta = generate_meta(i)
        shape = get_shape(data_meta)
        print("shape: ", shape)
        data = []
        for j in range(ROW_NUM):
            data.append(populate(data_meta))
        exp = pd.DataFrame({'col0': data}, columns=['col0'])
        print("expected: \n", exp)
        for j in data:
            d = rec.new_datum("test0")
            d.add_val("col0", j, shape=shape, type=ts.IDS[CANON_TO_DB[i]])
            d.record()
            rec.flush()
        obs = back.query("test0")
        print("observed: \n", obs)
        yield assert_frame_equal, exp, obs
        rec.close()
        os.remove(PATH)

if __name__ == "__main__":
    nose.runmodule()
    

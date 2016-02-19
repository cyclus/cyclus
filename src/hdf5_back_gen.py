#!/usr/bin/env python
"""This module generates HDF5 backend code found in src/hdf5_back.cc"""
import os
import json
import textwrap
from ast import literal_eval





with open(os.path.join(os.path.dirname(__file__), '..', 'share', 'dbtypes.json')) as f:
    RAW_TABLE = json.load(f)

TABLE_START = 0
for row in range(0, len(RAW_TABLE)):
    current = tuple(RAW_TABLE[row])
    if current[5] == "v1.3" and current[4] == "HDF5":
        TABLE_START = row
        break

V3_TABLE = list(tuple(row) for row in RAW_TABLE[TABLE_START:])

CANON_SET = set()
DB_TO_CPP = {}
CANON_TO_DB = {}
INDENT = '    '

def convert_canonical(raw_list):
    if isinstance(raw_list, str):
        return raw_list
    return tuple(convert_canonical(x) for x in raw_list)

for row in V3_TABLE:
    if row[6] == 1 and row[4] == "HDF5":
        CANON_SET.add(convert_canonical(row[7]))
        DB_TO_CPP[row[1]] = row[2]
        CANON_TO_DB[convert_canonical(row[7])] = row[1]

def list_dependencies(canon):
    """A list of a type's dependencies, in canonical form.
    
    Parameters
    ----------
    canon : tuple, str
        The canonical form of the type, after conversion from list
    
    Returns
    -------
    list
        List of all dependencies, with original type as element 0.   
    
    Examples
    --------
    >>> list_dep("('PAIR', 'INT', 'VL_STRING')")
    [('PAIR', 'INT', 'VL_STRING'), 'INT', 'VL_STRING']
    """
    if isinstance(canon, str):
        return canon
    
    dependency_list = [u for u in canon[1:]]
    return [canon] + dependency_list 

class TypeStr(object):
    """Represents a archetype data type.
    
    Parameters
    ----------
    canon : str
        Canonical representation of the data type
    
    Attributes
    ----------
    canon : str
        Canonical representation of the data type
    db : str
        Dbtype of the data type
    cpp : str
        C++ representation of the data type
    sub : list
        List of dependencies as TypeStr objects (including self as element 0)
    """
    def __init__(self, canon):
        self.canon = canon
        self.db = CANON_TO_DB[canon]
        self.cpp = DB_TO_CPP[self.db]
        if isinstance(canon, str):
            self.sub = [self]
        else:
            self.sub = [self] + [TypeStr(u) for u in list_dependencies(self.canon)[1:]]
NORMAL_CLOSE = """
is_row_selected = CmpConds<{t.cpp}>>(&x, &(field_conds[qr.fields[j]]));
if(is_row_selected)
    row[j] = x;
""".strip()

CASE_TEMPLATE = """
case {t.db}: {{
{read_x}
    break;
}}"""

REINTERPRET_CAST_READER = """
{t.cpp}{*1} xraw = {*2}reinterpret_cast<{t.cpp}*>(buf+offset);
{teardown}
""".strip()

STRING_READER = """
{left_side} = {t.cpp}(buf + offset, col_sizes_[table][j]);
size_t nullpos = {left_side}.find('\\0');
if (nullpos != {t.cpp}::npos)
    {left_side}.resize(nullpos);
{teardown}
""".strip()

VL_READER = """
{left_side} x = VLRead<{t.cpp}, {t.db}>(buf + offset {cyclus_constant});
{teardown}
""".strip()

UUID_READER = """
{t.cpp} x;
memcpy(&x, buf+offset, 16);
{NO_CLOSE}
""".strip()

VECTOR_READER = """
{setup}
{t.cpp} x;
{body}
{teardown}
""".strip()

SET_READER = """
{setup}
{t.cpp} x;
{body}
{teardown}
""".strip()

LIST_READER = """
""".strip()

PAIR_READER = """
{setup1}
{setup2}

""".strip()

MAP_READER = """
""".strip()

READERS = {'INT': REINTERPRET_CAST_READER,
           'BOOL': REINTERPRET_CAST_READER,
           'FLOAT': REINTERPRET_CAST_READER,
           'DOUBLE': REINTERPRET_CAST_READER,
           'STRING': STRING_READER,
           'VL_STRING': VL_READER,
           'BLOB': VL_READER,
           'UUID': UUID_READER,
           'VECTOR': VECTOR_READER,
           'VL_VECTOR': VL_READER,
           'SET': SET_READER,
           'VL_SET': VL_READER,
           'LIST': LIST_READER,
           'VL_LIST': VL_READER,
           'PAIR': PAIR_READER,
           'MAP': MAP_READER,
           'VL_MAP': VL_READER}

DEF_BODY = "x = {t.cpp}(xraw, xraw+jlen);"

MEMCPY_BODY = """
x = {t.cpp}({col_size} / {fieldlen});
memcpy(&x[0], buf + offset, {col_size});
""".strip()

ELEMENTWISE_BODY = """
for (unsigned int k = 0; k < {fieldlen}; ++k) {{
    {decl_k};
    {def_k} = {t.cpp}(buf + offset + strlen*k, strlen);
    nullpos = {def_k}.find('\\0');
    if (nullpos != {t.cpp}::npos)
        {def_k}.resize(nullpos);
    {apply}
}}
""".strip()

VL_BODY = """
jlen = {col_size} / CYCLUS_SHA1_SIZE;
x = {t.cpp}(jlen);
x[k] = VLRead<{t.sub[1].cpp}, {t.sub[1].db}>(buf + offset + CYCLUS_SHA1_SIZE*k);
""".strip()

TEMPLATE_BODY = """
for (unsigned int k = 0; k < {fieldlen} ++k) {
    {sub_body}
}
""".strip()

BODIES = {'INT': DEF_BODY,
          'DOUBLE': MEMCPY_BODY,
          'STRING': ELEMENTWISE_BODY,
          'VL_STRING': VL_BODY,
          'VECTOR': TEMPLATE_BODY,
          'SET': TEMPLATE_BODY}

QUERY_CASES = ''

#CLOSURE_STACK = []

def create_reader(a_type, depth=0):
    current_type = a_type
    if isinstance(current_type.canon, str):
        reader = READERS[current_type.canon]
        rtn = create_primitive_reader(current_type.canon, current_type, reader, depth+1)
    else:
        reader = READERS[current_type.canon[0]]
        if current_type.canon[0] == "VECTOR":
            rtn = create_vector_reader(current_type.canon, current_type, reader, depth+1)
        if current_type.canon[0] == "SET":
            rtn = create_set_reader(current_type.canon, current_type, reader, depth)
    
    ctx = {"t": a_type,
           "setup": rtn[0],
           "body": rtn[1],
           "teardown": rtn[2],
           "fieldlen": create_fieldlen(a_type).format(t = a_type.sub[1])}
    
    return reader.format(**ctx)

def create_setup(a_type, depth):
    #we know how to do setup explicitly if it's a primitive type
    if isinstance(a_type.canon, str):
        if a_type.db == "STRING":
            return """
hid_t field_type = H5Tget_member_type(tb_type, j);
size_t nullpos;
hsize_t fieldlen;
H5Tget_array_dims2(field_type, &fieldlen);
unsigned int strlen = col_sizes_[table][j] / fieldlen;
""".strip()
        else:
            return """
jlen = col_sizes_[table][j] / {fieldlen};
{t.cpp}* xraw = reinterpret_cast<{t.cpp}*>(buf + offset);
""".strip().format(t = a_type, fieldlen = create_fieldlen(a_type).format(t = a_type))    
    #however, if the type is a dependent type, there's no explicit way
    else:
        #recurse!
        return create_setup(a_type.sub[1], depth+1)

def create_body(a_type, depth):
    body = ""
    if isinstance(a_type.sub[1].canon, str):
        ctx = create_body_ctx(a_type, depth)
        body = BODIES[a_type.sub[1].canon].format(**ctx)
    else:
        body = BODIES[a_type.canon[0]]
        #recurse!
        body = body.format(t = a_type, fieldlen = create_fieldlen(a_type), col_size = "col_sizes_[table][j]", sub_body = create_body(a_type.sub[1], depth+1))
    return body

def create_body_ctx(a_type, depth):
    ctx = {"t": a_type,
           "fieldlen": create_fieldlen(a_type.sub[1]),
           "col_size": "col_sizes_[table][j]"}
    if a_type.canon[0] == "SET":
        ctx["decl_k"] = "{t.sub[1].cpp} x_k;".format(t=a_type)
        ctx["def_k"] = "x_k"
        ctx["apply"] = "x.insert(x_k);"
    elif a_type.canon[0] == "VECTOR":        
        ctx["decl_k"] = ""
        ctx["def_k"] = "x[k]"
        ctx["apply"] = ""
    return ctx
#helper method for create_teardown
import re
def find_whole_word(word):
    return re.compile(r'\b({0})\b'.format(word)).search

def create_teardown(a_type, setup=""):
    closure = NORMAL_CLOSE
    if "hid_t" not in setup:
        return closure + "\nbreak;"
    else:
        for line in setup.split("\n"):
            if find_whole_word("hid_t")(line) != None:
                closure += "\nH5Tclose("+line.split("=")[0].strip("hid_t").strip()+");"
        return (closure + "\nbreak;").format(t = a_type)

def create_fieldlen(a_type):
    if a_type.db == "STRING":
        return "fieldlen"
    elif a_type.db == "VL_STRING":
        return "CYCLUS_SHA1_SIZE"
    else:
        return "sizeof({t.cpp})"

def create_primitive_reader(t, current_type, reader, depth):
    ctx = {"t": current_type}
    if depth == 1:
        ctx["*2"] = "*"
        ctx["*1"] = ""
        ctx["teardown"] = create_teardown()
    else:
        ctx["*2"] = ""
        ctx["*1"] = "*"
        ctx["teardown"] = ""
    return reader.format(**ctx)
    
def create_vector_reader(t, current_type, reader, depth):
    ctx = {"t": current_type}
    if isinstance(current_type.canon[1], str):
        ctx["setup"] = create_setup(current_type, depth)
        ctx["fieldlen"] = create_fieldlen(current_type).format(t=current_type)
        ctx["read_type"] = create_read_type(current_type.sub[1])
        if depth == 1:
            ctx["teardown"] = create_teardown(ctx["setup"]).format(t=current_type)
        else:
            ctx["teardown"] = ""
    else:
        ctx["read_type"] = create_reader(current_type.canon[1])
    return reader.format(**ctx)

def create_set_reader(t, current_type, reader, depth):
    pieces = []
    
    ctx = {"t": current_type, "fieldlen": create_fieldlen(current_type.sub[1]), "col_size": "col_sizes_[table][j]"}
    
    setup = create_setup(current_type.sub[1], depth)
    pieces.append(setup)
    
    body = create_body(current_type, depth)
    pieces.append(body)
    
    teardown = create_teardown(current_type, setup)
    pieces.append(teardown)
    
    #pieces = [p.format(**ctx) for p in pieces]
    
    return tuple(pieces)
    
def create_list_reader(t, current_type, reader, depth):
    return
def create_pair_reader(t, current_type, reader, depth):
    ctx = {"t": current_type}
    ctx["setup1"] = create_setup(current_type.sub[1], depth)
    ctx["setup2"] = create_setup(current_type.sub[2], depth)
    
    
    return reader.format(**ctx)
def create_map_reader(t, current_type, reader, depth):
    return
    
def main():
    #global QUERY_CASES
    #for ca in CANON_SET: 
   #     current_type = TypeStr(ca)
   #     reader = READERS[current_type.db]
   #     ctx = {"t": current_type,
   #            "NO_CLOSE": NO_CLOSE.format(t = current_type),
   #            "H5TCLOSE": H5TCLOSE.format(t = current_type),
   #            "H5TCLOSE_MULTI": H5TCLOSE_MULTI.format(t = current_type)}
                  
   #     QUERY_CASES += CASE_TEMPLATE.format(t = current_type, read_x = textwrap.indent(reader.format(**ctx), INDENT))

    #print(textwrap.indent(QUERY_CASES, INDENT*2))
    
    s = create_reader(TypeStr(("VECTOR","STRING")))
    
    print(textwrap.indent(s, INDENT)) 
    
if __name__ == '__main__':
    main()

#!/usr/bin/env python
"""This module generates HDF5 backend code found in src/hdf5_back.cc"""
import os
import json
from ast import literal_eval

def resolve_unicode(item):	   
    #Python3, if we can handle it, don't bother.    
    if isinstance(item, str):
        return item        
    #We must check every element in tuples and lists.    
    elif isinstance(item, tuple):
        return tuple([resolve_unicode(i) for i in item])
    elif isinstance(item, list):
        return [resolve_unicode(i) for i in item]
    #Not a string, either unicode (Python2.7) or an int.    
    else: 
        try:
            return item.encode('utf-8')
        except Exception:
            pass
        return item

with open(os.path.join(os.path.dirname(__file__), '..', 'share', 'dbtypes.json')) as f:
    RAW_TABLE = resolve_unicode(json.load(f))

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
    if type(raw_list) is str:
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

CASE_TEMPLATE = """
case {t.db}: {{
{read_x}
    break;
}}"""

NO_CLOSE = """
is_row_selected = CmpConds<{t.cpp}>(&x, &(field_conds[qr.fields[j]]));
if (is_row_selected)
    row[j] = x;
""".strip()

H5TCLOSE = """
is_row_selected = CmpConds<{t.cpp}>(&x, &(field_conds[qr.fields[j]]));
if (is_row_selected)
    row[j] = x;
H5Tclose(field_type);
""".strip()

H5TCLOSE_MULTI = """
is_row_selected = CmpConds<{t.cpp}>(&x, &(field_conds[qr.fields[j]]));
if (is_row_selected)
    row[j] = x;
H5Tclose(key_type);
H5Tclose(item_type);
H5Tclose(field_type);
""".strip()

REINTERPRET_CAST_READER = """
{t.cpp} x = *reinterpret_cast<{t.cpp}*>(buf + offset);
{NO_CLOSE}
""".strip()

STRING_READER = """
{t.cpp} x = {t.cpp}(buf + offset, col_sizes_[table][j]);
size_t nullpos = x.find('\\0');
if (nullpos != {t.cpp}::npos)
    x.resize(nullpos);
{NO_CLOSE}
""".strip()

VL_READER = """
{t.cpp} x = VLRead<{t.cpp}, {t.db}>(buf + offset);
{NO_CLOSE}
""".strip()

UUID_READER = """
{t.cpp} x;
memcpy(&x, buf+offset, 16);
{NO_CLOSE}
""".strip()

VECTOR_READER = """
{t.cpp} x = {t.cpp}(col_sizes_[table][j] / sizeof({t.sub[1].cpp}));
memcpy(&x[0], buf + offset, col_sizes_[table][j]);
{NO_CLOSE}
""".strip()

VECTOR_STRING_READER = """
hid_t field_type = H5Tget_member_type(tb_type, j);
size_t nullpos;
hsize_t fieldlen;
H5Tget_array_dims2(field_type, &fieldlen);
unsigned int strlen = col_sizes_[table][j] / fieldlen;
{t.cpp} x = {t.cpp}(fieldlen);
for(unsigned int k = 0; k < fieldlen; ++k) {{
    x[k] = {t.sub[1].cpp}(buf + offset + strlen*k, strlen);
    nullpos = x[k].find('\\0');
    if(nullpos != {t.sub[1].cpp}::npos)
        x[k].resize(nullpos);
}}
{H5TCLOSE}
""".strip()

VECTOR_VL_STRING_READER = """
jlen = col_sizes_[table][j] / CYCLUS_SHA1_SIZE;
{t.cpp} x = {t.cpp}(jlen);
for (unsigned int k = 0; k < jlen; ++k) {{
    x[k] = VLRead<{t.sub[1].cpp}, {t.sub[1].db}>(buf + offset + CYCLUS_SHA1_SIZE*k);
}}
{NO_CLOSE}
""".strip()

SET_LIST_X_READER = """
jlen = col_sizes_[table][j] / sizeof({t.sub[1].cpp});
{t.sub[1].cpp}* xraw = reinterpret_cast<{t.sub[1].cpp}*>(buf + offset);
{t.cpp} x = {t.cpp}(xraw, xraw+jlen);
{NO_CLOSE}
""".strip()

SET_STRING_READER = """
hid_t field_type = H5Tget_member_type(tb_type, j);
size_t nullpos;
hsize_t fieldlen;
H5Tget_array_dims2(field_type, &fieldlen);
unsigned int strlen = col_sizes_[table][j] / fieldlen;
{t.cpp} x;
for(unsigned int k = 0; k < fieldlen; ++k) {{
    {t.sub[1].cpp} s = {t.sub[1].cpp}(buf + offset + strlen*k, strlen);
    nullpos = s.find('\\0');
    if(nullpos != {t.sub[1].cpp}::npos)
        s.resize(nullpos);
    x.insert(s);
}}
{H5TCLOSE}
""".strip()

SET_VL_STRING_READER = """
jlen = col_sizes_[table][j] / CYCLUS_SHA1_SIZE;
{t.cpp} x;
for (unsigned int k = 0; k < jlen; ++k) {{
    x.insert(VLRead<{t.sub[1].cpp}, {t.sub[1].db}>(buf + offset + CYCLUS_SHA1_SIZE*k));
}}
{NO_CLOSE}
""".strip()

LIST_STRING_READER = """
hid_t field_type = H5Tget_member_type(tb_type, j);
size_t nullpos;
hsize_t fieldlen;
H5Tget_array_dims2(field_type, &fieldlen);
unsigned int strlen = col_sizes_[table][j] / fieldlen;
{t.cpp} x;
for(unsigned int k = 0; k < fieldlen; ++k) {{
    {t.sub[1].cpp} s = {t.sub[1].cpp}(buf + offset + strlen*k, strlen);
    nullpos = s.find('\\0');
    if(nullpos != {t.sub[1].cpp}::npos)
        s.resize(nullpos);
    x.push_back(s);
}}
{H5TCLOSE}
""".strip()

LIST_VL_STRING_READER = """
jlen = col_sizes_[table][j] / CYCLUS_SHA1_SIZE;
{t.cpp} x;
for (unsigned int k = 0; k < jlen; ++k) {{
    x.push_back(VLRead<{t.sub[1].cpp}, {t.sub[1].db}>(buf + offset + CYCLUS_SHA1_SIZE*k));
}}
{NO_CLOSE}
""".strip()

PAIR_INT_INT_READER = """
{t.cpp} x = std::make_pair(*reinterpret_cast<{t.sub[1].cpp}*>(buf + offset), *reinterpret_cast<{t.sub[1].cpp}*>(buf + offset + sizeof({t.sub[1].cpp})));
{NO_CLOSE}
""".strip()

PAIR_INT_STRING_READER = """
size_t nullpos;
unsigned int strlen = col_sizes_[table][j] - sizeof(int);
{t.sub[1].cpp} xfirst = *reinterpret_cast<{t.sub[1].cpp}*>(buf + offset);
{t.sub[2].cpp} s = {t.sub[2].cpp}(buf + offset + sizeof(int), strlen);
nullpos = s.find('\\0');
if(nullpos != {t.sub[2].cpp}::npos)
    s.resize(nullpos);
{t.cpp} x = std::make_pair(xfirst, s);
{NO_CLOSE}
""".strip()

PAIR_INT_VL_STRING_READER = """
unsigned int itemsize = sizeof(int) + CYCLUS_SHA1_SIZE;
jlen = col_sizes_[table][j] / itemsize;
{t.cpp} x = std::make_pair(*reinterpret_cast<{t.sub[1].cpp}*>(buf + offset), VLRead<{t.sub[2].cpp}, {t.sub[2].db}>(buf + offset + sizeof(int)));
{NO_CLOSE}
""".strip()

MAP_XY_READER = """
{t.cpp} x = {t.cpp}();
size_t itemsize = sizeof({t.sub[1].cpp}) + sizeof({t.sub[2].cpp});
jlen = col_sizes_[table][j] / itemsize;
for (unsigned int k = 0; k < jlen; ++k) {{
    x[*reinterpret_cast<{t.sub[1].cpp}*>(buf + offset + itemsize*k)] = *reinterpret_cast<{t.sub[2].cpp}*>(buf + offset + itemsize*k + sizeof({t.sub[1].cpp}));
}}
{NO_CLOSE}
""".strip()

MAP_INT_STRING_READER = """
hid_t field_type = H5Tget_member_type(tb_type, j);
size_t nullpos;
hsize_t fieldlen;
H5Tget_array_dims2(field_type, &fieldlen);
unsigned int itemsize = col_sizes_[table][j] / fieldlen;
unsigned int strlen = itemsize - sizeof(int);
{t.cpp} x;
for (unsigned int k = 0; k < fieldlen; ++k) {{
    {t.sub[2].cpp} s = {t.sub[2].cpp}(buf + offset + itemsize*k + sizeof({t.sub[1].cpp}), strlen);
    nullpos = s.find('\\0');
    if(nullpos != {t.sub[2].cpp}::npos)
        s.resize(nullpos);
    x[*reinterpret_cast<{t.sub[1].cpp}*>(buf + offset + itemsize*k)] = s;
}}
{H5TCLOSE}
""".strip()

MAP_INT_VL_STRING_READER = """
unsigned int itemsize = sizeof({t.sub[1].cpp}) + CYCLUS_SHA1_SIZE;
jlen = col_sizes_[table][j] / itemsize;
{t.cpp} x;
for(unsigned int k = 0; k < jlen; ++k) {{
    x[*reinterpret_cast<{t.sub[1].cpp}*>(buf + offset + itemsize*k)] = VLRead<{t.sub[2].cpp}, {t.sub[2].db}>(buf + offset + itemsize*k + sizeof({t.sub[1].cpp}));
}}
{NO_CLOSE}
""".strip()

MAP_STRING_X_READER = """
hid_t field_type = H5Tget_member_type(tb_type, j);
size_t nullpos;
hsize_t fieldlen;
H5Tget_array_dims2(field_type, &fieldlen);
unsigned int itemsize = col_sizes_[table][j] / fieldlen;
unsigned int strlen = itemsize - sizeof({t.sub[2].cpp});
{t.cpp} x;
for (unsigned int k = 0; k < fieldlen; ++k) {{
    {t.sub[1].cpp} s = {t.sub[1].cpp}(buf + offset + itemsize*k, strlen);
    nullpos = s.find('\\0');
    if (nullpos != {t.sub[1].cpp}::npos)
        s.resize(nullpos);
    x[s] = *reinterpret_cast<{t.sub[2].cpp}*>(buf + offset + itemsize*k + strlen);
}}
{H5TCLOSE}
""".strip()

MAP_VL_STRING_X_READER =  """
unsigned int itemsize = sizeof({t.sub[2].cpp}) + CYCLUS_SHA1_SIZE;
jlen = col_sizes_[table][j] / itemsize;
{t.cpp} x;
for (unsigned int k = 0; k < jlen; ++k) {{
    x[VLRead<{t.sub[1].cpp}, {t.sub[1].db}>(buf + offset + itemsize*k)] = *reinterpret_cast<{t.sub[2].cpp}*>(buf + offset + itemsize*k + CYCLUS_SHA1_SIZE);
}}
{NO_CLOSE}
""".strip()

MAP_STRING_STRING_READER = """
hid_t field_type = H5Tget_member_type(tb_type, j);
hid_t item_type = H5Tget_super(field_type);
hid_t key_type = H5Tget_member_type(item_type, 0);
size_t nullpos;
hsize_t fieldlen;
H5Tget_array_dims2(field_type, &fieldlen);
unsigned int itemsize = col_sizes_[table][j] / fieldlen;
unsigned int keylen = H5Tget_size(key_type);
unsigned int vallen = itemsize - keylen;
{t.cpp} x;
for (unsigned int k = 0; k < fieldlen; ++k) {{
    {t.sub[1].cpp} key = {t.sub[1].cpp}(buf + offset + itemsize*k, keylen);
    nullpos = key.find('\\0');
    if (nullpos != {t.sub[1].cpp}::npos)
        key.resize(nullpos);
    {t.sub[2].cpp} val = {t.sub[2].cpp}(buf + offset + itemsize*k + keylen, vallen);
    nullpos = val.find('\\0');
    if (nullpos != {t.sub[2].cpp}::npos)
        val.resize(nullpos);
    x[key] = val;
}}
{H5TCLOSE_MULTI}
""".strip()

MAP_STRING_VL_STRING_READER = """
hid_t field_type = H5Tget_member_type(tb_type, j);
size_t nullpos;
hsize_t fieldlen;
H5Tget_array_dims2(field_type, &fieldlen);
unsigned int itemsize = col_sizes_[table][j] / fieldlen;
unsigned int keylen = itemsize - CYCLUS_SHA1_SIZE;
{t.cpp} x;
for (unsigned int k = 0; k < fieldlen; ++k) {{
    {t.sub[1].cpp} key = {t.sub[1].cpp}(buf + offset + itemsize*k, keylen);
    nullpos = key.find('\\0');
    if (nullpos != {t.sub[1].cpp}::npos)
        key.resize(nullpos);
    x[key] = VLRead<{t.sub[2].cpp}, {t.sub[2].db}>(buf + offset + itemsize*k + keylen);
}}
{H5TCLOSE}
""".strip()

MAP_VL_STRING_STRING_READER = """
hid_t field_type = H5Tget_member_type(tb_type, j);
size_t nullpos;
hsize_t fieldlen;
H5Tget_array_dims2(field_type, &fieldlen);
unsigned int itemsize = col_sizes_[table][j] / fieldlen;
unsigned int vallen = itemsize - CYCLUS_SHA1_SIZE;
{t.cpp} x;
for (unsigned int k = 0; k < fieldlen; ++k) {{
    {t.sub[2].cpp} val = {t.sub[2].cpp}(buf + offset + itemsize*k + CYCLUS_SHA1_SIZE, vallen);
    nullpos = val.find('\\0');
    if (nullpos != {t.sub[2].cpp}::npos)
        val.resize(nullpos);
    x[VLRead<{t.sub[1].cpp}, {t.sub[1].db}>(buf + offset + itemsize*k)] = val;
}}
{H5TCLOSE}
""".strip()

MAP_VL_STRING_VL_STRING_READER = """
hid_t field_type = H5Tget_member_type(tb_type, j);
size_t nullpos;
hsize_t fieldlen;
H5Tget_array_dims2(field_type, &fieldlen);
unsigned int itemsize = 2*CYCLUS_SHA1_SIZE;
{t.cpp} x;
for(unsigned int k = 0; k < fieldlen; ++k) {{
    x[VLRead<{t.sub[1].cpp}, {t.sub[1].db}>(buf + offset + itemsize*k)] = VLRead<{t.sub[2].cpp}, {t.sub[2].db}>(buf + offset + itemsize*k + CYCLUS_SHA1_SIZE);
}}
{H5TCLOSE}
""".strip()

MAP_PAIR_INT_STRING_DOUBLE_READER = """
hid_t field_type = H5Tget_member_type(tb_type, j);
size_t nullpos;
hsize_t fieldlen;
H5Tget_array_dims2(field_type, &fieldlen);
unsigned int itemsize = col_sizes_[table][j] / fieldlen;
unsigned int strlen = itemsize - sizeof({t.sub[1].sub[1].cpp}) - sizeof({t.sub[2].cpp});
{t.sub[1].cpp} key;
{t.cpp} x;
for (unsigned int k = 0; k < fieldlen; ++k) {{
    {t.sub[1].sub[2].cpp} s = {t.sub[1].sub[2].cpp}(buf + offset + itemsize*k + sizeof({t.sub[1].sub[1].cpp}), strlen);
    nullpos = s.find('\\0');
    if (nullpos != {t.sub[1].sub[2].cpp}::npos)
        s.resize(nullpos);
    key = std::make_pair(*reinterpret_cast<{t.sub[1].sub[1].cpp}*>(buf + offset + itemsize*k), s);
    x[key] = *reinterpret_cast<{t.sub[2].cpp}*>(buf + offset + itemsize*k + sizeof({t.sub[1].sub[1].cpp}) + strlen);
}}
{H5TCLOSE}
""".strip()

MAP_PAIR_INT_VL_STRING_DOUBLE_READER = """
unsigned int itemsize = sizeof({t.sub[1].sub[1].cpp}) + CYCLUS_SHA1_SIZE + sizeof({t.sub[2].cpp});
jlen = col_sizes_[table][j] / itemsize;
{t.sub[1].cpp} key;
{t.cpp} x;
for (unsigned int k = 0; k < jlen; ++k) {{
    key = std::make_pair(*reinterpret_cast<{t.sub[1].sub[1].cpp}*>(buf + offset + itemsize*k), VLRead<{t.sub[1].sub[2].cpp}, {t.sub[1].sub[2].db}>(buf + offset + itemsize*k + sizeof({t.sub[1].sub[1].cpp})));
    x[key] = *reinterpret_cast<{t.sub[2].cpp}*>(buf + offset + itemsize*k + sizeof({t.sub[1].sub[1].cpp}) + CYCLUS_SHA1_SIZE);
}}
{NO_CLOSE}
""".strip()

READERS = {'INT': REINTERPRET_CAST_READER,
           'BOOL': REINTERPRET_CAST_READER,
           'FLOAT': REINTERPRET_CAST_READER,
           'DOUBLE': REINTERPRET_CAST_READER,
           'STRING': STRING_READER,
           'VL_STRING': VL_READER,
           'BLOB': VL_READER,
           'UUID': UUID_READER,
           'VECTOR_INT': VECTOR_READER,
           'VL_VECTOR_INT': VL_READER,
           'VECTOR_DOUBLE': VECTOR_READER,
           'VL_VECTOR_DOUBLE': VL_READER,
           'VECTOR_FLOAT': VECTOR_READER,
           'VL_VECTOR_FLOAT': VL_READER,
           'VECTOR_STRING': VECTOR_STRING_READER,
           'VECTOR_VL_STRING': VECTOR_VL_STRING_READER,
           'VL_VECTOR_STRING': VL_READER,
           'VL_VECTOR_VL_STRING': VL_READER,
           'SET_INT': SET_LIST_X_READER,
           'VL_SET_INT': VL_READER,
           'SET_STRING': SET_STRING_READER,
           'VL_SET_STRING': VL_READER,
           'SET_VL_STRING': SET_VL_STRING_READER,
           'VL_SET_VL_STRING': VL_READER,
           'LIST_INT': SET_LIST_X_READER,
           'VL_LIST_INT': VL_READER,
           'LIST_STRING': LIST_STRING_READER,
           'VL_LIST_STRING': VL_READER,
           'VL_LIST_VL_STRING': VL_READER,
           'LIST_VL_STRING': LIST_VL_STRING_READER,
           'PAIR_INT_INT': PAIR_INT_INT_READER,
           'PAIR_INT_STRING': PAIR_INT_STRING_READER,
           'PAIR_INT_VL_STRING': PAIR_INT_VL_STRING_READER,
           'MAP_INT_INT': MAP_XY_READER,
           'VL_MAP_INT_INT': VL_READER,
           'MAP_INT_DOUBLE': MAP_XY_READER,
           'VL_MAP_INT_DOUBLE': VL_READER,
           'MAP_INT_STRING': MAP_INT_STRING_READER,
           'VL_MAP_INT_STRING': VL_READER,
           'MAP_INT_VL_STRING': MAP_INT_VL_STRING_READER,
           'VL_MAP_INT_VL_STRING': VL_READER,
           'MAP_STRING_INT': MAP_STRING_X_READER,
           'VL_MAP_STRING_INT': VL_READER,
           'MAP_VL_STRING_INT': MAP_VL_STRING_X_READER,
           'VL_MAP_VL_STRING_INT': VL_READER,
           'MAP_STRING_DOUBLE': MAP_STRING_X_READER,
           'VL_MAP_STRING_DOUBLE': VL_READER,
           'MAP_STRING_STRING': MAP_STRING_STRING_READER,
           'VL_MAP_STRING_STRING': VL_READER,
           'MAP_STRING_VL_STRING': MAP_STRING_VL_STRING_READER,
           'VL_MAP_STRING_VL_STRING': VL_READER,
           'MAP_VL_STRING_DOUBLE': MAP_VL_STRING_X_READER,
           'VL_MAP_VL_STRING_DOUBLE': VL_READER,
           'MAP_VL_STRING_STRING': MAP_VL_STRING_STRING_READER,
           'VL_MAP_VL_STRING_STRING': VL_READER,
           'MAP_VL_STRING_VL_STRING': MAP_VL_STRING_VL_STRING_READER,
           'VL_MAP_VL_STRING_VL_STRING': VL_READER,
           'MAP_PAIR_INT_STRING_DOUBLE': MAP_PAIR_INT_STRING_DOUBLE_READER,
           'VL_MAP_PAIR_INT_STRING_DOUBLE': VL_READER,
           'MAP_PAIR_INT_VL_STRING_DOUBLE': MAP_PAIR_INT_VL_STRING_DOUBLE_READER,
           'VL_MAP_PAIR_INT_VL_STRING_DOUBLE': VL_READER}

def indent(text, prefix, predicate=None):
    """Adds 'prefix' to the beginning of selected lines in 'text'.
    If 'predicate' is provided, 'prefix' will only be added to the lines
    where 'predicate(line)' is True. If 'predicate' is not provided,
    it will default to adding 'prefix' to all non-empty lines that do not
    consist solely of whitespace characters.
    """
    if predicate is None:
        def predicate(line):
            return line.strip()

    def prefixed_lines():
        for line in text.splitlines(True):
            yield (prefix + line if predicate(line) else line)
    return ''.join(prefixed_lines())

QUERY_CASES = ''

def main():
    global QUERY_CASES
    for ca in CANON_SET: 
        current_type = TypeStr(ca)
        reader = READERS[current_type.db]
        ctx = {"t": current_type,
               "NO_CLOSE": NO_CLOSE.format(t = current_type),
               "H5TCLOSE": H5TCLOSE.format(t = current_type),
               "H5TCLOSE_MULTI": H5TCLOSE_MULTI.format(t = current_type)}
                  
        QUERY_CASES += CASE_TEMPLATE.format(t=current_type, read_x=indent(reader.format(**ctx), INDENT))

    print(indent(QUERY_CASES, INDENT))
    
if __name__ == '__main__':
    main()

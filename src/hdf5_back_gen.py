#!/usr/bin/env python
"""This module generates HDF5 backend code found in src/hdf5_back.cc"""
import json
import textwrap

with open('../share/dbtypes.json') as f:
    RAW_TABLE = json.load(f)

V3_TABLE = set(tuple(row) for row in RAW_TABLE[897:])

def list_dep(orig):
    """Returns a list of dependent dbtypes.
    
    Keyword arguments: 
    orig -- original dbtype.
    
    First element of returned list will always be orig itself.
    """
    pieces = orig.split("_")
    if len(pieces) == 1:
        return pieces
    containers1 = ["VECTOR", "SET", "LIST", "VL_VECTOR", "VL_SET", "VL_LIST"]
    containers2 = ["MAP", "PAIR", "VL_MAP", "VL_PAIR"]

    if pieces[0] == "VL":
        del pieces[0:2]
    else:
        del pieces[0]

    for i in range(0, len(pieces)):
        if pieces[i] == "VL":
            pieces[i] = pieces[i] + "_" + pieces[i + 1]
            del pieces[i + 1]
        if i == len(pieces) - 1:
            break
    
    for i in range(len(pieces) - 1, -1, -1):
        if pieces[i] in containers1:
            pieces[i] += "_" + pieces[i + 1]
            del pieces[i + 1]
        elif pieces[i] in containers2:
            pieces[i]+= "_" + pieces[i + 1] + "_" + pieces[i + 2]
            del pieces[i + 1:i + 3]
    pieces = [orig] + pieces
    return pieces

DB_LIST = []
DB_TO_CPP = {}
INDENT = '    '

for row in V3_TABLE:
    if row[6] == 1 and row[4] == "HDF5":
        DB_LIST+=[row[1]]
        DB_TO_CPP[row[1]] = row[2]

class TypeStr(object):
    """Represents a archetype data type.
    
    Keyword Arguments:
    db -- dbtype of the data type
    
    Attributes:
    db -- dbtype of the data type
    sub -- list of dependencies as TypeStr objects (including self as element 0)
    """
    def __init__(self, db):
        self.db = db
        self.sub = [self] + [TypeStr(u) for u in list_dep(self.db)[1:]]
    
    @property
    def cpptype(self):
        """str: Returns c++ representation of data type."""
        return DB_TO_CPP[self.db]

    @property
    def dbtype(self):
        """str: Returns dbtype representation of data type."""
        return self.db

CASE_TEMPLATE = """
case {t.dbtype}: {{
{read_x}
    break;
}}"""

NO_CLOSE = """\
is_row_selected = CmpConds<{t.cpptype}>(&x, &(field_conds[qr.fields[j]]));
if (is_row_selected)
    row[j] = x;\
"""

H5TCLOSE = """\
is_row_selected = CmpConds<{t.cpptype}>(&x, &(field_conds[qr.fields[j]]));
if (is_row_selected)
    row[j] = x;
H5Tclose(field_type);\
"""

H5TCLOSE_MULTI = """\
is_row_selected = CmpConds<{t.cpptype}>(&x, &(field_conds[qr.fields[j]]));
if (is_row_selected)
    row[j] = x;
H5Tclose(key_type);
H5Tclose(item_type);
H5Tclose(field_type);\
"""

REINTERPRET_CAST_READER = """\
{t.cpptype} x = *reinterpret_cast<{t.cpptype}*>(buf + offset);
{NO_CLOSE}\
"""

STRING_READER = """\
{t.cpptype} x = {t.cpptype}(buf + offset, col_sizes_[table][j]);
size_t nullpos = x.find('\0');
if (nullpos != {t.cpptype}::npos)
    x.resize(nullpos);
{NO_CLOSE}\
"""

VL_READER = """\
{t.cpptype} x = VLRead<{t.cpptype}, {t.dbtype}>(buf + offset);
{NO_CLOSE}\
"""

UUID_READER = """\
{t.cpptype} x;
memcpy(&x, buf+offset, 16);
{NO_CLOSE}\
"""

VECTOR_READER = """\
{t.cpptype} x = {t.cpptype}(col_sizes_[table][j] / sizeof({t.sub[1].cpptype}));
memcpy(&x[0], buf + offset, col_sizes_[table][j]);
{NO_CLOSE}\
"""

VECTOR_STRING_READER = """\
hid_t field_type = H5Tget_member_type(tb_type, j);
size_t nullpos;
hsize_t fieldlen;
H5Tget_array_dims2(field_type, &fieldlen);
unsigned int strlen = col_sizes_[table][j] / fieldlen;
{t.cpptype} x = {t.cpptype}(fieldlen);
for(unsigned int k = 0; k < fieldlen; ++k) {{
    x[k] = {t.sub[1].cpptype}(buf + offset + strlen*k, strlen);
    nullpos = x[k].find('\0');
    if(nullpos != {t.sub[1].cpptype}::npos)
        x[k].resize(nullpos);'
}}
{H5TCLOSE}\
"""

VECTOR_VL_STRING_READER = """\
jlen = col_sizes_[table][j] / CYCLUS_SHA1_SIZE;
{t.cpptype} x = {t.cpptype}(jlen);
for (unsigned int k = 0; k < jlen; ++k) {{
    x[k] = VLRead<{t.sub[1].cpptype}, {t.sub[1].dbtype}>(buf + offset + CYCLUS_SHA1_SIZE*k);
}}
{NO_CLOSE}\
"""

SET_LIST_X_READER = """\
jlen = col_sizes_[table][j] / sizeof({t.sub[1].cpptype});
{t.sub[1].cpptype}* xraw = reinterpret_cast<{t.sub[1].cpptype}*>(buf + offset);
{t.cpptype} x = {t.cpptype}(xraw, xraw+jlen);
{NO_CLOSE}\
"""

SET_STRING_READER = """\
hid_t field_type = H5Tget_member_type(tb_type, j);
size_t nullpos;
hsize_t fieldlen;
H5Tget_array_dims2(field_type, &fieldlen);
unsigned int strlen = col_sizes_[table][j] / fieldlen;
{t.cpptype} x;
for(unsigned int k = 0; k < fieldlen; ++k) {{
    {t.sub[1].cpptype} s = {t.sub[1].cpptype}(buf + offset + strlen*k, strlen);
    nullpos = s.find('\0');
    if(nullpos != {t.sub[1].cpptype}::npos)
        s.resize(nullpos);
    x.insert(s);
}}
{H5TCLOSE}\
"""

SET_VL_STRING_READER = """\
jlen = col_sizes_[table][j] / CYCLUS_SHA1_SIZE;
{t.cpptype} x;
for (unsigned int k = 0; k < jlen; ++k) {{
    x.insert(VLRead<{t.sub[1].cpptype}, {t.sub[1].dbtype}>(buf + offset + CYCLUS_SHA1_SIZE*k));
}}
{NO_CLOSE}\
"""

LIST_STRING_READER = """\
hid_t field_type = H5Tget_member_type(tb_type, j);
size_t nullpos;
hsize_t fieldlen;
H5Tget_array_dims2(field_type, &fieldlen);
unsigned int strlen = col_sizes_[table][j] / fieldlen;
{t.cpptype} x;
for(unsigned int k = 0; k < fieldlen; ++k) {{
    {t.sub[1].cpptype} s = {t.sub[1].cpptype}(buf + offset + strlen*k, strlen);
    nullpos = s.find('\0');
    if(nullpos != {t.sub[1].cpptype}::npos)
        s.resize(nullpos);
    x.push_back(s);
}}
{H5TCLOSE}\
"""

LIST_VL_STRING_READER = """\
jlen = col_sizes_[table][j] / CYCLUS_SHA1_SIZE;
{t.cpptype} x;
for (unsigned int k = 0; k < jlen; ++k) {{
    x.push_back(VLRead<{t.sub[1].cpptype}, {t.sub[1].dbtype}>(buf + offset + CYCLUS_SHA1_SIZE*k));
}}
{NO_CLOSE}\
"""

PAIR_INT_INT_READER = """\
{t.cpptype} x = std::make_pair(*reinterpret_cast<{t.sub[1].cpptype}*>(buf + offset), *reinterpret_cast<{t.sub[1].cpptype}*>(buf + offset + sizeof({t.sub[1].cpptype})));
{NO_CLOSE}\
"""

PAIR_INT_STRING_READER = """\
size_t nullpos;
unsigned int strlen = col_sizes_[table][j] - sizeof(int);
{t.sub[1].cpptype} xfirst = *reinterpret_cast<{t.sub[1].cpptype}*>(buf + offset);
{t.sub[2].cpptype} s = {t.sub[2].cpptype}(buf + offset + sizeof(int), strlen);
nullpos = s.find('\0');
if(nullpos != {t.sub[2].cpptype}::npos)
    s.resize(nullpos);
{t.cpptype} x = std::make_pair(xfirst, s);
{NO_CLOSE}\
"""

PAIR_INT_VL_STRING_READER = """\
unsigned int itemsize = sizeof(int) + CYCLUS_SHA1_SIZE;
jlen = col_sizes_[table][j] / itemsize;
{t.cpptype} x = std::make_pair(*reinterpret_cast<{t.sub[1].cpptype}*>(buf + offset), VLRead<{t.sub[2].cpptype}, {t.sub[2].dbtype}>(buf + offset + sizeof(int)));
{NO_CLOSE}\
"""

MAP_XY_READER = """\
{t.cpptype} x = {t.cpptype}();
size_t itemsize = sizeof({t.sub[1].cpptype}) + sizeof({t.sub[2].cpptype})
jlen = col_sizes_[table][j] / itemsize;
for (unsigned int k = 0; k < jlen; ++k) {{
    x[*reinterpret_cast<{t.sub[1].cpptype}*>(buf + offset + itemsize*k)] = *reinterpret_cast<{t.sub[2].cpptype}*>(buf + offset + itemsize*k + sizeof({t.sub[1].cpptype}));
}}
{NO_CLOSE}\
"""

MAP_INT_STRING_READER = """\
hid_t fieldtype = H5Tget_member_type(tb_type, j);
size_t nullpos;
hsize_t fieldlen;
H5Tget_array_dims2(field_type, &fieldlen);
unsigned int itemsize = col_sizes_[table][j] / fieldlen;
unsigned int strlen = itemsize - sizeof(int);
{t.cpptype} x;
for (unsigned int k = 0; k < fieldlen; ++k) {{
    {t.sub[2].cpptype} s = {t.sub[2].cpptype}(buf + offset + itemsize*k + sizeof({t.sub[1].cpptype}), strlen);
    nullpos = s.find('\0');
    if(nullpos != {t.sub[2].cpptype}::npos)
        s.resize(nullpos);
    x[*reinterpret_cast<{t.sub[1].cpptype}*>(buf + offset + itemsize*k)] = s;
}}
{H5TCLOSE}\
"""

MAP_INT_VL_STRING_READER = """\
unsigned int itemsize = sizeof({t.sub[1].cpptype}) + CYCLUS_SHA1_SIZE;
jlen = col_sizes_[table][j] / itemsize;
{t.cpptype} x;
for(unsigned int k = 0; k < jlen; ++k) {{
    x[*reinterpret_cast<{t.sub[1].cpptype}*>(buf + offset + itemsize*k)] = VLRead<{t.sub[2].cpptype}, {t.sub[2].dbtype}>(buf + offset + itemsize*k + sizeof({t.sub[1].cpptype}));
}}
{NO_CLOSE}\
"""

MAP_STRING_X_READER = """\
hid_t field_type = H5get_member_type(tb_type, j);
size_t nullpos;
hsize_t fieldlen;
H5Tget_array_dims2(field_type, &fieldlen);
unsigned int itemsize = col_sizes_[table][j] / fieldlen;
unsigned int strlen = itemsize - sizeof({t.sub[2].cpptype});
{t.cpptype} x;
for (unsigned int k = 0; k < fieldlen; ++k) {{
    {t.sub[1].cpptype} s = {t.sub[1].cpptype}(buf + offset + itemsize*k, strlen);
    nullpos = s.find('\0');
    if (nullpos != {t.sub[1].cpptype}::npos)
        s.resize(nullpos);
    x[s] = *reinterpret_cast<{t.sub[2].cpptype}*>(buf + offset + itemsize*k + strlen);
}}
{H5TCLOSE}\
"""

MAP_VL_STRING_X_READER =  """\
unsigned int itemsize = sizeof({t.sub[2].cpptype}) + CYCLUS_SHA1_SIZE;
jlen = col_sizes_[table][j] / itemsize;
{t.cpptype} x;
for (unsigned int k = 0; k < jlen; ++k) {{
    x[VLRead<{t.sub[1].cpptype}, {t.sub[1].dbtype}>(buf + offset + itemsize*k)] = *reinterpret_cast<{t.sub[2].cpptype}*>(buf + offset + itemsize*k + CYCLUS_SHA1_SIZE);
}}
{NO_CLOSE}\
"""

MAP_STRING_STRING_READER = """\
hid_t field_type = H5Tget_member_type(tb_type, j);
hid_t item_type = H5Tget_super(field_type);
hid_t key_type = H5Tget_member_type(item_type, 0);
size_t nullpos;
hsize_t fieldlen;
H5Tget_array_dims2(field_type, &fieldlen);
unsigned int itemsize = col_sizes_[table][j] / fieldlen;
unsigned int keylen = H5Tget_size(key_type);
unsigned int vallen = itemsize - keylen;
{t.cpptype} x;
for (unsigned int k = 0; k < fieldlen; ++k) {{
    {t.sub[1].cpptype} key = {t.sub[1].cpptype}(buf + offset + itemsize*k, keylen);
    nullpos = key.find('\0');
    if (nullpos != {t.sub[1].cpptype}::npos)
        key.resize(nullpos);
    {t.sub[2].cpptype} val = {t.sub[2].cpptype}(buf + offset + itemsize*k + keylen, vallen);
    nullpos = val.find('\0');
    if (nullpos != {t.sub[2].cpptype}::npos)
        val.resize(nullpos);
    x[key] = val;
}}
{H5TCLOSE_MULTI}\
"""

MAP_STRING_VL_STRING_READER = """\
hid_t field_type = H5Tget_member_type(tb_type, j);
size_t nullpos;
hsize_t fieldlen;
H5Tget_array_dims2(field_type, &fieldlen);
unsigned int itemsize = col_sizes_[table][j] / fieldlen;
unsigned int keylen = itemsize - CYCLUS_SHA1_SIZE;
{t.cpptype} x;
for (unsigned int k = 0; k < fieldlen; ++k) {{
    {t.sub[1].cpptype} key = {t.sub[1].cpptype}(buf + offset + itemsize*k, keylen);
    nullpos = key.find('\0');
    if (nullpos != {t.sub[1].cpptype}::npos)
        key.resize(nullpos);
    x[key] = VLRead<{t.sub[2].cpptype}, {t.sub[2].dbtype}>(buf + offset + itemsize*k + keylen);
}}
{H5TCLOSE}\
"""

MAP_VL_STRING_STRING_READER = """\
hid_t field_type = H5Tget_member_type(tb_type, j);
size_t nullpos;
hsize_t fieldlen;
H5Tget_array_dims2(field_type, &fieldlen);
unsigned int itemsize = col_sizes_[table][j] / fieldlen;
unsigned int vallen = itemsize - CYCLUS_SHA1_SIZE;
{t.cpptype} x;
for (unsigned int k = 0; k < fieldlen; ++k) {{
    {t.sub[2].cpptype} val = {t.sub[2].cpptype}(buf + offset + itemsize*k + CYCLUS_SHA1_SIZE, vallen);
    nullpos = val.find('\0');
    if (nullpos != {t.sub[2].cpptype}::npos)
        val.resize(nullpos);
    x[VLRead<{t.sub[1].cpptype}, {t.sub[1].dbtype}>(buf + offset + itemsize*k)] = val;
}}
{H5TCLOSE}\
"""

MAP_VL_STRING_VL_STRING_READER = """\
hid_t field_type = H5Tget_member_type(tb_type, j);
size_t nullpos;
hsize_t fieldlen;
H5Tget_array_dims2(field_type, &fieldlen);
unsigned int itemsize = 2*CYCLUS_SHA1_SIZE;
{t.cpptype} x;
for(unsigned int k = 0; k < fieldlen; ++k) {{
    x[VLRead<{t.sub[1].cpptype}, {t.sub[1].dbtype}>(buf + offset + itemsize*k)] = VLRead<{t.sub[2].cpptype}, {t.sub[2].dbtype}>(buf + offset + itemsize*k + CYCLUS_SHA1_SIZE);
}}
{H5TCLOSE}\
"""

MAP_PAIR_INT_STRING_DOUBLE_READER = """\
hid_t field_type = H5Tget_member_type(tb_type, j);
size_t nullpos;
hsize_t fieldlen;
H5Tget_array_dims2(field_type, &fieldlen);
unsigned int itemsize = col_sizes_[table][j] / fieldlen;
unsigned int strlen = itemsize - sizeof({t.sub[1].sub[0].cpptype}) - sizeof({t.sub[2].cpptype});
{t.sub[1].cpptype} key;
{t.cpptype} x;
for (unsigned int k = 0; k < fieldlen; ++k) {{
    {t.sub[1].sub[1].cpptype} s = {t.sub[1].sub[1].cpptype}(buf + offset + itemsize*k + sizeof({t.sub[1].sub[0].cpptype}), strlen);
    nullpos = s.find('\0');
    if (nullpos != {t.sub[1].sub[1].cpptype}::npos)
        s.resize(nullpos);
    key = std::make_pair(*reinterpret_cast<{t.sub[1].sub[0].cpptype}*>(buf + offset + itemsize*k), s);
    x[key] = *reinterpret_cast<{t.sub[2].cpptype}*>(buf + offset + itemsize*k + sizeof({t.sub[1].sub[0].cpptype}) + strlen);
}}
{H5TCLOSE}\
"""

MAP_PAIR_INT_VL_STRING_DOUBLE_READER = """\
unsigned int itemsize = sizeof({t.sub[1].sub[0].cpptype}) + CYCLUS_SHA1_SIZE + sizeof({t.sub[2].cpptype});
jlen = col_sizes_[table][j] / itemsize;
{t.sub[1].cpptype} key;
{t.cpptype} x;
for (unsigned int k = 0; k < jlen; ++k) {{
    key = std::make_pair(*reinterpret_cast<{t.sub[1].sub[0].cpptype}*>(buf + offset + itemsize*k), VLREAD<{t.sub[1].sub[1].cpptype}, {t.sub[1].sub[1].dbtype}>(buf + offset + itemsize*k + sizeof({t.sub[1].sub[0].cpptype})));
    x[key] = *reinterpret_cast<{t.sub[2].cpptype}*>(buf + offset + itemsize*k + sizeof({t.sub[1].sub[0].cpptype}) + CYCLUS_SHA1_SIZE);
}}
{NO_CLOSE}\
"""

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

QUERY_CASES = ''

def main():
    global QUERY_CASES
    for db in DB_LIST: 
        current_type = TypeStr(db)
        reader = READERS[db]
        ctx = {"t": current_type,
               "NO_CLOSE": NO_CLOSE,
               "H5TCLOSE": H5TCLOSE,
               "H5TCLOSE_MULTI": H5TCLOSE_MULTI}
        QUERY_CASES += CASE_TEMPLATE.format(t = current_type, read_x = textwrap.indent(reader.format(**ctx), INDENT))

    print(QUERY_CASES)
    
if __name__ == '__main__':
    main()

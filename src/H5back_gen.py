import json
import textwrap

with open('../share/dbtypes.json') as f:
    rawtable = json.load(f)

table = set(tuple(row[:-3]) for row in rawtable[1:])
v3_table = set(tuple(row) for row in rawtable[897:])

def parse_template(s, open_brace = '<', close_brace = '>', separator = ','):
    s = s.replace(' ', '')
    if open_brace not in s and separator not in s:
        return s
    i = s.find(open_brace)
    j = s.rfind(close_brace)
    t = [s[:i]]
    inner = s[i+1:j]
    t.extend(parse_arg(inner, open_brace, close_brace, separator))
    if isinstance(t, str):
        return t
    else:
        return tuple(t)

def parse_arg(s, open_brace = '<', close_brace = '>', separator = ','):
    nest = 0
    ts = []
    start = 0
    for i in range(len(s)):
        ch = s[i]
        if ch == open_brace:
            nest += 1
        elif ch == close_brace:
            nest -= 1
        if ch == separator and nest == 0:
            t = parse_template(s[start:i], open_brace, close_brace, separator)
            ts.append(t)
            start = i+1 
    if start < len(s):
        t = parse_template(s[start:], open_brace, close_brace, separator)
        ts.append(t)
    return ts

def list_dep(s):
    pieces = s.split("_")
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
    pieces = [s] + pieces
    return pieces

dbtest = []
db_to_cpp = {}
cpptypes = {}
canon = {}

for row in v3_table:
    if row[6] == 1 and row[4] == "HDF5":
        dbtest+=[row[1]]
        #print(tuple(parse_template(row[2])))
        current = parse_template(row[2])
        #print(current)
        #print(len(current))
        cpptypes[current] = row[2]
        canon[row[1]] = current
        db_to_cpp[row[1]] = row[2]

class TypeStr(object):
    def __init__(self, db):
        self.db = db
        self.canon = canon[self.db]
        self.sub = [self] + [TypeStr(u) for u in list_dep(self.db)[1:]]

    def __str__(self):
        return str(canon[self.db])
    
    @property
    def cpptype(self):
        return db_to_cpp[self.db]

    @property
    def dbtype(self):
        return self.db

case_template = """
case {t.dbtype}: {{
{read_x}
    is_row_selected = CmpConds<{t.cpptype}>(&x, &(field_conds[qr.fields[j]]));
    if (is_row_selected)
        row[j] = x;
    break;
}}"""

indent = '    '

reinterpret_cast_reader = '{t.cpptype} x = *reinterpret_cast<{t.cpptype}*>(buf + offset);'

string_reader = """\
{t.cpptype} x = {t.cpptype}(buf + offset, col_sizes_[table][j]);
size_t nullpos = x.find('\0');
if (nullpos != {t.cpptype}::npos)
    x.resize(nullpos);\
"""
vl_reader = '{t.cpptype} x = VLRead<{t.cpptype}, {t.dbtype}>(buf + offset);'

uuid_reader = '{t.cpptype} x;' + '\nmemcpy(&x, buf+offset, 16);'

vector_reader = '{t.cpptype} x = {t.cpptype}(col_sizes_[table][j] / sizeof({t.sub[1].cpptype})); \nmemcpy(&x[0], buf + offset, col_sizes_[table][j]);'

vector_string_reader = """\
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
}}\
"""

vector_vl_string_reader = """\
jlen = col_sizes_[table][j] / CYCLUS_SHA1_SIZE;
{t.cpptype} x = {t.cpptype}(jlen);
for (unsigned int k = 0; k < jlen; ++k) {{
    x[k] = VLRead<{t.sub[1].cpptype}, {t.sub[1].dbtype}>(buf + offset + CYCLUS_SHA1_SIZE*k);
}}\
"""

set_list_reader = """\
jlen = col_sizes_[table][j] / sizeof({t.sub[1].cpptype});
{t.sub[1].cpptype}* xraw = reinterpret_cast<{t.sub[1].cpptype}*>(buf + offset);
{t.cpptype} x = {t.cpptype}(xraw, xraw+jlen);\
"""

#changed x.insert(s) to x.insert(k, s) to allow for list and set use
set_list_string_reader = """\
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
    x.insert(k, s);
}}\
"""

#changed insert method to insert(k, value) to allow for list use
set_list_vl_string_reader = """\
jlen = col_sizes_[table][j] / CYCLUS_SHA1_SIZE;
{t.cpptype} x;
for (unsigned int k = 0; k < jlen; ++k) {{
    x.insert(k, VLRead<{t.sub[1].cpptype}, {t.sub[1].dbtype}>(buf + offset + CYCLUS_SHA1_SIZE*k));
}}\
"""

pair_int_int_reader = '{t.cpptype} x = std::make_pair(*reinterpret_cast<{t.sub[1].cpptype}*>(buf + offset), *reinterpret_cast<{t.sub[1].cpptype}*>(buf + offset + siizeof({t.sub[1].cpptype})));'

pair_int_string_reader = """\
size_t nullpos;
unsigned int strlen = col_sizes_[table][j] - sizeof(int);
{t.sub[1].cpptype} xfirst = *reinterpret_cast<{t.sub[1].cpptype}*>(buf + offset);
{t.sub[2].cpptype} s = {t.sub[2].cpptype}(buf + offset + sizeof(int), strlen);
nullpos = s.find('\0');
if(nullpos != {t.sub[2].cpptype}::npos)
    s.resize(nullpos);
{t.cpptype} x = std::make_pair(xfirst, s);\
"""

pair_int_vl_string_reader = """\
unsigned int itemsize = sizeof(int) + CYCLUS_SHA1_SIZE;
jlen = col_sizes_[table][j] / itemsize;
{t.cpptype} x = std::make_pair(*reinterpret_cast<{t.sub[1].cpptype}*>(buf + offset), VLRead<{t.sub[2].cpptype}, {t.sub[2].dbtype}>(buf + offset + sizeof(int)));\
"""

map_xy_reader = """\
{t.cpptype} x = {t.cpptype}();
size_t itemsize = sizeof({t.sub[1].cpptype}) + sizeof({t.sub[2].cpptype})
jlen = col_sizes_[table][j] / itemsize;
for (unsigned int k = 0; k < jlen; ++k) {{
    x[*reinterpret_cast<{t.sub[1].cpptype}*>(buf + offset + itemsize*k)] = *reinterpret_cast<{t.sub[2].cpptype}*>(buf + offset + itemsize*k + sizeof({t.sub[1].cpptype}));
}}\
"""

map_int_string_reader = """\
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
}}\
"""

map_int_vl_string_reader = """\
unsigned int itemsize = sizeof({t.sub[1].cpptype}) + CYCLUS_SHA1_SIZE;
jlen = col_sizes_[table][j] / itemsize;
{t.cpptype} x;
for(unsigned int k = 0; k < jlen; ++k) {{
    x[*reinterpret_cast<{t.sub[1].cpptype}*>(buf + offset + itemsize*k)] = VLRead<{t.sub[2].cpptype}, {t.sub[2].dbtype}>(buf + offset + itemsize*k + sizeof({t.sub[1].cpptype}));
}}\
"""

map_string_x_reader = """\
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
}}\
"""

map_vl_string_x_reader =  """\
unsigned int itemsize = sizeof({t.sub[2].cpptype}) + CYCLUS_SHA1_SIZE;
jlen = col_sizes_[table][j] / itemsize;
{t.cpptype} x;
for (unsigned int k = 0; k < jlen; ++k) {{
    x[VLRead<{t.sub[1].cpptype}, {t.sub[1].dbtype}>(buf + offset + itemsize*k)] = *reinterpret_cast<{t.sub[2].cpptype}*>(buf + offset + itemsize*k + CYCLUS_SHA1_SIZE);
}}\
"""

map_string_string_reader = """\
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
}}\
"""

map_string_vl_string_reader = """\
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
}}\
"""

map_vl_string_string_reader = """\
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
}}\
"""

map_vl_string_vl_string_reader = """\
hid_t field_type = H5Tget_member_type(tb_type, j);
size_t nullpos;
hsize_t fieldlen;
H5Tget_array_dims2(field_type, &fieldlen);
unsigned int itemsize = 2*CYCLUS_SHA1_SIZE;
{t.cpptype} x;
for(unsigned int k = 0; k < fieldlen; ++k) {{
    x[VLRead<{t.sub[1].cpptype}, {t.sub[1].dbtype}>(buf + offset + itemsize*k)] = VLRead<{t.sub[2].cpptype}, {t.sub[2].dbtype}>(buf + offset + itemsize*k + CYCLUS_SHA1_SIZE);
}}\
"""

readers = {'INT': reinterpret_cast_reader,
           'BOOL': reinterpret_cast_reader,
           'FLOAT': reinterpret_cast_reader,
           'DOUBLE': reinterpret_cast_reader,
           'STRING': string_reader,
           'VL_STRING': vl_reader,
           'BLOB': vl_reader,
           'UUID': uuid_reader,
           'VECTOR_INT': vector_reader,
           'VL_VECTOR_INT': vl_reader,
           'VECTOR_DOUBLE': vector_reader,
           'VL_VECTOR_DOUBLE': vl_reader,
           'VECTOR_FLOAT': vector_reader,
           'VL_VECTOR_FLOAT': vl_reader,
           'VECTOR_STRING': vector_string_reader,
           'VECTOR_VL_STRING': vector_vl_string_reader,
           'VL_VECTOR_STRING': vl_reader,
           'VL_VECTOR_VL_STRING': vl_reader,
           'SET_INT': set_list_reader,
           'VL_SET_INT': vl_reader,
           'SET_STRING': set_list_string_reader,
           'VL_SET_STRING': vl_reader,
           'SET_VL_STRING': set_list_vl_string_reader,
           'VL_SET_VL_STRING': vl_reader,
           'LIST_INT': set_list_reader,
           'VL_LIST_INT': vl_reader,
           'LIST_STRING': set_list_string_reader,
           'VL_LIST_STRING': vl_reader,
           'VL_LIST_VL_STRING': vl_reader,
           'LIST_VL_STRING': set_list_vl_string_reader,
           'PAIR_INT_INT': pair_int_int_reader,
           'PAIR_INT_STRING': pair_int_string_reader,
           'PAIR_INT_VL_STRING': pair_int_vl_string_reader,
           'MAP_INT_INT': map_xy_reader,
           'VL_MAP_INT_INT': vl_reader,
           'MAP_INT_DOUBLE': map_xy_reader,
           'VL_MAP_INT_DOUBLE': vl_reader,
           'MAP_INT_STRING': map_int_string_reader,
           'VL_MAP_INT_STRING': vl_reader,
           'MAP_INT_VL_STRING': map_int_vl_string_reader,
           'VL_MAP_INT_VL_STRING': vl_reader,
           'MAP_STRING_INT': map_string_x_reader,
           'VL_MAP_STRING_INT': vl_reader,
           'MAP_VL_STRING_INT': map_vl_string_x_reader,
           'VL_MAP_VL_STRING_INT': vl_reader,
           'MAP_STRING_DOUBLE': map_string_x_reader,
           'VL_MAP_STRING_DOUBLE': vl_reader,
           'MAP_STRING_STRING': map_string_string_reader,
           'VL_MAP_STRING_STRING': vl_reader,
           'MAP_STRING_VL_STRING': map_string_vl_string_reader,
           'VL_MAP_STRING_VL_STRING': vl_reader,
           'MAP_VL_STRING_DOUBLE': map_vl_string_x_reader,
           'VL_MAP_VL_STRING_DOUBLE': vl_reader,
           'MAP_VL_STRING_STRING': map_vl_string_string_reader,
           'VL_MAP_VL_STRING_STRING': vl_reader,
           'MAP_VL_STRING_VL_STRING': map_vl_string_vl_string_reader,
           'VL_MAP_VL_STRING_VL_STRING': vl_reader}

query_cases = ''
for db in dbtest: 
    current_type = TypeStr(db)
    reader = readers[db]
    #reader.format(t=db)
    #ctx = {}
    #ctx['read_x'] = textwrap.indent(reader.format(t =current_type), indent)
    query_cases += case_template.format(t = current_type, read_x = textwrap.indent(reader.format(t = current_type), indent))

print(query_cases)


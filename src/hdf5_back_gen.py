#!/usr/bin/env python
"""This module generates HDF5 backend code found in src/hdf5_back.cc"""
import os
import json
import textwrap
from ast import literal_eval
from pprint import pformat

is_primitive = lambda t: isinstance(t.canon, str)

class Node(object):
    fields = ()
    
    def __init__(self, **kwargs):
        seen = set()
        for field, value in kwargs.items():
            if field not in self.fields:
                raise RuntimeError
            setattr(self, field, value)
            seen.add(field)
        for field in self.fields:
            if field not in seen:
                setattr(self, field, None)           
    
    def __str__(self):
        return PrettyFormatter(self).visit()

class Block(Node):
    fields = ("nodes",)

class Var(Node):
    fields = ("name",)
    
class Type(Node):
    fields = ("cpp", "db", "canon")

class Decl(Node):
    fields = ("type", "name")
    
class Expr(Node):
    fields = ("value",)

class ExprStmt(Node):
    fields = ("child",)

class Assign(Node):
    fields = ("target", "value")

class DeclAssign(Node):
    fields = ("type", "target", "value")

class Case(Node):
    fields = ("cond", "body")
    
class If(Node):
    fields = ("cond", "body", "elifs", "el")

#how to allow ';' as decl    
class For(Node):
    fields = ("adecl", "cond", "incr", "body")
    
class BinOp(Node):
    fields = ("x", "op", "y")

class LeftUnaryOp(Node):
    fields = ("op", "name")

class RightUnaryOp(Node):
    fields = ("name", "op")

class FuncCall(Node):
    #targs means template args
    fields = ("name", "args", "targs") 
    
class Raw(Node):
    #for cheating and literals
    fields = ("code",)
    
class Nothing(Node):
    #for "nothing"
    fields = ()

_lowername = lambda cls: cls.__name__.lower()

class Visitor(object):
    """Super-class for all classes that should walk over a tree of nodes.
    This implements the visit() method.
    """

    def __init__(self, tree=None):
        self.tree = tree

    def visit(self, node=None):
        """Walks over a node.  If no node is provided, the tree is used."""
        if node is None:
            node = self.tree
        if node is None:
            raise RuntimeError('no node or tree given!')
        for clsname in map(_lowername, type.mro(node.__class__)):
            meth = getattr(self, 'visit_' + clsname, None)
            if callable(meth):
                rtn = meth(node)
                break
        else:
            msg = 'could not find valid visitor method for {0} on {1}'
            nodename = node.__class__.__name__ 
            selfname = self.__class__.__name__
            msg = msg.format(nodename, selfname)
            try:
                msg += "\n"
                msg += str(node)
            except Exception:
                pass    
            raise AttributeError(msg)
        return rtn

class PrettyFormatter(Visitor):
    """Formats a tree of nodes into a pretty string"""

    def __init__(self, tree=None, indent=' '):
        super().__init__(tree=tree)
        self.level = 0
        self.indent = indent

    def visit_node(self, node):
        s = node.__class__.__name__ + '('
        if len(node.fields) == 0:
            return s + ')'
        s += '\n'
        self.level += 1
        t = []
        for field in node.fields:
            a = getattr(node, field)
            t.append(self.visit(a) if isinstance(a, Node) else pformat(a))
        t = ['{0}={1}'.format(n, x) for n, x in zip(node.fields, t)]
        s += textwrap.indent(',\n'.join(t), self.indent)
        self.level -= 1
        s += '\n)'
        return s

class CppGen(Visitor):
    def __init__(self, tree=None, indent='  '):
        super().__init__(tree=tree)
        self.level = 0
        self.indent = indent
        
    def visit_var(self, node):
        return node.name
    
    def visit_type(self, node):
        return node.cpp
        
    def visit_decl(self, node):
        s = self.visit(node.type)
        s += " "
        s += self.visit(node.name)
        s += ";"
        return s
    
    def visit_exprstmt(self, node):
        s = self.visit(node.child)
        s += ";\n"
        return s
    
    def visit_assign(self, node):
        s = self.visit(node.target)
        s += " = "
        s += self.visit(node.value)
        s += ";"
        return s
    
    def visit_declassign(self, node):
        s = self.visit(node.type)
        s += " "
        s += self.visit(node.target)
        s += "="
        s += self.visit(node.value)
        s += ";"
        return s
    
    def visit_binop(self, node):
        s = self.visit(node.x)
        #s += " "
        s += node.op
        #s += " "
        s += self.visit(node.y)
        return s
        
    def visit_leftunaryop(self, node):
        s = node.op
        s += self.visit(node.name)
        return s
        
    def visit_rightunaryop(self, node):
        s = self.visit(node.name)
        s += node.op
        return s
        
    def visit_raw(self, node):
        s = node.code
        return s
    
    def visit_case(self, node):
        s = "case "
        s += self.visit(node.cond)
        s += ": {\n"
        for n in node.body:
            s += textwrap.indent(self.visit(n), self.indent)
        s += "}\n"
        return s
        
    def visit_if(self, node):
        s = "if("
        s += self.visit(node.cond)
        s += "){\n"
        for n in node.body:
            s += textwrap.indent(self.visit(n), self.indent)
        s += "\n}"
        #check if elifs is an empty list
        if node.elifs:
            for cond, body in node.elifs:
                s += "else if("
                s += self.visit(cond)
                s += "){\n"
                for n in body:
                    b = ""
                    b += self.visit(n)
                    s += textwrap.indent(b, self.indent)
                s += "\n}"  
        #check if else attribute exists
        if node.el is not None:
            s += "else{\n"
            s += textwrap.indent(self.visit(node.el), self.indent)
            s += "\n}"
        return s
        
    def visit_for(self, node):
        s = "for("
        if node.adecl is not None:
            s += self.visit(node.adecl)
        else:
            s += ";" 
        s += self.visit(node.cond)
        s += ";"
        s += self.visit(node.incr)
        s += "){\n"
        for n in node.body:
            s += textwrap.indent(self.visit(n), self.indent)
        s += "\n}"
        return s
        
    def visit_funccall(self, node):
        s = self.visit(node.name)
        if node.targs is not None:
            s += "<"
            for i in range(0, len(node.targs)):
                #print(node.targs[i])
                #print(self.visit(node.targs[i]))
                s += self.visit(node.targs[i])
                if i < (len(node.targs)-1):
                    s += ","
            s += ">"
        s += "("
        for i in range(0, len(node.args)):
            s += self.visit(node.args[i])
            if i < (len(node.args)-1):
                s += ","
        s += ")"
        return s
    
    def visit_nothing(self, node):
        return ""
        
    def visit_block(self, node):
        for n in node.nodes:
            if n !isinstance(list)

with open(os.path.join(os.path.dirname(__file__), '..', 'share', 'dbtypes.json')) as f:
    RAW_TABLE = json.load(f)

TABLE_START = 0
for row in range(0, len(RAW_TABLE)):
    current = tuple(RAW_TABLE[row])
    if current[5] == "v1.3" and current[4] == "HDF5":
        TABLE_START = row
        break

V3_TABLE = list(tuple(row) for row in RAW_TABLE[TABLE_START:])

CANON_TO_NODE = {}
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
        db = row[1]
        cpp = row[2]
        canon = convert_canonical(row[7])
        CANON_SET.add(canon)
        DB_TO_CPP[db] = cpp
        CANON_TO_DB[canon] = db
        CANON_TO_NODE[canon] = Type(cpp=cpp, db=db, canon=canon)
        

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

def normal_close(t):
    """
    is_row_selected = CmpConds<{t.cpp}>>(&x, &(field_conds[qr.fields[j]]));
    if(is_row_selected)
        row[j] = x;
    """
    tree = Block(nodes=[
        Assign(target=Var(name="is_row_selected"),
               value=FuncCall(name=Var(name="CmpConds"), targs=[t],
                              args=[Raw(code="&x"),
                                    Raw(code="&(field_conds[qr.fields[j]]))")])),
        If(cond=Var(name="is_row_selected"),
           body=[Assign(target=Var(name="row[j]"), value=Var(name="x"))])])
    return tree

def case_template(t, read_x):
    """
    case {t.db}: {
        {read_x}
        break;
    }
    """
    if isinstance(read_x, Block):
        body = read_x.nodes
    else:
        body = read_x
    body += [ExprStmt(child=Var(name="break"))]
    tree = Case(cond=Var(name=t.db), body=body)
    return tree

def reinterpret_cast_reader(t):
    """
    {t.cpp} x = *reinterpret_cast<{t.cpp}*>(buf+offset);
    {teardown}
    """
    tree = Block(nodes=[
                 DeclAssign(type=t, target=Var(name="x"), 
                            value=FuncCall(name=Var(name="reinterpret_cast"),
                            targs=[t], args=[Raw(code="buf+offset")])),
                 create_teardown(t)])
    return tree
    
#REINTERPRET_CAST_READER = """
#{t.cpp}{*1} xraw = {*2}reinterpret_cast<{t.cpp}*>(buf+offset);
#{teardown}
#""".strip()

def string_reader(t):
    """
    {left_side} = {t.cpp}(buf + offset, col_sizes_[table][j]);
    size_t nullpos = {left_side}.find('\\0');
    if (nullpos != {t.cpp}::npos)
        {left_side}.resize(nullpos);
    {teardown}
    """
    tree = Block(nodes=[
                 DeclAssign(type=t, target=Var(name="x"), 
                            value=FuncCall(name=t.cpp,
                                    args=[Raw(code="buf+offset"),
                                          Raw(code="col_sizes_[table][j]")])),
                 DeclAssign(type=Raw(code="size_t"), 
                            target=Var(name="nullpos"),
                            value=BinOp(x=Var(name="x"), op=".", 
                                        y=FuncCall(name=Raw(code="find"),
                                                   args=[Raw(code="'\\0'")]))),
                 If(cond=BinOp(x=Var(name="nullpos"), op="!=",
                               y=BinOp(x=t.cpp, op="::", y=Raw(code="npos"))), 
                    body=[BinOp(x=Var(name="x"), op=".", 
                                y=FuncCall(name=Raw(code="resize"),
                                           args=[Raw(code="nullpos")]))]),
                 create_teardown(t)])
    return tree      

def vl_string_reader(t):
    """
    {left_side} x = VLRead<{t.cpp}, {t.db}>(buf + offset {cyclus_constant});
    {teardown}
    """
    tree = Block(nodes=[
                 DeclAssign(type=t, target=Var(name="x"), 
                            value=FuncCall(name=Raw(code="VLRead"),
                               args=[Raw(code="buf+offset+CYCLUS_SHA1_SIZE")],
                               targs=[t])),
                 create_teardown(t)])
    return tree

def uuid_reader(t):
    """
    {t.cpp} x;
    memcpy(&x, buf+offset, 16);
    {teardown}
    """
    tree = Block(nodes=[
                 Decl(type=t, name=Var(name="x")),
                 ExprStmt(child=FuncCall(name=Raw(code="memcpy"), 
                          args=[Raw(code="&x"), 
                                Raw(code="buf+offset"), Raw(code="16")])),
                 create_teardown(t)])
    return tree

def vector_reader(t):
    """
    {t.cpp} x = {t.cpp}(col_sizes_[table][j] / sizeof({t.sub[1].cpp}));
    memcpy(&x[0], buf + offset, col_sizes_[table][j]);
    {NO_CLOSE}
    """
    tree = Block(nodes=[
                 #setup
                 get_setup("VECTOR", t),
                 #decl
                 get_decl("VECTOR", t),
                 #body
                 get_body("VECTOR", t),
                 #close
                 normal_close(t)])
    return tree

VECTOR_READER = """
{setup}
{t.cpp} x;
{body}
{teardown}
""".strip()

def set_reader(t):
    tree = Block(nodes=[
                 Assign(target=Var(name="jlen"), 
                        value=BinOp(x=Raw(code="col_sizes_[table][j]"),
                                    op="/", y=FuncCall(name=Raw(code="sizeof"),
                                            args=[Raw(code=t.sub[1].cpp)]))),
                 get_body(t),
                 ])
    return tree
    
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

#setup functions

def primitive_setup(depth=0, prefix="", t):
    jlen = "jlen" + str(depth) + prefix
    node = Block(nodes=[
        Assign(target=Var(name=jlen),
               value=Raw(code="col_sizes_[table][j] / sizeof("+t.cpp+")"))])
    return node

def string_setup(depth=0, prefix=""): 
    field_type = "field_type" + str(depth) + prefix
    nullpos = "nullpos" + str(depth) + prefix
    fieldlen = "fieldlen" + str(depth) + prefix
    strlen = "strlen" + str(depth) + prefix
    
    node = Block(nodes=[
        DeclAssign(typ=Type(cpp="hid_t"), 
                   target=Var(name=fieldlen), 
                   value=FuncCall(name=Raw(code="H5Tget_member_type"),
                                  args=[Raw(code="tb_type"), 
                                  Raw(code="j")])),
        Decl(type=Type(cpp="size_t"), name=Var(name=nullpos),
        Decl(type=Type(cpp="hsize_t"), name=Var(name=fieldlen),
        FuncCall(name=Raw(code="H5Tget_array_dims2"),
                 args=[Raw(code=field_type), Raw(code="&"+fieldlen)]),
        DeclAssign(type=Type(cpp="unsigned int"), 
                   target=Var(name=strlen),
                   value=Raw(code="col_sizes_[table][j] / "+fieldlen))])
    return node

def vl_string_setup(depth=0, prefix=""):
    jlen = "jlen" + str(depth) + prefix
    node = Block(nodes=[
        Assign(target=Var(name=jlen), 
               value=Raw(code="col_sizes_[table][j] / CYCLUS_SHA1_SIZE"))])
    return node

def get_setup(t, depth=0, prefix=""):
    node = Node()
    if is_primitive(t):
        if t.sub[0] == "STRING":
            node = string_setup(depth, prefix)
        elif t.sub[0] == "VL_STRING":
            node = vl_string_setup(depth, prefix)
        else
            node = primitive_setup(depth, prefix, t)
    else:
        node = Block(nodes=[get_setup(i, depth=depth+1, prefix=prefix+part) for i, part in zip(t.sub[1:], template_args[t.sub[0]])])
    return node
    
#declaration

def get_decl(t, depth=0, prefix=""):
    variable = "x" + str(depth) + prefix
    node = Decl(type=t, name=Var(name=variable))
    return node

#bodies

#to-do: fill these out
def def_body(t, depth=0, prefix=""):
    pass

def memcpy_body(t, depth=0, prefix=""):
    pass

def elementwise_body(t, depth=0, prefix=""):
    pass

def vl_body(t, depth=0, prefix=""):
    pass

def vec_string_body(t, depth=0, prefix=""):
    pass
    
def set_string_body(t, depth=0, prefix=""):
    pass

template_args = {"MAP": ("KEY", "VALUE"),
                 "VECTOR": ("ELEM",),
                 "SET": ("ELEM",),
                 "LIST": ("ELEM",),
                 "PAIR": ("ITEM1", "ITEM2")}

BODIES = {"INT": def_body,
          "DOUBLE": memcpy_body,
          "STRING": elementwise_body,
          "VL_STRING": vl_body,
          "VECTOR_STRING": vec_string_body,
          "SET_STRING": set_string_body}

def get_body(t, depth=0, prefix=""):
    block = []
    block.append(get_decl(t, depth, prefix))
    if is_primitive(t):
        return BODIES[t.db](t, depth, prefix)
    elif t in BODIES:
        return BODIES[t.db](t, depth, prefix)
    else:
        for i, part in zip(t.sub[1:], template_args[t[0]]):
            new_prefix = prefix + part
            block.append(get_body(i, depth=depth+1, prefix=new_prefix))
        block.insert(0, initial_decl)
        return Block(nodes=block)


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
           "fieldlen": create_fieldlen(a_type).format(t=a_type.sub[1])}
    
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
    setup_as_str = PrettyFormatter.visit(setup)
    if "hid_t" not in setup_as_str:
        return (closure + "\nbreak;")
    else:
        for node in setup.nodes:
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

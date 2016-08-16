#!/usr/bin/env python
"""This module generates HDF5 backend code found in src/hdf5_back.cc"""
import os
import json
from ast import literal_eval
from pprint import pformat

is_primitive = lambda t: isinstance(t.canon, str)

class Node(object):
    fields = ()
    
    def __init__(self, **kwargs):
        seen = set()
        for field, value in kwargs.items():
            if field not in self.fields:
                print(field, " is not a valid field")
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

class Line(Node):
    fields = ("child",)

class Assign(Node):
    fields = ("target", "value")

class DeclAssign(Node):
    fields = ("type", "target", "value")

class Case(Node):
    fields = ("cond", "body")
    
class If(Node):
    fields = ("cond", "body", "elifs", "el")
   
class For(Node):
    fields = ("adecl", "cond", "incr", "body")
    
class BinOp(Node):
    fields = ("x", "op", "y")

class LeftUnaryOp(Node):
    fields = ("op", "name")

class RightUnaryOp(Node):
    fields = ("name", "op")

class FuncCall(Node):
    # targs means template args
    fields = ("name", "args", "targs") 
    
class Raw(Node):
    # for cheating and literals
    fields = ("code",)
    
class Nothing(Node):
    # for "nothing"
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
        s += indent(',\n'.join(t), self.indent)
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
        return s
    
    def visit_exprstmt(self, node):
        s = self.visit(node.child)
        s += ";\n"
        return s
    
    def visit_assign(self, node):
        s = self.visit(node.target)
        s += "="
        s += self.visit(node.value)
        return s
    
    def visit_declassign(self, node):
        s = self.visit(node.type)
        s += " "
        s += self.visit(node.target)
        s += "="
        s += self.visit(node.value)
        return s
    
    def visit_binop(self, node):
        s = self.visit(node.x)
        # s += " "
        s += node.op
        # s += " "
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
            s += indent(self.visit(n), self.indent)
        s += "\n}\n"
        return s
        
    def visit_if(self, node):
        s = "if("
        s += self.visit(node.cond)
        s += "){\n"
        for n in node.body:
            s += indent(self.visit(n), self.indent)
        s += "\n}"
        # check if elifs is an empty list
        if node.elifs:
            for cond, body in node.elifs:
                s += "else if("
                s += self.visit(cond)
                s += "){\n"
                for n in body:
                    b = ""
                    b += self.visit(n)
                    s += indent(b, self.indent)
                s += "\n}"  
        # check if else attribute exists
        if node.el is not None:
            s += "else{\n"
            s += indent(self.visit(node.el), self.indent)
            s += "\n}"
        return s + "\n"
        
    def visit_for(self, node):
        s = "for("
        if node.adecl is not None:
            s += self.visit(node.adecl) + ";"
        else:
            s += ";" 
        s += self.visit(node.cond)
        s += ";"
        s += self.visit(node.incr)
        s += "){\n"
        for n in node.body:
            s += indent(self.visit(n), self.indent)
        s += "\n}\n"
        return s
        
    def visit_funccall(self, node):
        s = self.visit(node.name)
        if node.targs is not None:
            s += "<"
            for i in range(len(node.targs)):
                # print(node.targs[i])
                # print(self.visit(node.targs[i]))
                s += self.visit(node.targs[i])
                if i < (len(node.targs)-1):
                    s += ","
            s += ">"
        s += "("
        for i in range(len(node.args)):
            s += self.visit(node.args[i])
            if i < (len(node.args)-1):
                s += ","
        s += ")"
        return s
    
    def visit_nothing(self, node):
        return ""
        
    def visit_block(self, node):
        s = ""
        for n in node.nodes:
            s += self.visit(n)
        return s  

def resolve_unicode(item):	   
    # Python3, if we can handle it, don't bother.    
    if isinstance(item, str):
        return item        
    # We must check every element in tuples and lists.    
    elif isinstance(item, tuple):
        return tuple([resolve_unicode(i) for i in item])
    elif isinstance(item, list):
        return [resolve_unicode(i) for i in item]
    # Not a string, either unicode (Python2.7) or an int.    
    else: 
        try:
            return item.encode('utf-8')
        except Exception:
            pass
        return item

with open(os.path.join(os.path.dirname(__file__), '..', 'share', 'dbtypes.json')) as f:
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

CANON_TO_NODE = {}
CANON_SET = set()
DB_TO_CPP = {}
CANON_TO_DB = {}
DB_TO_VL = {}
INDENT = '    '

def convert_canonical(raw_list):
    if isinstance(raw_list, str):
        return raw_list
    return tuple(convert_canonical(x) for x in raw_list)
        
for row in TYPES_TABLE:
    if row[6] == 1 and row[4] == "HDF5" and row[5] == VERSION:        
        db = row[1]
        cpp = row[2]
        canon = convert_canonical(row[7])
        CANON_SET.add(canon)
        DB_TO_CPP[db] = cpp
        CANON_TO_DB[canon] = db
        CANON_TO_NODE[canon] = Type(cpp=cpp, db=db, canon=canon)
        DB_TO_VL[db] = row[8]

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
    This function represents the generic close to an hdf5 type
    code block.
 
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

def reinterpret_cast_reader(t, depth=0, prefix="", variable="x", offset="buf+offset"):
    """
    This function represents a primitive reader using the reinterpret_cast
    method. This includes int, double, float, etc.
    
    {t.cpp} x = *reinterpret_cast<{t.cpp}*>(buf+offset);
    """
    
    
    tree = Block(nodes=[
                 ExprStmt(child=Assign(target=Var(name=variable), 
                            value=FuncCall(name=Raw(code="*reinterpret_cast"),
                            targs=[Raw(code=t.cpp+"*")], 
                            args=[Raw(code=offset)])))])
    return tree

def string_reader(t, depth=0, prefix="", variable="x", offset="buf+offset", col_size="col_sizes_[table][j]"):
    """
    This function represents the reader for the string primitive.
    
    {left_side} = {t.cpp}(buf + offset, col_sizes_[table][j]);
    size_t nullpos = {left_side}.find('\\0');
    if (nullpos != {t.cpp}::npos)
        {left_side}.resize(nullpos);
    {teardown}
    """
    nullpos = "nullpos" + str(depth) + prefix
    
    tree = Block(nodes=[
                 ExprStmt(child=Assign(target=Var(name=variable),
                                       value=FuncCall(name=Raw(code=t.cpp),
                                            args=[Raw(code=offset),
                                                  Raw(code=col_size)]))),
                 ExprStmt(child=Assign(target=Var(name=nullpos),
                                       value=BinOp(x=Var(name=variable), 
                                              op=".", 
                                              y=FuncCall(name=Raw(code="find"),
                                                  args=[Raw(code="'\\0'")])))),
                 If(cond=BinOp(x=Var(name=nullpos), op="!=",
                               y=BinOp(x=Raw(code=t.cpp), op="::",
                                       y=Raw(code="npos"))), 
                    body=[ExprStmt(child=BinOp(x=Var(name=variable), op=".", 
                                y=FuncCall(name=Raw(code="resize"),
                                           args=[Raw(code=nullpos)])))])])
    return tree      

def vl_string_reader(t, depth=0, prefix="", variable="x", offset="buf+offset"):
    """
    This function represents the reader for the vl_string primitive.
    
    {left_side} x = VLRead<{t.cpp}, {t.db}>(buf + offset {cyclus_constant});
    {teardown}
    """
    tree = Block(nodes=[
                 ExprStmt(child=Assign(target=Var(name=variable), 
                            value=FuncCall(name=Raw(code="VLRead"),
                               args=[Raw(code=offset)],
                               targs=[Raw(code=t.cpp), Raw(code=t.db)])))])
    return tree

def uuid_reader(t):
    """
    This function represents the reader for the boost uuid primitive.
    
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
    This function represents the reader for the vector container type.
    
    {t.cpp} x = {t.cpp}(col_sizes_[table][j] / sizeof({t.sub[1].cpp}));
    memcpy(&x[0], buf + offset, col_sizes_[table][j]);
    {NO_CLOSE}
    """
    tree = Block(nodes=[
                 get_setup("VECTOR", t),
                 get_decl("VECTOR", t),
                 get_body("VECTOR", t),
                 normal_close(t)])
    return tree

# setup functions

def primitive_setup(t, depth=0, prefix=""):
    """
    This function represents the setup for primitive types.
    """
    jlen = "jlen" + str(depth) + prefix
    node = Block(nodes=[
        ExprStmt(child=Assign(target=Var(name=jlen),
               value=Raw(code="col_sizes_[table][j] / sizeof("+t.cpp+")")))])
    return node

def string_setup(depth=0, prefix=""): 
    """
    This function represents the setup for the string primitive.
    """
    field_type = "field_type" + str(depth) + prefix
    nullpos = "nullpos" + str(depth) + prefix
    fieldlen = "fieldlen" + str(depth) + prefix
    strlen = "strlen" + str(depth) + prefix
    
    node = Block(nodes=[
        ExprStmt(child=DeclAssign(type=Type(cpp="hid_t"), 
                   target=Var(name=fieldlen), 
                   value=FuncCall(name=Raw(code="H5Tget_member_type"),
                                  args=[Raw(code="tb_type"), 
                                  Raw(code="j")]))),
        ExprStmt(child=Decl(type=Type(cpp="size_t"), name=Var(name=nullpos))),
        ExprStmt(child=Decl(type=Type(cpp="hsize_t"), name=Var(name=fieldlen))),
        ExprStmt(child=FuncCall(name=Raw(code="H5Tget_array_dims2"),
                 args=[Raw(code=field_type), Raw(code="&"+fieldlen)])),
        ExprStmt(child=DeclAssign(type=Type(cpp="unsigned int"), 
                   target=Var(name=strlen),
                   value=Raw(code="col_sizes_[table][j] / "+fieldlen)))])
    return node

def vl_string_setup(depth=0, prefix=""):
    """
    This function represents the setup for the vl_string primitive.
    """
    jlen = "jlen" + str(depth) + prefix
    node = Block(nodes=[
        ExprStmt(child=Assign(target=Var(name=jlen), 
               value=Raw(code="col_sizes_[table][j] / CYCLUS_SHA1_SIZE")))])
    return node

template_args = {"MAP": ("KEY", "VALUE"),
                 "VECTOR": ("ELEM",),
                 "SET": ("ELEM",),
                 "LIST": ("ELEM",),
                 "PAIR": ("ITEM1", "ITEM2")}

def get_setup(t, depth=0, prefix=""):
    """
    This function is the dispatch for various setups. Primitives are directly
    setup, while template types are setup recursively.
    """
    node = Node()
    setup_nodes = []
    if is_primitive(t):
        setup_nodes.append(ExprStmt(child=DeclAssign(type=Type(cpp="unsigned int"), 
                                                     target=Var(name="item_size" + str(depth) + prefix), 
                                                     value=Raw(code=get_size(t, depth, prefix)))))
        if t.canon == "STRING":
            setup_nodes.append(string_setup(depth, prefix))
        elif t.canon == "VL_STRING":
            setup_nodes.append(vl_string_setup(depth, prefix))
        else:
            setup_nodes.append(primitive_setup(t, depth, prefix))
        node = Block(nodes=setup_nodes)
    else:
        item_size_variable = "item_size" + str(depth) + prefix
        item_size = ExprStmt(child=DeclAssign(type=Type(cpp="unsigned int"),
                                              target=Var(name=item_size_variable),
                                              value=Raw(code=get_size(t, depth, prefix))))
        jlen_variable = "jlen" + str(depth) + prefix
        jlen = ExprStmt(child=Assign(target=Var(name=jlen_variable),
                                     value=Raw(code="col_sizes_[table][j] / " + item_size_variable)))
        setup_nodes.append(item_size)
        setup_nodes.append(jlen)
        setup_nodes.append(Block(nodes=[get_setup(CANON_TO_NODE[i], depth=depth+1, prefix=prefix+part) 
                                        for i, part in zip(t.canon[1:], template_args[t.canon[0]])]))
        node = Block(nodes=setup_nodes)
    return node

variable_length_types = ["MAP", "LIST", "SET", "VECTOR"]

current_type_has_string = False

def get_size(t, depth=0, prefix=""):
    global current_type_has_string
    s = ""
    if is_primitive(t):
        if t.canon =="VL_STRING":
            s = "CYCLUS_SHA1_SIZE"
        elif t.canon == "STRING":
            s = "strlen" + str(depth) + prefix
            current_type_has_string = True
        else:
            s = "sizeof(" + str.lower(t.canon) + ")"
    else:
        pieces = []
        s += "("
        zipped = list(zip(t.canon[1:], template_args[the_type[0]]))
        for index in range(len(zipped)):
            i = zipped[index][0]
            part = zipped[index][1]
            new_prefix = prefix + part
            s += get_size(i, depth=depth+1, prefix=new_prefix)
            if index != len(zipped) - 1:
                s += "+"
        s += ")"
        if t.canon[0] in variable_length_types and depth != 0:
            multiplier = "fieldlen" if current_type_has_string else "jlen"
            s += "*" + multiplier + str(depth) + prefix
            current_type_has_string = False
    return s
    

# declaration

def get_decl(t, depth=0, prefix=""):
    """
    This function is the dispatch for declarations. Declarations occur
    directly before bodies, so they are created without recursion.
    """
    variable = "x" + str(depth) + prefix
    node = ExprStmt(child=Decl(type=t, name=Var(name=variable)))
    return node

# bodies

# to-do: fill these out
def def_body(t, depth=0, prefix=""):
    """
    This function represents a body which requires only an assignment
    elem decl
    elem body
    x = {t.cpp}(pointer to elem, pointer to elem + jlen);
    """
    elem = CANON_TO_NODE[t.canon[1]]
    elem_prefix = prefix + template_args[t.canon[0]]
    elem_var = "x" + str(depth + 1) + elem_prefix
    x = "x" + str(depth) + prefix
    jlen = "jlen" + str(depth) + prefix
    
    node = Block(nodes=[get_body(elem, depth=depth+1, prefix=elem_prefix),
                        ExprStmt(child=Assign(
                                    target=Raw(code=x),
                                    value=FuncCall(
                                        name=Raw(code=t.cpp),
                                        args=[Raw(code="buf+offset")])))])
    return node

def memcpy_body(t, depth=0, prefix=""):
    """
    This function represents the generic memcpy body. 
    elem decl
    elem body
    x = {t.cpp}(col_sizes_[table][j] / {fieldlen});
    memcpy(&x[0], buf + offset, col_sizes_[table][j]);
    """
    variable = "x" + str(depth) + prefix
    
    elem = CANON_TO_NODE[t.canon[1]]
    elem_prefix = prefix + template_args[t.canon[0]][0]
    arg = "col_sizes_[table][j]/sizeof(" + elem.cpp +")"
    
    node = Block(nodes=[get_body(elem, depth=depth+1, prefix=elem_prefix),
                        ExprStmt(child=Assign(
                                       target=Raw(code=x),
                                       value=FuncCall(name=Raw(code=t.cpp),
                                                      args=[Raw(code=arg)]))),
                        ExprStmt(child=FuncCall(
                                    name=Raw(code="memcpy"),
                                    args=[Raw(code="&"+x+"[0]"),
                                          Raw(code="buf+offset"), 
                                          Raw(code="col_sizes_[table][j]")]))])
    return node

def elementwise_body(t, depth=0, prefix=""):
    """
    x = t.cpp(fieldlen)
    for(unsigned int k = 0; k < fieldlen; ++k) {
        elem decl
        elem body // using x[k]
    }
    """
    pass
    
def map_body(t, depth=0, prefix=""):
    """
    for(unsigned int k = 0; k < jlen; ++k) {
        key decl
        key body
        val decl
        val body
        x[key] = val
    }
    """
    x = "x" + str(depth) + prefix
    k = "k" + str(depth) + prefix
    
    key = CANON_TO_NODE[t.canon[1]]
    value = CANON_TO_NODE[t.canon[2]]
    key_prefix = prefix + template_args[t.canon[0]][0]
    value_prefix = prefix + template_args[t.canon[0]][1]
    key_name = "x" + str(depth + 1) + key_prefix
    value_name = "x" + str(depth + 1) + value_prefix
    
    #we need to figure out what to do for fieldlen here. This variable changes
    #depending on data type of the key and value. String is fieldlen, otherwise
    #we get jlen.
    node = Block(nodes=[
          For(adecl=DeclAssign(type=Type(cpp="unsigned int"), 
                               target=Var(name=k), 
                               value=Raw(code="0")),
              cond=BinOp(x=Var(name=k), op="<", y=Var(name="fieldlen")),
              incr=LeftUnaryOp(op="++", name=Var(name=k)),
              body=[
                get_body(key, depth=depth+1, prefix=key_prefix),
                get_body(value, depth=depth+1, prefix=value_prefix),
                ExprStmt(child=Assign(target=Raw(code=x+"["+key_name+"]"),
                                      value=Raw(code=value_name)))])])
    return node

def pair_body(t, depth=0, prefix=""):
    """
    item1 decl
    item1 body
    item2 decl
    item2 body
    x = std::make_pair(item1, item2);
    """
    x = "x" + str(depth) + prefix
    
    item1 = CANON_TO_NODE[t.canon[1]]
    item2 = CANON_TO_NODE[t.canon[2]]
    
    item1_prefix = prefix + template_args[t.canon[0]][0]
    item2_prefix = prefix + template_args[t.canon[0]][1]
    
    item1_name = "x" + str(depth + 1) + item1_prefix
    item2_name = "x" + str(depth + 1) + item2_prefix
    
    node = Block(nodes=[get_body(item1, depth=depth+1, prefix=item1_prefix), 
                        get_body(item2, depth=depth+1, prefix=item2_prefix),
                        ExprStmt(child=Assign(
                            target=Raw(code=x),
                            value=FuncCall(name=Raw(code="std::make_pair"),
                                           args=[Raw(code=item1_name),
                                                 Raw(code=item2_name)])))])
    return node

def vl_body(t, depth=0, prefix=""):
    x = "x" + str(depth) + prefix
    node = Block(nodes=[ExprStmt(child=Assign(target=Var(name=x),
                            value=FuncCall(name=Var(name="VLRead"),
                                           args=[Raw(code="buf+offset")],
                                           targs=[Raw(code=t.cpp), 
                                                  Raw(code=t.db)])))])
    return node

def vec_string_body(t, depth=0, prefix=""):
    x = "x" + str(depth) + prefix
    k = "k" + str(depth) + prefix
    index = x + "[" + k + "]"
    fieldlen = "fieldlen" + str(depth+1) + "ELEM"
    strlen = "strlen" + str(depth+1) + "ELEM"
    node = Block(nodes=[
          For(adecl=DeclAssign(type=Type(cpp="unsigned int"), 
                               target=Var(name=k), 
                               value=Raw(code="0")),
              cond=BinOp(x=Var(name=k), op="<", y=Var(name=fieldlen)),
              incr=LeftUnaryOp(op="++", name=Var(name=k)),
              body=[
                string_reader(CANON_TO_NODE[t.canon[1]], depth=depth+1,
                              prefix="ELEM", variable=index,
                              offset="buf+offset+"+strlen+"*"+k,
                              col_size=strlen)
                ])])
    return node

def vec_vl_string_body(t, depth=0, prefix=""):
    x = "x" + str(depth) + prefix
    k = "k" + str(depth) + prefix
    index = x + "[" + k + "]"
    jlen = "jlen" + str(depth+1) + "ELEM"
    node = Block(nodes=[
          For(adecl=DeclAssign(type=Type(cpp="unsigned int"), 
                               target=Var(name=k), 
                               value=Raw(code="0")),
              cond=BinOp(x=Var(name=k), op="<", y=Var(name=jlen)),
              incr=LeftUnaryOp(op="++", name=Var(name=k)),
              body=[
                vl_string_reader(CANON_TO_NODE[t.canon[1]], depth=depth+1,
                              prefix="ELEM", variable=index,
                              offset="buf+offset+CYCLUS_SHA1_SIZE*"+k)
                ])])
    return node
    
def set_string_body(t, depth=0, prefix=""):
    x = "x" + str(depth) + prefix
    k = "k" + str(depth) + prefix
    s = "s" + str(depth+1) + "ELEM"
    fieldlen = "fieldlen" + str(depth+1) + "ELEM"
    strlen = "strlen" + str(depth+1) + "ELEM"
    node = Block(nodes=[
          For(adecl=DeclAssign(type=Type(cpp="unsigned int"),
                               target=Var(name=k), 
                               value=Raw(code="0")),
              cond=BinOp(x=Var(name=k), op="<", y=Var(name=fieldlen)),
              incr=LeftUnaryOp(op="++", name=Var(name=k)),
              body=[
                string_reader(CANON_TO_NODE[t.canon[1]], depth=depth+1,
                              prefix="ELEM", variable=s,
                              offset="buf+offset+"+strlen+"*"+k,
                              col_size=strlen),
                ExprStmt(child=FuncCall(name=Raw(code=x+".insert"), 
                                        args=[Raw(code=s)]))])])
    return node
    
def list_string_body(t, depth=0, prefix=""):
    x = "x" + str(depth) + prefix
    k = "k" + str(depth) + prefix
    s = "s" + str(depth+1) + "ELEM"
    fieldlen = "fieldlen" + str(depth+1) + "ELEM"
    strlen = "strlen" + str(depth+1) + "ELEM"
    node = Block(nodes=[ 
          For(adecl=DeclAssign(type=Type(cpp="unsigned int"), 
                               target=Var(name=k), 
                               value=Raw(code="0")),
              cond=BinOp(x=Var(name=k), op="<", y=Var(name=fieldlen)),
              incr=LeftUnaryOp(op="++", name=Var(name=k)),
              body=[
                string_reader(CANON_TO_NODE[t.canon[1]], depth=depth+1,
                              prefix="ELEM", variable=s, 
                              offset="buf+offset+"+strlen+"*"+k, 
                              col_size=strlen),
                ExprStmt(child=FuncCall(name=Raw(code=x+".pushback"),
                                        args=[Raw(code=s)]))])])
    return node

BODIES = {"INT": def_body,
          "DOUBLE": memcpy_body,
          "STRING": elementwise_body,
          "VL_STRING": vl_body,
          "VECTOR_STRING": vec_string_body,
          "SET_STRING": set_string_body}

def get_body(t, depth=0, prefix="", base_offset="buf+offset"):
    block = []
    block.append(get_decl(t, depth, prefix))
    if is_primitive(t):
        if depth == 0:
            pass # eventually do primitive
        else:
            return BODIES[t.db](t, depth, prefix)
    # catch vl types here:
    elif DB_TO_VL[t.db]:
        return vl_body(t, depth, prefix)
    elif t.db in BODIES:
        return BODIES[t.db](t, depth, prefix)
    else:
        for i, part in zip(t.canon[1:], template_args[t[0]]):
            new_prefix = prefix + part
            block.append(get_body(CANON_TO_NODE[i], depth=depth+1, prefix=new_prefix))
        block.insert(0, initial_decl)
        return Block(nodes=block)

def get_teardown():
    return

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

TEMPLATE_BODY = """
for (unsigned int k = 0; k < {fieldlen} ++k) {
    {sub_body}
}
""".strip()

QUERY_CASES = ''

def indent(text, prefix, predicate=None):
    """This function copied from textwrap library version 3.3.

    Adds 'prefix' to the beginning of selected lines in 'text'.
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
    s = "hello world"
    print(indent(s, INDENT)) 
    
if __name__ == '__main__':
    main()

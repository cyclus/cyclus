#!/usr/bin/env python
"""This module generates HDF5 backend code found in src/hdf5_back.cc"""
import os
import sys
import json
from pprint import pformat
from itertools import chain
from collections import OrderedDict

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
    
class FuncDef(Node):
    fields = ("type", "name", "args", "targs", "body", "tspecial")
    
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
        super(PrettyFormatter, self).__init__(tree=tree)
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
        super(CppGen, self).__init__(tree=tree)
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
                s += self.visit(node.targs[i])
                if i < (len(node.targs)-1):
                    s += ","
            s += ">"
        s += "("
        for i in range(len(node.args)):
            s += self.visit(node.args[i])
            if i < (len(node.args)-1):
                s += ", "
        s += ")"
        return s
    
    def visit_funcdef(self, node):
        s = ""
        if node.tspecial:
            s += "template<>\n"
        elif node.targs is not None:
            s += "template<"
            for i in range(len(node.targs)):
                s += self.visit(t)
                if i < (len(node.targs)-1):
                    s += ","
            s += ">\n"
            #We've taken care of the targs and no longer need them to appear
            node.targs = None
        s += self.visit(node.type)
        s += " "
        f = FuncCall(name=node.name, args=node.args, targs=node.targs)
        s += self.visit(f)
        b = Block(nodes=node.body)
        s += " {\n"
        s += indent(self.visit(b), self.indent)
        s += "}\n"
        return s
    
    def visit_nothing(self, node):
        return ""
        
    def visit_block(self, node):
        s = ""
        for n in node.nodes:
            s += self.visit(n)
        return s  

def resolve_unicode(item):	   
    """Translate unicode types into string types, if necessary.
    
    This function exists to support Python 2.7.
    
    Parameters
    ----------
    item : int or str or list
        The list of items, or item to potentially encode.
        
    Returns
    -------
    int or str or list
        The same type as was passed to the function, encoded if necessary
    """   
    if isinstance(item, str):
        return item         
    elif isinstance(item, tuple):
        return tuple([resolve_unicode(i) for i in item])
    elif isinstance(item, list):
        return [resolve_unicode(i) for i in item]
    else: 
        try:
            return item.encode('utf-8')
        except Exception:
            pass
        return item

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

CANON_TO_NODE = {}
CANON_SET = set()
DB_TO_CPP = {}
CANON_TO_DB = {}
DB_TO_VL = {}
INDENT = '  '

def convert_canonical(raw_list):
    """Converts JSON list of lists to tuple of tuples.
    
    Parameters
    ----------
    raw_list : list or str
        List to be converted, or str
    
    Returns
    -------
    str or tuple
        Converted list, or str
        """
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
    """Return a list of a type's dependencies, each in canonical form.
    
    Parameters
    ----------
    canon : tuple or str
        the canonical form of the type
        
    Returns
    -------
    list or str
        list of dependencies or str if base type is primitive  
    
    Examples:
    >>> list_dep("('PAIR', 'INT', 'VL_STRING')")
    [('PAIR', 'INT', 'VL_STRING'), 'INT', 'VL_STRING']
    """
    if isinstance(canon, str):
        return canon
    
    dependency_list = [u for u in canon[1:]]
    return [canon] + dependency_list 

def get_variable(name, depth=0, prefix=""):
    """Return a C++ variable, appropriately formatted for depth.
    
    Parameters
    ----------
    name : str
        Base variable name.
    depth : int, optional
        Depth of variable in relation to depth 0 type.
    prefix : str, optional
        New prefix to add, based on direct parent type.
        
    Returns
    -------
    str
        Variable name.
    
    """ 
    return name + str(depth) + prefix

def get_prefix(base_prefix, parent_type, child_index):
    """Return the prefix of a C++ variable, appropriately formatted for depth.
    
    Parameters
    ----------
    base_prefix : str
        Prefix of direct parent type.
    parent_type : Type
        Node of parent type.
    child_index : int
        Index relative to direct parent.
        
    Returns
    -------
    str
        New prefix.
    """
    return base_prefix + template_args[parent_type.canon[0]][child_index]

def case_template(t, read_x):
    """Represents C++ case statement.
    
    Parameters
    ----------
    t : Type
        Depth 0 type.
    read_x : Node
        Nodes of case statement body.
    
    Returns
    -------
    Node
        Complete case statement block.
    """
    if isinstance(read_x, Block):
        body = read_x.nodes
    else:
        body = [read_x]
    body.append(ExprStmt(child=Var(name="break")))
    node = Case(cond=Var(name=t.db), body=body)
    return node

def primitive_setup(t, depth=0, prefix=""):
    """HDF5 Query: Represents necessary setup steps for C++ primitives."""
    node = Nothing()
    return node

def string_setup(depth=0, prefix=""): 
    """HDF5 Query: Represents necessary setup steps for C++ String."""
    nullpos = "nullpos" + str(depth) + prefix
    
    node = Block(nodes=[
        ExprStmt(child=Decl(type=Type(cpp="size_t"), name=Var(name=nullpos)))])
    return node

def vl_string_setup(depth=0, prefix=""):
    """HDF5 Query: Represents necessary setup steps for C++ VL_String."""
    
    node = Block(nodes=[Nothing()])
    return node

template_args = {"MAP": ("key", "val"),
                 "VECTOR": ("elem",),
                 "SET": ("elem",),
                 "LIST": ("elem",),
                 "PAIR": ("first", "second")}

variable_length_types = ["MAP", "LIST", "SET", "VECTOR"]

def get_setup(t, depth=0, prefix="", HDF5_type="tb_type", child_index='j'):
    """HDF5 Query: Get nodes representing C++ setup. 
    
    Primitive setups are called directly, while template types are handled
    recursively.
    
    Parameters
    ----------
    t : Type
        C++ type, canonical form.
    depth : int, optional
        Depth relative to initial, depth 0 type.
    prefix : str, optional
        Current prefix, determined by parent type.
    HDF5_type : str
        hid_t type used to access HDF5 methods
    child_index : str or int
        Index into parent type, None if only child
        
    Returns
    -------
    Block
        Nodes required for type t setup.
    """
    
    node = Node()
    setup_nodes = []
    
    if not child_index is None:
        field_type_var = get_variable("fieldtype", depth=depth, prefix=prefix)
        field_type = ExprStmt(child=DeclAssign(
                                        type=Type(cpp="hid_t"),
                                        target=Var(name=field_type_var),
                                        value=FuncCall(
                                            name=Raw(code="H5Tget_member_type"),
                                            args=[Raw(code=HDF5_type),
                                                  Raw(code=str(child_index))])))
        HDF5_type = field_type_var
    
    total_size_var = get_variable("total_size", depth=depth, prefix=prefix)
    total_size = ExprStmt(child=DeclAssign(type=Type(cpp="unsigned int"),
                                           target=Var(name=total_size_var),
                                           value=FuncCall(
                                              name=Raw(code="H5Tget_size"), 
                                              args=[Raw(code=HDF5_type)])))
    if is_primitive(t):
        if t.canon == "STRING":
            setup_nodes.append(string_setup(depth=depth, prefix=prefix))
        elif t.canon == "VL_STRING":
            setup_nodes.append(vl_string_setup(depth=depth, prefix=prefix))
        else:
            setup_nodes.append(primitive_setup(t, depth=depth, prefix=prefix))
        if not child_index is None:
            setup_nodes.append(field_type)
            TEARDOWN_STACK.append(field_type_var)
        setup_nodes.append(total_size)
        node = Block(nodes=setup_nodes)
    else:
        if DB_TO_VL[t.db]:
            return Nothing()
        
        multi_items = (len(t.canon[1:]) > 1)
        
        children = len(t.canon) - 1

        if not child_index is None:
            setup_nodes.append(field_type)
            TEARDOWN_STACK.append(field_type_var)
        
        setup_nodes.append(total_size)
        
        if t.canon[0] in variable_length_types:
            fieldlen_var = get_variable("fieldlen", depth=depth, prefix=prefix)
            fieldlen = Block(nodes=[ExprStmt(child=Decl(
                                                  type=Type(cpp="hsize_t"),
                                                  name=Var(name=fieldlen_var))),
                                    ExprStmt(child=FuncCall(
                                           name=Raw(code="H5Tget_array_dims2"),
                                           args=[Raw(code=HDF5_type),
                                                 Raw(code="&"+fieldlen_var)]))])
            setup_nodes.append(fieldlen)
            item_type_var = get_variable("item_type", depth=depth, 
                                         prefix=prefix)
            item_type = ExprStmt(child=DeclAssign(
                                        type=Type(cpp="hid_t"),
                                        target=Var(name=item_type_var),
                                        value=FuncCall(
                                            name=Raw(code="H5Tget_super"),
                                            args=[Raw(code=HDF5_type)])))
            setup_nodes.append(item_type)
            TEARDOWN_STACK.append(item_type_var)
            HDF5_type = item_type_var
            if multi_items:
                setup_nodes.append(Block(nodes=[get_setup(
                                                CANON_TO_NODE[new_type], 
                                                depth=depth+1, 
                                                prefix=prefix+part,
                                                HDF5_type=HDF5_type,
                                                child_index=index) 
                                            for new_type, part, index in zip(
                                                t.canon[1:], 
                                                template_args[t.canon[0]],
                                                [i for i in range(children)])]))
            else:
                setup_nodes.append(Block(nodes=[get_setup(
                                                CANON_TO_NODE[new_type], 
                                                depth=depth+1, 
                                                prefix=prefix+part,
                                                HDF5_type=HDF5_type,
                                                child_index=None) 
                                            for new_type, part in zip(
                                                t.canon[1:], 
                                                template_args[t.canon[0]])]))
        else:
            setup_nodes.append(Block(nodes=[get_setup(
                                                CANON_TO_NODE[new_type], 
                                                depth=depth+1, 
                                                prefix=prefix+part,
                                                HDF5_type=HDF5_type,
                                                child_index=index) 
                                            for new_type, part, index in zip(
                                                t.canon[1:], 
                                                template_args[t.canon[0]],
                                                [i for i in range(children)])]))
        
        node = Block(nodes=setup_nodes)
    return node

def get_decl(t, depth=0, prefix=""):
    """HDF5 Query: Get node representing C++ type declaration. 
    
    Declarations occur directly before bodies, created without recursion.
    
    Parameters
    ----------
    t : Type
        C++ type, canonical form.
    depth : int, optional
        Depth relative to initial, depth 0 type.
    prefix : str, optional
        Prefix determined by parent type.
        
    Returns
    -------
    Node
        Declaration statement as a node.
    """
    variable = get_variable("x", depth=depth, prefix=prefix)
    node = ExprStmt(child=Decl(type=t, name=Var(name=variable)))
    return node

def reinterpret_cast_body(t, depth=0, prefix="", base_offset="buf+offset"):
    """HDF5 Query: Represents a body using the reinterpret_cast method.
    
    This includes int, double, float, etc.
    """
    x = get_variable("x", depth=depth, prefix=prefix)
    tree = Block(nodes=[
                 ExprStmt(child=Assign(
                              target=Var(name=x), 
                              value=FuncCall(name=Raw(code="*reinterpret_cast"),
                                             targs=[Raw(code=t.cpp+"*")], 
                                             args=[Raw(code=base_offset)])))])
    return tree

def string_body(t, depth=0, prefix="", base_offset="buf+offset", variable=None):
    """HDF5 Query: Represents body for the C++ String primitive."""
    if variable == None:
        variable = get_variable("x", depth=depth, prefix=prefix)
    
    nullpos = get_variable("nullpos", depth=depth, prefix=prefix)
    
    total_size = get_variable("total_size", depth=depth, prefix=prefix)
    
    tree = Block(nodes=[
                 ExprStmt(child=Assign(target=Var(name=variable),
                                       value=FuncCall(name=Raw(code=t.cpp),
                                            args=[Raw(code=base_offset),
                                                  Raw(code=total_size)]))),
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

def vl_string_body(t, depth=0, prefix="", base_offset="buf+offset", 
                   variable=None):
    """HDF5 Query: Represents the body for the VL_String primitive."""
    
    if variable == None:
        variable = get_variable("x", depth=depth, prefix=prefix)
    
    tree = Block(nodes=[
                 ExprStmt(child=Assign(target=Var(name=variable), 
                            value=FuncCall(name=Raw(code="VLRead"),
                               args=[Raw(code=base_offset)],
                               targs=[Raw(code=t.cpp), Raw(code=t.db)])))])
    return tree

def uuid_body(t, depth=0, prefix="", base_offset="buf+offset"):
    """HDF5 Query: Represents the body for the boost::uuid primitive."""
    x = get_variable("x", depth=depth, prefix=prefix)
    total_size = get_variable("total_size", depth=depth, prefix=prefix)
    
    tree = Block(nodes=[
                 ExprStmt(child=FuncCall(name=Raw(code="memcpy"), 
                                         args=[Raw(code="&"+x), 
                                               Raw(code=base_offset),
                                               Raw(code=total_size)]))])
    return tree

def vl_body(t, depth=0, prefix="", base_offset="buf+offset"):
    """HDF5 Query: Represents the body for all C++ VL types."""
    x = get_variable("x", depth=depth, prefix=prefix)
    node = Block(nodes=[ExprStmt(child=Assign(target=Var(name=x),
                            value=FuncCall(name=Var(name="VLRead"),
                                           args=[Raw(code=base_offset)],
                                           targs=[Raw(code=t.cpp), 
                                                  Raw(code=t.db)])))])
    return node

def map_body(t, depth=0, prefix="", base_offset="buf+offset"):
    """HDF5 Query: Represents the body for C++ map type."""
    x = get_variable("x", depth=depth, prefix=prefix)
    k = get_variable("k", depth=depth, prefix=prefix)
    fieldlen = get_variable("fieldlen", depth=depth, prefix=prefix)
    
    key = CANON_TO_NODE[t.canon[1]]
    value = CANON_TO_NODE[t.canon[2]]
    
    key_prefix = prefix + template_args[t.canon[0]][0]
    key_name = get_variable("x", depth=depth+1, prefix=key_prefix)
    value_prefix = prefix + template_args[t.canon[0]][1]
    value_name = get_variable("x", depth=depth+1, prefix=value_prefix)
    
    key_size = get_variable("total_size", depth=depth+1, prefix=key_prefix)
    value_size = get_variable("total_size", depth=depth+1, prefix=value_prefix)
    
    item_size = "(" + key_size + "+" + value_size + ")"
    
    key_offset = base_offset + "+" + item_size + "*" + k
    value_offset = key_offset + "+" + key_size
    
    node = Block(nodes=[
          For(adecl=DeclAssign(type=Type(cpp="unsigned int"), 
                               target=Var(name=k), 
                               value=Raw(code="0")),
              cond=BinOp(x=Var(name=k), op="<", y=Var(name=fieldlen)),
              incr=LeftUnaryOp(op="++", name=Var(name=k)),
              body=[
                get_body(key, depth=depth+1, prefix=key_prefix,
                         base_offset=key_offset),
                get_body(value, depth=depth+1, prefix=value_prefix,
                         base_offset=value_offset),
                ExprStmt(child=Assign(target=Raw(code=x+"["+key_name+"]"),
                                      value=Raw(code=value_name)))])])
    return node

def pair_body(t, depth=0, prefix="", base_offset="buf+offset"):
    """HDF5 Query: Represents body for C++ pair type."""
    x = get_variable("x", depth=depth, prefix=prefix)
    
    item1 = CANON_TO_NODE[t.canon[1]]
    item2 = CANON_TO_NODE[t.canon[2]]
    
    item1_prefix = prefix + template_args[t.canon[0]][0]
    item2_prefix = prefix + template_args[t.canon[0]][1]
    item1_name = get_variable("x", depth=depth+1, prefix=item1_prefix)
    item2_name = get_variable("x", depth=depth+1, prefix=item2_prefix)
    
    item1_size = get_variable("total_size", depth=depth+1, prefix=item1_prefix)
    item2_size = get_variable("total_size", depth=depth+1, prefix=item2_prefix)
    
    item2_offset = base_offset + "+" + item1_size
    
    node = Block(nodes=[get_body(item1, depth=depth+1, prefix=item1_prefix,
                                 base_offset=base_offset), 
                        get_body(item2, depth=depth+1, prefix=item2_prefix,
                                 base_offset=item2_offset),
                        ExprStmt(child=Assign(
                            target=Raw(code=x),
                            value=FuncCall(name=Raw(code="std::make_pair"),
                                           args=[Raw(code=item1_name),
                                                 Raw(code=item2_name)])))])
    return node

def vector_primitive_body(t, depth=0, prefix="", base_offset="buf+offset"):
    """HDF5 Query: Represents body of C++ Vector<primitive> types."""
    x = get_variable("x", depth=depth, prefix=prefix)
    k = get_variable("k", depth=depth, prefix=prefix)
    fieldlen = get_variable("fieldlen", depth=depth, prefix=prefix)
    total_size = get_variable("total_size", depth=depth, prefix=prefix)
    
    vector_start = "&" + x + "[0]"
    
    node = Block(nodes=[ExprStmt(child=Assign(target=Var(name=x),
                                              value=FuncCall(
                                                  name=Raw(code=t.cpp),
                                                  args=[Raw(code=fieldlen)]))),
                       ExprStmt(child=FuncCall(name=Var(name="memcpy"),
                                               args=[Raw(code=vector_start),
                                                     Raw(code=base_offset),
                                                     Raw(code=total_size)]))])
    return node

def vector_body(t, depth=0, prefix="", base_offset="buf+offset"):
    """HDF5 Query: Represents body of C++ Vector<non-primitive> types."""
    x = get_variable("x", depth=depth, prefix=prefix)
    k = get_variable("k", depth=depth, prefix=prefix)
    fieldlen = get_variable("fieldlen", depth=depth, prefix=prefix)
    index = x + "[" + k + "]"
    
    child_prefix = get_prefix(prefix, t, 0) 
    child_var = get_variable("x", depth=depth+1, prefix=child_prefix)
    
    child_size = get_variable("total_size", depth=depth+1, prefix=child_prefix)
    child_offset = base_offset + "+" + child_size + "*" + k
    
    node = Block(nodes=[
          ExprStmt(child=Assign(target=Raw(code=x),
                                value=FuncCall(name=Raw(code=t.cpp),
                                               args=[Raw(code=fieldlen)]))),
          For(adecl=DeclAssign(type=Type(cpp="unsigned int"), 
                               target=Var(name=k), 
                               value=Raw(code="0")),
              cond=BinOp(x=Var(name=k), op="<", y=Var(name=fieldlen)),
              incr=LeftUnaryOp(op="++", name=Var(name=k)),
              body=[
                get_body(CANON_TO_NODE[t.canon[1]], depth=depth+1,
                         prefix=child_prefix,
                         base_offset=child_offset),
                ExprStmt(child=Assign(target=Var(name=index), 
                                      value=Raw(code=child_var)))
                ])])
    return node
    
def vec_string_body(t, depth=0, prefix="", base_offset="buf+offset"):
    """HDF5 Query: Represents body of C++ Vector<std::string> types."""
    x = get_variable("x", depth=depth, prefix=prefix)
    k = get_variable("k", depth=depth, prefix=prefix)
    index = x + "[" + k + "]"
    fieldlen = get_variable("fieldlen", depth=depth, prefix=prefix)
    
    string_prefix = get_prefix(prefix, t, 0)
    child_size = get_variable("total_size", depth=depth+1, prefix=string_prefix)
    child_offset = base_offset + "+" + child_size + "*" + k
    
    node = Block(nodes=[
          ExprStmt(child=Assign(target=Raw(code=x),
                                value=FuncCall(name=Raw(code=t.cpp),
                                               args=[Raw(code=fieldlen)]))),
          For(adecl=DeclAssign(type=Type(cpp="unsigned int"), 
                               target=Var(name=k), 
                               value=Raw(code="0")),
              cond=BinOp(x=Var(name=k), op="<", y=Var(name=fieldlen)),
              incr=LeftUnaryOp(op="++", name=Var(name=k)),
              body=[
                string_body(CANON_TO_NODE[t.canon[1]], depth=depth+1,
                              prefix=string_prefix, base_offset=child_offset, 
                              variable=index)
                ])])
    return node

def set_primitive_body(t, depth=0, prefix="", base_offset="buf+offset"):
    """HDF5 Query: Represents body of C++ set<primitive> types."""
    
    x = get_variable("x", depth=depth, prefix=prefix)
    fieldlen = get_variable("fieldlen", depth=depth, prefix=prefix)
    child_prefix = get_prefix(prefix, t, 0)
    xraw = get_variable("xraw", depth=depth+1, prefix=child_prefix)
    
    xraw_type = CANON_TO_NODE[t.canon[1]].cpp + "*"
    
    node = Block(nodes=[ExprStmt(child=DeclAssign(
                           type=Type(cpp=xraw_type),
                           target=Var(name=xraw),
                           value=FuncCall(name=Raw(code="reinterpret_cast"),
                                          targs=[Raw(code=xraw_type)],
                                          args=[Raw(code=base_offset)]))),
                        ExprStmt(child=Assign(
                                        target=Var(name=x),
                                        value=FuncCall(name=Raw(code=t.cpp),
                                                       args=[Raw(code=xraw), 
                                                             Raw(code=xraw
                                                                 +"+"
                                                                 +fieldlen)
                                                                 ])))])
    return node
    
def set_body(t, depth=0, prefix="", base_offset="buf+offset"):
    """HDF5 Query: Represents body of C++ set<non-primitive> types."""
    x = get_variable("x", depth=depth, prefix=prefix)
    k = get_variable("k", depth=depth, prefix=prefix)
    
    fieldlen = get_variable("fieldlen", depth=depth, prefix=prefix)
    
    child_prefix = get_prefix(prefix, t, 0)
    child_var = get_variable("x", depth=depth+1, prefix=child_prefix)
    
    item_size = get_variable("total_size", depth=depth+1, prefix=child_prefix)
    
    child_offset = base_offset + "+" + item_size + "*" + k
    
    node = Block(nodes=[
          For(adecl=DeclAssign(type=Type(cpp="unsigned int"), 
                               target=Var(name=k), 
                               value=Raw(code="0")),
              cond=BinOp(x=Var(name=k), op="<", y=Var(name=fieldlen)),
              incr=LeftUnaryOp(op="++", name=Var(name=k)),
              body=[
                get_body(CANON_TO_NODE[t.canon[1]], depth=depth+1,
                         prefix=child_prefix, base_offset=child_offset),
                ExprStmt(child=FuncCall(name=Raw(code=x+".insert"), 
                                        args=[Raw(code=child_var)]))])])
    return node

def set_string_body(t, depth=0, prefix="", base_offset="buf+offset"):
    """HDF5 Query: Represents body of C++ set<std::string> types."""
    x = get_variable("x", depth=depth, prefix=prefix)
    k = get_variable("k", depth=depth, prefix=prefix) 
    
    fieldlen = get_variable("fieldlen", depth=depth, prefix=prefix)
    
    string_prefix = get_prefix(prefix, t, 0)
    string_size = get_variable("total_size", depth=depth+1, 
                               prefix=string_prefix)
    string_name = get_variable("x", depth=depth+1, prefix=string_prefix)
    
    offset = base_offset + "+" + string_size + "*" + k
    
    node = Block(nodes=[
          For(adecl=DeclAssign(type=Type(cpp="unsigned int"),
                               target=Var(name=k), 
                               value=Raw(code="0")),
              cond=BinOp(x=Var(name=k), op="<", y=Var(name=fieldlen)),
              incr=LeftUnaryOp(op="++", name=Var(name=k)),
              body=[
                string_body(CANON_TO_NODE[t.canon[1]], depth=depth+1,
                            prefix=string_prefix, base_offset=offset),
                ExprStmt(child=FuncCall(name=Raw(code=x+".insert"), 
                                        args=[Raw(code=string_name)]))])])
    return node
    
def list_primitive_body(t, depth=0, prefix="", base_offset="buf+offset"):
    """HDF5 Query: Represents body of C++ list<primitive> types."""
    x = get_variable("x", depth=depth, prefix=prefix)
    fieldlen = get_variable("fieldlen", depth=depth, prefix=prefix)
    child_prefix = get_prefix(prefix, t, 0)
    xraw = get_variable("xraw", depth=depth+1, prefix=child_prefix)
    
    xraw_type = CANON_TO_NODE[t.canon[1]].cpp + "*"
    
    node = Block(nodes=[ExprStmt(child=DeclAssign(
                           type=Type(cpp=xraw_type),
                           target=Var(name=xraw),
                           value=FuncCall(name=Raw(code="reinterpret_cast"),
                                          targs=[Raw(code=xraw_type)],
                                          args=[Raw(code=base_offset)]))),
                        ExprStmt(child=Assign(
                                        target=Var(name=x),
                                        value=FuncCall(name=Raw(code=t.cpp),
                                                       args=[Raw(code=xraw), 
                                                             Raw(code=xraw
                                                                 +"+"
                                                                 +fieldlen)
                                                                 ])))])
    return node

def list_body(t, depth=0, prefix="", base_offset="buf+offset"):
    """HDF5 Query: Represents body of C++ list<non-primitive> types."""
    x = get_variable("x", depth=depth, prefix=prefix)
    k = get_variable("k", depth=depth, prefix=prefix)
    child_prefix = get_prefix(prefix, t, 0)
    child_variable = get_variable("x", depth=depth+1, prefix=child_prefix)
    fieldlen = get_variable("fieldlen", depth=depth, prefix=prefix)
    item_size = get_variable("total_size", depth=depth+1, prefix=child_prefix)
    offset = base_offset + "+" + item_size + "*" + k
    
    node = Block(nodes=[ 
          For(adecl=DeclAssign(type=Type(cpp="unsigned int"), 
                               target=Var(name=k), 
                               value=Raw(code="0")),
              cond=BinOp(x=Var(name=k), op="<", y=Var(name=fieldlen)),
              incr=LeftUnaryOp(op="++", name=Var(name=k)),
              body=[
                get_body(CANON_TO_NODE[t.canon[1]], depth=depth+1,
                            prefix=child_prefix, base_offset=offset),
                ExprStmt(child=FuncCall(name=Raw(code=x+".push_back"),
                                        args=[Raw(code=child_variable)]))])])
    return node

BODIES = {"INT": reinterpret_cast_body,
          "DOUBLE": reinterpret_cast_body,
          "FLOAT": reinterpret_cast_body,
          "BOOL": reinterpret_cast_body,
          "UUID": uuid_body,
          "STRING": string_body,
          "VL_STRING": vl_body,
          "BLOB": vl_body,
          "VECTOR_STRING": vec_string_body,
          "MAP": map_body,
          "PAIR": pair_body,
          "LIST_INT": list_primitive_body,
          "LIST_DOUBLE": list_primitive_body,
          "LIST_FLOAT": list_primitive_body,
          "LIST": list_body,
          "SET_INT": set_primitive_body,
          "SET_DOUBLE": set_primitive_body,
          "SET_FLOAT": set_primitive_body,
          "SET": set_body,
          "VECTOR_INT": vector_primitive_body,
          "VECTOR_DOUBLE": vector_primitive_body,
          "VECTOR_FLOAT": vector_primitive_body,
          "VECTOR": vector_body}

def get_body(t, depth=0, prefix="", base_offset="buf+offset"):
    """HDF5 Query: Get body nodes for a C++ type.
    
    Parameters
    ----------
    t : Type
        C++ type, canonical form.
    depth : int, optional
        Depth relative to initial, depth 0 type.
    prefix : str, optional
        Current prefix, determined by parent type.
    
    Returns
    -------
    Node
        Body nodes required for the type.
    """
    block = []
    block.append(get_decl(t, depth=depth, prefix=prefix))
    if is_primitive(t):
        if depth == 0:
            block.append(BODIES[t.db](t, depth=depth, prefix=prefix, 
                         base_offset=base_offset))
        else:
            block.append(BODIES[t.db](t, depth=depth, prefix=prefix, 
                         base_offset=base_offset))
    elif DB_TO_VL[t.db]:
        block.append(vl_body(t, depth=depth, prefix=prefix,
                             base_offset=base_offset))
    elif t.db in BODIES:
        block.append(BODIES[t.db](t, depth=depth, prefix=prefix,
                                  base_offset=base_offset))
    elif t.canon[0] in BODIES:
        block.append(BODIES[t.canon[0]](t, depth=depth, prefix=prefix,
                                        base_offset=base_offset))
    else:
        raise ValueError("No generation specified for type " + t.db)
    return Block(nodes=block)

# teardown functions

TEARDOWN_STACK = []
VARS = []

def normal_close(t):
    """Represents the generic close to an hdf5 type code block."""
    x = get_variable("x", depth=0, prefix="")
    
    tree = Block(nodes=[
        ExprStmt(child=Assign(target=Var(name="is_row_selected"),
                 value=FuncCall(name=Var(name="CmpConds"), 
                                targs=[Raw(code=t.cpp)],
                            args=[Raw(code="&"+x),
                                  Raw(code="&(field_conds[qr.fields[j]])")]))),
        If(cond=Var(name="is_row_selected"),
           body=[ExprStmt(child=Assign(target=Var(name="row[j]"),
                                       value=Var(name=x)))])])
    
    for i in range(len(TEARDOWN_STACK)):
        var_name = TEARDOWN_STACK.pop()
        teardown = ExprStmt(child=FuncCall(name=Var(name="H5Tclose"),
                                           args=[Raw(code=var_name)]))
        tree.nodes.append(teardown)
    return tree

def get_teardown(t):
    return normal_close(t)

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

def typeid(t):
    node = CANON_TO_NODE[t]
    return FuncCall(name=Raw(code="typeid"), args=[Raw(code=node.cpp)])

def no_vl(t):
    if DB_TO_VL[t.db]:
        return False
    else:
        if is_primitive(t):
            return True
        else:
            ret = True
            for i in t.canon[1:]:
                ret = ret and no_vl(CANON_TO_NODE[i])
            return ret
            
def get_dim_shape(canon, start=0, depth=0):
    tshape = []
    i = 0
    if isinstance(canon, str):
        tshape = start + i
        i += 1
        if depth == 0:
            return i, [tshape]
        else:
            return i, tshape
    else:
        for u in canon:
            j, jshape = get_dim_shape(u, start=start+i, depth=depth+1)
            i += j
            tshape.append(jshape)
        return i, tshape

def flatten(canon):
    if isinstance(canon, str):
        return [canon]
    result = list(canon)
    result[0] = canon
    i = 1
    while i < len(result):
        if isinstance(result[i], str):
            i += 1
        else:
            temp = result[i][1:]
            i += 1
            for j in range(0, len(temp)):
                result.insert(i+j, temp[j])
    return tuple(result)  

def get_variation_cond(t):
    """HDF5 Create: Generate C++ if-statement condition for a given type.
    
    These if-statements are always a string of boolean expressions of the form
    'shape[n]<1' or 'shape[n]>=1', where n is an index into the C++ shape array.
    A shape index less than one (<1) denotes a variable length type, whereas an 
    index greater than one (>=1) denotes fixed length type. These boolean
    expressions are joined by '&&' operators. For instance, a type of 
    VL_MAP_VL_STRING_VL_STRING would receive the condition 
    'shape[0]<1 && shape[1]<1 && shape[2]<1'.
    
    Parameters
    ----------
    t : Type
        C++ type for the boolean condition.
        
    Returns
    -------
    current_bool : BinOp
        Node representing the boolean condition.
    """
    vl_count = 0
    vl_potential_count = 0
    op_list = []
    shape_len, dim_shape = get_dim_shape(t.canon)
    
    flat_canon = flatten(t.canon)
    flat_shape = zip(flat_canon, [x for x in range(shape_len)])
    
    for sub_type, index in flat_shape:
        node = CANON_TO_NODE[sub_type]
        # This type is VL
        if DB_TO_VL[node.db]:
            vl_count += 1
            vl_potential_count += 1
            op_list.append(BinOp(x=Raw(code="shape["+str(index)+"]"), 
                                 op="<", y=Raw(code="1")))
        # Find out if type could be VL
        else:
            orig_type = ORIGIN_DICT[sub_type]
            if is_primitive(CANON_TO_NODE[orig_type]):
                if VARIATION_DICT[orig_type]:
                    vl_potential_count += 1
                    op_list.append(BinOp(x=Raw(code="shape["+str(index)+"]"),
                                         op=">=", y=Raw(code="1"))) 
            else:
                if orig_type[0] in variable_length_types:
                    vl_potential_count += 1
                    op_list.append(BinOp(x=Raw(code="shape["+str(index)+"]"),
                                         op=">=", y=Raw(code="1"))) 
    current_bool = op_list[0]
    for i in range(1,len(op_list)):
        current_bool = BinOp(x=current_bool, op="&&", y=op_list[i])
    
    if vl_count == vl_potential_count:
        current_bool = BinOp(x=Raw(code="shape.empty()"), op="||", 
                             y=current_bool)
    return current_bool

def VL_ADD_BLOCK(t, item_var):
    node = If(cond=BinOp(x=FuncCall(name=Raw(code="vldts_.count"),
                                    args=[Raw(code=t.db)]),
                         op="==",
                         y=Raw(code="0")),
              body=[ExprStmt(child=BinOp(
                                   x=Raw(code="vldts_["+t.db+"]"),
                                   op="=",
                                   y=Raw(code="H5Tvlen_create("+item_var+")"))),
                    ExprStmt(child=FuncCall(
                                        name=Raw(code="opened_types_.insert"),
                                        args=[Raw(code="vldts_["+t.db+"]")]))])
    return node

def print_statement(t, identifier):
    """Generate C++ print statement for debugging generated code."""
    msg_string = t.db + ": got here: " + str(identifier)
    return ExprStmt(child=Raw(code="std::cerr<<\"" + msg_string 
                                   + "\" << std::endl"))

def get_variation_body(t):
    """HDF5 Create: Generate C++ if-statement body for a given type.
    
    Called in coordination with get_variation_cond. For a given C++ type, this 
    function returns the necessary C++ statements to create the HDF5 version 
    of that type.
    
    Parameters
    ----------
    t : Type
        C++ type for which to create an if-statement body.
    
    Returns
    -------
    body : Block
        Node containing necessary C++ statements for HDF5 creation.
    """
    body = Block(nodes=[])
    # This handles types with non-standard bodies that we wish to
    # handle directly.
    if t.db in RAW_TYPES:
        return RAW_TYPES[t.db]
    
    body.nodes.append(ExprStmt(child=Raw(code="dbtypes[i]="+ t.db)))

    item_nodes, opened_types = get_item_type(t)
    body.nodes.append(item_nodes)
    type_var = opened_types[-1] if opened_types != [] else get_variable(
                                                                  "item_type",
                                                                   prefix="",
                                                                   depth=0)
     
    is_vl = True if DB_TO_VL[t.db] else False
    size_expression = Raw(code=get_item_size(t, vl_flag=is_vl))
    body.nodes.append(ExprStmt(child=BinOp(x=Raw(code="dst_sizes[i]"),
                                                 op="=",
                                                 y=size_expression)))
    if DB_TO_VL[t.db]:
        if is_primitive(t):
            default_item_var = type_var
        else:
            if opened_types == []:
                item_prefix = template_args[VL_TO_FL_CONTAINERS[t.canon[0]]][0]
                default_item_var = get_variable("item_type", 
                                                prefix=item_prefix,
                                                depth=1)
            else:
                default_item_var = get_variable("item_type", prefix="", depth=1)
        body.nodes.append(ExprStmt(child=BinOp(x=Raw(code="field_types[i]"),
                                               op="=",
                                               y=Raw(code="sha1_type_"))))
        body.nodes.append(VL_ADD_BLOCK(t, default_item_var))
    else:
        body.nodes.append(ExprStmt(child=BinOp(x=Raw(code="field_types[i]"),
                                               op="=",
                                               y=Raw(code=type_var))))
        for opened in opened_types[:-1]:
            body.nodes.append(ExprStmt(child=FuncCall(
                                          name=Raw(code="opened_types_.insert"),
                                          args=[Raw(code=opened)])))
        if not is_primitive(t):
            body.nodes.append(ExprStmt(child=FuncCall(
                                          name=Raw(code="opened_types_.insert"),
                                          args=[Raw(code="field_types[i]")])))
    return body

HDF5_PRIMITIVES = {"INT": "H5T_NATIVE_INT",
                   "DOUBLE": "H5T_NATIVE_DOUBLE",
                   "FLOAT": "H5T_NATIVE_FLOAT",
                   "BOOL": "H5T_NATIVE_CHAR",
                   "STRING": "CreateFLStrType({size})",
                   "BLOB": "sha1_type_",
                   "UUID": "uuid_type_"}

PRIMITIVE_SIZES = {"INT": "sizeof(int)",
                   "DOUBLE": "sizeof(double)",
                   "FLOAT": "sizeof(float)",
                   "BOOL": "sizeof(char)",
                   "VL_STRING": "CYCLUS_SHA1_SIZE",
                   "BLOB": "CYCLUS_SHA1_SIZE",
                   "UUID": "CYCLUS_UUID_SIZE"}

VL_TO_FL_CONTAINERS = {"VL_VECTOR": "VECTOR",
                       "VL_SET": "SET",
                       "VL_LIST": "LIST",
                       "VL_MAP": "MAP"}

def get_item_type(t, shape_array=None, vl_flag=False, prefix="", depth=0):
    """HDF5 Create: Build specified HDF5 type, recursively if necessary.
    
    HDF5 types are Primitive, Compound, or Array. We handle each of these cases
    here differently. Primitives are immediately returned by querying the 
    HDF5_PRIMITIVES dictionary. Compound types are made up of multiple
    Primitive or Compound types, so each of these child types must be declared
    and created before the parent type can be created. This is accomplished via
    recursion over every child type in the type's canon. It should be noted that
    Compound types depend heavily on the size of those types they contain,
    and this function relies on get_item_size for that information. Finally, 
    Arrays can contain one Primitive or Compound type, and are created by 
    specifying this child type, Array dimensions, and the Array length.
    
    Parameters
    ----------
    t : Type
        Type node representing C++ type
    shape_array : list, optional
        Dimensioned list of current type shape
    prefix : str, optional
        Used to name C++ variables throughout multiple levels of recursion
    depth : int, optional
        Recursive depth counter
    
    Returns
    -------
    node : Block
        Cumulative collection of nodes necessary for specified item type
    opened_stack : list
        Cumulative collection of opened HDF5 types which must eventually be
        closed
    
    """
    # We need to keep a persistant shape array, unless initial call.
    if shape_array == None:
        shape_len, dim_shape = get_dim_shape(t.canon)
    else:
        dim_shape = shape_array
    node = Block(nodes=[])
    opened_stack = []
    type_var = get_variable("item_type", prefix=prefix, depth=depth)
    node.nodes.append(ExprStmt(child=Decl(type=Type(cpp="hid_t"), 
                                          name=Var(name=type_var))))
    # Handle primitives
    if isinstance(t.canon, str):
        if DB_TO_VL[t.db] or (t.canon == "STRING" and vl_flag):
            node.nodes.append(ExprStmt(child=Assign(
                                                 target=Var(name=type_var),
                                                 value=Raw(code="sha1_type_"))))
            return node, opened_stack
        else:   
            primitive_type = Raw(code=HDF5_PRIMITIVES[t.db].format(size="shape["
                                                              +str(dim_shape[0])
                                                              +"]"))
            node.nodes.append(ExprStmt(child=Assign(target=Var(name=type_var),
                                                    value=primitive_type)))
            return node, opened_stack
    # Handle dependent types
    else:
        container_type = t.canon[0]
        canon_shape = list(zip(t.canon, dim_shape))
        is_vl = vl_flag
        if DB_TO_VL[t.db]:
            container_type = VL_TO_FL_CONTAINERS[t.canon[0]]
            is_vl = True
        else:
            if t.canon[0] in variable_length_types:
                shape_var = get_variable("shape0", prefix="", depth=depth+1)
                node.nodes.append(ExprStmt(
                                    child=DeclAssign(
                                         type=Type(cpp="hsize_t"),
                                         target=Var(name=shape_var),
                                         value=Raw(code="shape["
                                                   +str(dim_shape[0])+"]"))))
        item_var = ""
        if len(canon_shape[1:]) == 1:
            # Not a compound type.
            item_canon, item_shape = canon_shape[1]
            # Get nodes initializing our child type
            child_array = (item_shape if isinstance(item_shape, list) 
                                      else [item_shape])
            new_prefix = template_args[container_type][0]
            child_node, child_opened = get_item_type(CANON_TO_NODE[item_canon],
                                            shape_array=child_array,
                                            vl_flag=is_vl,
                                            prefix=new_prefix,
                                            depth=depth+1)
            node.nodes.append(child_node)
            opened_stack.extend(child_opened)
            child_var = get_variable("item_type", prefix=new_prefix, 
                                     depth=depth+1)
                                                 
            item_var = child_var
        else:
            # This is a compound type.
            child_dict = OrderedDict()
            # 1. Get all child item type nodes, recursively.
            for i in range(1, len(canon_shape)):
                item_canon, item_shape = canon_shape[i]
                item_node = CANON_TO_NODE[item_canon]
                pre_opened_len = len(opened_stack)
                child_array = (item_shape if isinstance(item_shape, list)
                                          else [item_shape])
                new_prefix = template_args[container_type][i-1]
                child_node, child_opened = get_item_type(item_node,
                                                        shape_array=child_array,
                                                        vl_flag=is_vl,
                                                        prefix=new_prefix,
                                                        depth=depth+1)
                node.nodes.append(child_node)
                opened_stack.extend(child_opened)
               
                # if the previous opened stack and current stack are the same,
                # we know that the child is a primitive, and we can generate
                # its variable accordingly.
                if  len(opened_stack) == pre_opened_len:
                    child_item_var = get_variable("item_type", 
                                                  prefix=new_prefix,
                                                  depth=depth+1)
                # However, if the current opened stack is longer, the first new
                # variable there will be our child variable.
                else:
                    child_item_var = opened_stack[pre_opened_len]
                # 2. Get item sizes.
                child_dict[child_item_var] = get_item_size(item_node, 
                                                           child_array,
                                                           vl_flag=is_vl, 
                                                           depth=depth+1)
            # 3. Create compound type using total item size.
            compound = hdf5_create_compound(list(child_dict.values()))
            
            item_var = get_variable("item_type", prefix="", depth=depth+1)
            node.nodes.append(ExprStmt(child=Decl(type=Type(cpp="hid_t"),
                                                  name=Raw(code=item_var))))
            
            node.nodes.append(ExprStmt(child=Assign(target=Raw(code=item_var),
                                                    value=compound)))
            
            opened_stack.append(item_var)
            # 4. Insert individual children into the compound type.            
            node.nodes.append(hdf5_insert(container_type, item_var, child_dict))
            
        if container_type in variable_length_types and not DB_TO_VL[t.db]:
            array_node = ExprStmt(child=Assign(target=Var(name=type_var),
                                               value=hdf5_array_create(
                                                           item_var,
                                                           rank=1,
                                                           dims="&"+shape_var)))
            
            opened_stack.append(type_var)
            node.nodes.append(array_node)
    return node, opened_stack

def get_item_size(t, shape_array=None, vl_flag=False, depth=0):
    """Resolves item size recursively.
    
    We can dig down into a type until we reach eventual primitives, and then
    multiply the known sizes of those primitives by the lengths of their
    containers. Container length is defined in the C++ shape array.
    
    Parameters
    ----------
    t : Type
        The type whose size is in question
    shape_array : list, optional
        Dimensioned list of shape array indicies, same shape as t.canon
    depth : int, optional
        Recursive depth counter
    
    Returns
    -------
    size : str
        String of C++ expression representing t's size.
    """
    if shape_array == None:
        shape_array = get_dim_shape(t.canon)[1]
    if is_primitive(t):
        if t.db in PRIMITIVE_SIZES.keys():
            return PRIMITIVE_SIZES[t.db]
        else:
            if not vl_flag:
                return "shape[" + str(shape_array[0]) + "]"
            else:
                return "CYCLUS_SHA1_SIZE"
    else:
        size = "("
        if DB_TO_VL[t.db]:
            size += "CYCLUS_SHA1_SIZE"
        else:
            size += "("
            if len(t.canon[1:]) > 1:
                children = []
                for child_index in range(1, len(t.canon)):
                    child_array = shape_array[child_index]
                    if not isinstance(child_array, list):
                        child_array = [child_array]
                    children.append(get_item_size(
                                           CANON_TO_NODE[t.canon[child_index]],
                                           child_array,
                                           vl_flag=vl_flag,
                                           depth=depth+1))
                size += "+".join(children)
            else:
                child_array = shape_array[1]
                if not isinstance(child_array, list):
                    child_array = [child_array]
                size += get_item_size(CANON_TO_NODE[t.canon[1]], child_array,
                                      vl_flag=vl_flag, depth=depth+1)
            size += ")"
            if t.canon[0] in variable_length_types:
                size += "*" + "shape[" + str(shape_array[0]) + "]"
        size += ")"
        return size    

def hdf5_array_create(item_variable, rank=1, dims="&shape0"):
    """Node representation of the C++ H5Tarray_create2 method.
    
    Parameters
    ----------
    item_variable : str
        Variable name of HDF5 array item.
    rank : int, optional
        Number of HDF5 array dimensions.
    dims : str, optional
        Variable (by reference) of shape array belonging to HDF5 array

    Returns
    -------
    node : FuncCall
        Node of H5Tarray_create2 function call.
    """     
    node = FuncCall(name=Var(name="H5Tarray_create2"), 
                    args=[Raw(code=item_variable), Raw(code=str(rank)),
                          Raw(code=dims)])
    return node

def hdf5_create_compound(sizes):
    """Node representation of the C++ HDF5 compound type creation function.
    
    Parameters
    ----------
    sizes : list
        List of type sizes, all must be str type.
    
    Returns
    -------
    node : FuncCall
        H5Tcreate function call node.
    """
    node = FuncCall(name=Var(name="H5Tcreate"), args=[Raw(code="H5T_COMPOUND"),
                                            Raw(code="+".join(sizes))])
    return node
    
def hdf5_insert(container_type, compound_var, types_sizes_dict):
    """Node representation of the C++ H5Tinsert function.
    
    This function is used to identify partitions within an already established
    HDF5 Compound type. That is, we specify which inner types are located at
    what memory location within the Compound type.
    
    Parameters
    ----------
    container_type : str
        Should be a key in the template_args dict
    compound_var : str
        C++ variable to which the function should refer
    types_sizes_dict : dict
        Dictionary of C++ type variables mapped to their size in memory
         
    Returns
    -------
    node : Block
        Cumulative nodes for H5Tinsert function
    """
    node = Block(nodes=[])
    buf = str(0)
    keys = list(types_sizes_dict.keys())
    for i in range(len(types_sizes_dict)):
        type_var = keys[i]
        type_size = types_sizes_dict[type_var]
        descriptor = "\"" + template_args[container_type][i] + "\""
        func = FuncCall(name=Var(name="H5Tinsert"), args=[])
        func.args.append(Raw(code=compound_var))
        func.args.append(Raw(code=descriptor))
        func.args.append(Raw(code=buf))
        buf += "+" + type_size
        func.args.append(Raw(code=type_var))
        node.nodes.append(ExprStmt(child=func))
    return node

def main_query():
    """HDF5 Query: Generate Query case statement code."""
    CPPGEN = CppGen()
    output = ""
    for type in CANON_SET:
        type_node = CANON_TO_NODE[type]
        setup = get_setup(type_node)
        body = get_body(type_node)
        teardown = get_teardown(type_node)
        read_x = Block(nodes=[setup, body, teardown])
        output += CPPGEN.visit(case_template(type_node, read_x))
    output = indent(output, INDENT * 5)
    return output

io_error = Raw(code=("throw IOError(\"the type for column \'\"+"
                     "std::string(field_names[i])+\"\' is not yet supported "
                     "in HDF5.\");"))

raw_string = Raw(code=("dbtypes[i]=STRING;\n"
                      "field_types[i]=H5Tcopy(H5T_C_S1);\n"
                      "H5Tset_size(field_types[i], shape[0]);\n"
                      "H5Tset_strpad(field_types[i], H5T_STR_NULLPAD);\n"
                      "opened_types_.insert(field_types[i]);\n"
                      "dst_sizes[i]=sizeof(char)*shape[0];\n"))

raw_blob = Raw(code=("dbtypes[i]=BLOB;\n"
                     "field_types[i]=sha1_type_;\n"
                     "dst_sizes[i]=CYCLUS_SHA1_SIZE;\n"))

RAW_TYPES = {"STRING": raw_string,
             "BLOB": raw_blob}

DEBUG_TYPES = ["VECTOR_STRING"]

def main_create():
    """HDF5 Create: Generate CreateTable if-statements."""
    CPPGEN = CppGen()
    output = ""
        
    outer_if_bodies = {n: Block(nodes=[]) for n in VARIATION_DICT.keys()}
    
    for n in VARIATION_DICT.keys():
        variations = VARIATION_DICT[n][:]
        key_node = CANON_TO_NODE[n]
        try:
            initial_type = variations.pop()
            sub_if = If(cond=get_variation_cond(initial_type),
                        body=[get_variation_body(initial_type)],
                        elifs=[(get_variation_cond(v), 
                               [get_variation_body(v)]) 
                              for v in variations],
                        el=Block(nodes=[get_variation_body(key_node)]))
            outer_if_bodies[n].nodes.append(sub_if)
        except IndexError:
            lone_node = get_variation_body(key_node)
            outer_if_bodies[n].nodes.append(lone_node)

    shape_line = ExprStmt(child=Raw(code="shape=shapes[i]"))
    
    initial_node, initial_body = outer_if_bodies.popitem()
    if_statement = If(cond=BinOp(x=Var(name="valtype"), op="==", 
                                 y=typeid(initial_node)),
                      body=[shape_line, initial_body],
                      elifs=[(BinOp(x=Var(name="valtype"), op="==",
                                    y=typeid(t)), 
                              [shape_line, outer_if_bodies[t]])
                             for t in outer_if_bodies.keys()],
                      el=io_error)
    output += CPPGEN.visit(if_statement)
    output = indent(output, INDENT)
    return output

def camel_case(db):
    parts = db.split("_")
    for i in range(len(parts)):
        parts[i] = parts[i].capitalize()
    return "".join(parts)
    
def string(s):
    return "\"" + s + "\""

def main_vl_dataset():
    CPPGEN = CppGen()
    output = ""
    origin_types = list(VARIATION_DICT.keys())
    for origin in origin_types:
        vals = [v.canon for v in VARIATION_DICT[origin] if DB_TO_VL[v.db]]
        origin_node = CANON_TO_NODE[origin]
        case_body = Block()
        if vals == []:
            if DB_TO_VL[origin_node.db]:
                vals.append(origin)
            else:   
                continue
        for v in vals:
            node = Assign(
                       target=Var(name="name"),
                       value=Raw(code=string(camel_case(origin_node.db))))
            case_body = ExprStmt(child=node)
            output += CPPGEN.visit(case_template(CANON_TO_NODE[v], case_body))
    
    output = indent(output, INDENT*2)
    return output

def main_fill_buf():
    CPPGEN = CppGen()
    output = ""
    for i in CANON_SET:
        node = CANON_TO_NODE[i]
        write_to_buf = FuncCall(name=Var(name="WriteToBuf"), 
                                targs=[Raw(code=node.db)],
                                args=[Raw(code="buf+offset"), Raw(code="shapes[col]"),
                                      Raw(code="a"), Raw(code="sizes[col]")])
        case_body = ExprStmt(child=write_to_buf)
        output += CPPGEN.visit(case_template(node, case_body))
    output = indent(output, INDENT*4)
    return output
                    
vl_write_vl_string = """hasher_.Clear();
hasher_.Update({var});
Digest {key} = hasher_.digest();
hid_t {keysds} = VLDataset({t.db}, true);
hid_t {valsds} = VLDataset({t.db}, false);
if (vlkeys_[{t.db}].count(key) != 1) {{
  AppendVLKey({keysds}, {t.db}, {key});
  InsertVLVal({valsds}, {t.db}, {key}, {var});
}}\n"""

vl_write_blob = """hasher_.Clear();
hasher_.Update({var});
Digest {key} = hasher_.digest();
hid_t {keysds} = VLDataset({t.db}, true);
hid_t {valsds} = VLDataset({t.db}, false);
if (vlkeys_[{t.db}].count(key) != 1) {{
  AppendVLKey({keysds}, {t.db}, {key});
  InsertVLVal({valsds}, {t.db}, {key}, {var}.str());
}}\n"""

VL_SPECIAL_TYPES = {"VL_STRING": vl_write_vl_string,
                    "BLOB": vl_write_blob}

def vl_write(t, variable, depth=0, prefix=""):
    buf_variable = get_variable("buf", depth=depth, prefix=prefix)
    key_variable = get_variable("key", depth=depth, prefix=prefix)
    keysds_variable = get_variable("keysds", depth=depth, prefix=prefix)
    valsds_variable = get_variable("valsds", depth=depth, prefix=prefix)
    node_str = ""
    if t.db in VL_SPECIAL_TYPES:
        node_str = VL_SPECIAL_TYPES[t.db]
    else:
        node_str = """hasher_.Clear();
hasher_.Update({var});
Digest {key} = hasher_.digest();
hid_t {keysds} = VLDataset({t.db}, true);
hid_t {valsds} = VLDataset({t.db}, false);
if (vlkeys_[{t.db}].count({key}) != 1) {{
  hvl_t {buf} = VLValToBuf({var});
  AppendVLKey({keysds}, {t.db}, {key});
  InsertVLVal({valsds}, {t.db}, {key}, {buf});
}}\n"""
    node = Raw(code=node_str.format(var=variable, key=key_variable,
                                    keysds=keysds_variable, t=t,  
                                    valsds=valsds_variable,
                                    buf=buf_variable))
    return node

def memcpy(dest, src, size):
    return FuncCall(name=Var(name="memcpy"), args=[Raw(code=dest),
                                                   Raw(code=src),
                                                   Raw(code=size)])

def memset(dest, src, size):
    return FuncCall(name=Var(name="memset"), args=[Raw(code=dest),
                                                   Raw(code=src),
                                                   Raw(code=size)])

def a_cast(t):
    if is_primitive(t) and t.db != "STRING":
        return "a->castsmallvoid()" 
    else:
        return "a->cast<" + t.cpp + ">()"

def get_write_setup(t, shape_array, depth=0, prefix=""):
    setup = Block(nodes=[])
    variable = get_variable("item_size", depth=depth, prefix=prefix)
    setup.nodes.append(ExprStmt(child=DeclAssign(type=Type(cpp="size_t"),
                                                 target=Var(name=variable),
                                                 value=Raw(code=get_item_size(t, shape_array)))))
    if t.db == "STRING":
        valuelen = get_variable("valuelen", depth=depth, prefix=prefix)
        setup.nodes.append(ExprStmt(child=Decl(type=Type(cpp="size_t"),
                                               name=Var(name=(valuelen)))))
    if is_primitive(t):
        return setup
    else:
        #Setup prefixes and container-level length variable.
        container = t.canon[0]
        if DB_TO_VL[t.db]:
            container = VL_TO_FL_CONTAINERS[container]
        elif t.canon[0] in variable_length_types:
            length = get_variable("length", depth=depth, prefix=prefix)
            setup.nodes.append(ExprStmt(child=DeclAssign(
                                                    type=Type(cpp="size_t"),
                                                    target=Var(name=length),
                                                    value=Raw(code="shape["+str(shape_array[0])+"]"))))
        prefixes = template_args[container]
        
        #Add sizes of any children.
        for c, s, p in zip(t.canon[1:], shape_array[1:], prefixes):
            node = CANON_TO_NODE[c]
            if isinstance(s, int):
                s = [s]
            setup.nodes.append(get_write_setup(node, s, depth=depth+1, 
                                               prefix=prefix+p))
        total_item_size = get_variable("total_item_size", depth=depth, 
                                       prefix=prefix)
        
        #Put together total_item_size variable.
        children = []
        for i in range(len(t.canon[1:])):
            children.append(get_variable("item_size", depth=depth+1, 
                                         prefix=prefix+prefixes[i]))
        if len(children) == 1:
            setup.nodes.append(ExprStmt(child=DeclAssign(
                                               type=Type(cpp="size_t"),
                                               target=Var(name=total_item_size),
                                               value=Raw(code=children[0]))))
        else:
            setup.nodes.append(ExprStmt(child=DeclAssign(
                                               type=Type(cpp="size_t"),
                                               target=Var(name=total_item_size),
                                               value=Raw(
                                                     code="+".join(children)))))        
    return setup

def write_body_string(t, depth=0, prefix="", variable=None, offset="buf", pointer=False):
    if variable is None:
        variable = get_variable("val", depth=depth, prefix=prefix)
    node = Block(nodes=[])
    size = "->size()" if pointer else ".size()"
    c_str = "->c_str()" if pointer else ".c_str()"
    valuelen = get_variable("valuelen", depth=depth, prefix=prefix)
    item_size = get_variable("item_size", depth=depth, prefix=prefix)
    node.nodes.append(ExprStmt(child=Assign(target=Var(name=valuelen),
                                            value=FuncCall(
                                                 name=Raw(code="std::min"),
                                                 args=[
                                                   Raw(code="static_cast<int>("
                                                            +variable+size+")"),
                                                   Raw(code=item_size)]))))
    node.nodes.append(ExprStmt(child=memcpy(offset, variable+c_str, valuelen)))
    node.nodes.append(ExprStmt(child=memset(offset+"+"+valuelen, "0", 
                                            item_size+"-"+valuelen)))
    return node
    
def write_body_primitive(t, depth=0, prefix="", variable=None, offset="buf", pointer=False):
    if variable is None:
        variable = get_variable("val", depth=depth, prefix=prefix)
    if depth != 0:
        variable = "&(" + variable + ")"
    size = get_variable("item_size", depth=depth, prefix=prefix)
    node = ExprStmt(child=memcpy(offset, variable, size))
    return node

WRITE_BODY_PRIMITIVES = {"STRING": write_body_string}

def get_write_body(t, shape_array, depth=0, prefix="", variable="a", offset="buf", pointer=False):
    all_vl = False
    result = Block(nodes=[])
    #Determine if type is entirely variable length
    if is_primitive(t):
        if DB_TO_VL[t.db]:
            all_vl = True
    else:
        flat = flatten(t.canon)
        for i in range(len(flat)):
            canon = flat[i]
            node = CANON_TO_NODE[canon]
            if DB_TO_VL[node.db]:
                all_vl = True
                continue
            else:
                if i == 0:
                    break
                elif node.canon in NOT_VL:
                    continue
                else:
                    all_vl = False
                    break
    #Declare and assign the 'val' variable
    if depth == 0:
        result.nodes.append(get_write_setup(t, shape_array))
        variable = get_variable("val", depth=depth, prefix=prefix)
        result.nodes.append(ExprStmt(child=DeclAssign(
                                                    type=t, 
                                                    target=Var(name=variable),
                                                    value=Raw(code=a_cast(t)))))
    #If entirely variable length, we can simply use the VLWrite definition
    if all_vl:
        result.nodes.append(vl_write(t, variable, depth=depth, prefix=prefix))
        key = get_variable("key", depth=depth, prefix=prefix)
        result.nodes.append(ExprStmt(child=memcpy(offset, key + ".val", 
                                                  "CYCLUS_SHA1_SIZE")))
        return result
    #Handle primitive bodies 
    if is_primitive(t):
        if t.db in WRITE_BODY_PRIMITIVES:
            result.nodes.append(WRITE_BODY_PRIMITIVES[t.db](t, depth=depth, 
                                                            prefix=prefix, 
                                                            variable=variable, 
                                                            offset=offset,
                                                            pointer=pointer))
        else:
            result.nodes.append(write_body_primitive(t, depth=depth, 
                                                     prefix=prefix,
                                                     variable=variable,
                                                     offset=offset,
                                                     pointer=pointer))
        return result
    #Handle potentially variable length bodies
    else:
        #Declare count and iterator variables for the loop.
        count = get_variable("count", depth=depth, prefix=prefix)
        iterator = get_variable("it", depth=depth, prefix=prefix)
        #Recursively gather child bodies
        child_bodies = []
        container = t.canon[0]
        #Handle variable length container
        if DB_TO_VL[t.db]:
            container = VL_TO_FL_CONTAINERS[container]
            prefixes = template_args[container]
            fixed_val = get_variable("fixed_val", depth=depth, prefix=prefix)
            result.nodes.append(ExprStmt(child=Decl(type=Type(cpp=t.cpp),
                                                    name=Raw(code=fixed_val))))
            result.nodes.append(ExprStmt(child=DeclAssign(
                                                      type=Type(cpp="unsigned int"),
                                                      target=Var(name=count),
                                                      value=Raw(code="0"))))
            result.nodes.append(ExprStmt(child=DeclAssign(
                                                  type=Type(cpp=t.cpp+"::iterator"),
                                                  target=Raw(code=iterator),
                                                  value=Raw(code=variable 
                                                                 +".begin()"))))
            
            result.nodes.append(For(cond=BinOp(x=Var(name=iterator), op="==", 
                                               y=Var(name=variable+".end()")),
                                    incr=Raw(code="++" + iterator),
                                    body=[]))
        #Handle fixed length containers
        else:
            prefixes = template_args[container]
            if len(t.canon[1:]) == 1:
                pointer_var = "*" + iterator
                child_node = CANON_TO_NODE[t.canon[1]]
                child_size = get_variable("item_size", depth=depth+1, 
                                          prefix=prefix+prefixes[0])
                child_bodies.append(get_write_body(child_node, shape_array[1], 
                                                   depth=depth+1, 
                                                   prefix=prefix+prefixes[0],
                                                   variable=pointer_var,
                                                   offset=offset+"+"+child_size+"*"+count))
            else:
                partial_size = "0"
                if container in variable_length_types:
                    labels = ['->first', '->second']
                else:
                    labels = ['.first', '.second']
                for c, s, p, l in zip(t.canon[1:], shape_array[1:], prefixes, labels):
                    child_node = CANON_TO_NODE[c]
                    item_label = iterator+l
                    child_size = get_variable("item_size", depth=depth+1, 
                                              prefix=prefix+p)
                    child_bodies.append(get_write_body(child_node, s, depth=depth+1,
                                                       prefix=prefix+p, 
                                                       variable=item_label, 
                                                       offset=offset+"+("+child_size
                                                              +"*"+count+")+"
                                                              +partial_size))
                    partial_size += "+" + child_size
            if container in variable_length_types:
                #For loop uses child bodies
                result.nodes.append(ExprStmt(child=DeclAssign(
                                                      type=Type(cpp="unsigned int"),
                                                      target=Var(name=count),
                                                      value=Raw(code="0"))))
                result.nodes.append(ExprStmt(child=DeclAssign(
                                                  type=Type(cpp=t.cpp+"::iterator"),
                                                  target=Raw(code=iterator),
                                                  value=Raw(code=variable 
                                                                 +".begin()"))))
                result.nodes.append(For(cond=BinOp(x=Var(name=iterator), op="==", 
                                                   y=Var(name=variable+".end()")),
                                        incr=Raw(code="++" + iterator),
                                        body=[*child_bodies, 
                                              ExprStmt(child=LeftUnaryOp(
                                                                 op="++", 
                                                                 name=Var(
                                                                    name=count)))]))
            else:
                result.nodes.extend(child_bodies)
        #Add memset statement outside of loop
        item_size = get_variable("total_item_size", depth=depth, prefix=prefix)
        container_length = get_variable("length", depth=depth, prefix=prefix)
        dest = offset + "+" + item_size + "*" + count
        length = item_size + "*" + "(" + container_length + "-" + count + ")"
        if len(child_bodies) == 1 and t.canon[1] != "STRING" and t.canon[1] != "VL_STRING" and is_primitive(CANON_TO_NODE[t.canon[1]]) and depth == 0:
            length = "column - std::min(column, " + container_length + "*" + item_size + ")"
        result.nodes.append(ExprStmt(child=memset(dest, str(0), length)))
        return result
        
def main_write():
    CPPGEN = CppGen()
    output = ""
    for i in CANON_SET:
        block = Block(nodes=[])
        t = CANON_TO_NODE[i]
        node = FuncDef(type=Raw(code="void"), 
                       name=Var(name="Hdf5Back::WriteToBuf"),
                       targs=[Raw(code=t.db)], 
                       args=[Decl(type=Type(cpp="char*"), name=Var(name="buf")),
                             Decl(type=Type(cpp="std::vector<int>&"), 
                                  name=Var(name="shape")),
                             Decl(type=Type(cpp="const boost::spirit::hold_any*"),
                                  name=Var(name="a")),
                             Decl(type=Type(cpp="size_t"),
                                  name=Var(name="column"))],
                       body=[get_write_body(t, get_dim_shape(t.canon)[1])], 
                       tspecial=True)
        block.nodes.append(node)       
        output += CPPGEN.visit(block)
    return output

NOT_VL = []
VARIATION_DICT = {}
ORIGIN_DICT = {}

MAIN_DISPATCH = {"QUERY": main_query,
                 "CREATE": main_create,
                 "VL_DATASET": main_vl_dataset,
                 "FILL_BUF": main_fill_buf,
                 "WRITE": main_write}

def main():
    global NOT_VL
    global VARIATION_DICT
    global ORIGIN_DICT
    try:
        gen_instruction = sys.argv[1]
    except:
        raise ValueError("No generation instruction provided")    
    
    # Setup for global util dictionaries
    
    fixed_length_types = set(t for t in CANON_SET if no_vl(CANON_TO_NODE[t]))
    
    for n in fixed_length_types:
        key = CANON_TO_NODE[n]
        vals = []
        for x in CANON_SET:
            val_node = CANON_TO_NODE[x]
            if val_node.cpp == key.cpp and val_node.db != key.db: 
                vals.append(val_node)
        VARIATION_DICT[n] = vals
    
    VARIATION_DICT['BLOB'] = []
    VARIATION_DICT['STRING'] = [CANON_TO_NODE['VL_STRING']]
        
    for i in VARIATION_DICT.keys():
        ORIGIN_DICT[i] = i
        if VARIATION_DICT[i] != []:
            for j in VARIATION_DICT[i]:
                ORIGIN_DICT[j.canon] = i
                
    NOT_VL = []
    for i in VARIATION_DICT.keys():
        node = CANON_TO_NODE[i]
        if DB_TO_VL[node.db]:
            continue
        if not is_primitive(node):
            if i[0] not in variable_length_types:
                NOT_VL.append(i)
                for j in VARIATION_DICT[i]:
                    NOT_VL.append(j.canon)
        if not VARIATION_DICT[i]:
            NOT_VL.append(i)
    NOT_VL = set(NOT_VL)
        
    # Dispatch to requested generation function
    function = MAIN_DISPATCH[gen_instruction]
    print(function())
    
if __name__ == '__main__':
    main()

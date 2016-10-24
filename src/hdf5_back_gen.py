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
INDENT = '    '

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
        body = read_x
    body += [ExprStmt(child=Var(name="break"))]
    node = Case(cond=Var(name=t.db), body=body)
    return node

def primitive_setup(t, depth=0, prefix=""):
    """Represents necessary setup steps for C++ primitives."""
    node = Nothing()
    return node

def string_setup(depth=0, prefix=""): 
    """Represents necessary setup steps for C++ String."""
    nullpos = "nullpos" + str(depth) + prefix
    
    node = Block(nodes=[
        ExprStmt(child=Decl(type=Type(cpp="size_t"), name=Var(name=nullpos)))])
    return node

def vl_string_setup(depth=0, prefix=""):
    """Represents necessary setup steps for C++ VL_String."""
    
    node = Block(nodes=[Nothing()])
    return node

template_args = {"MAP": ("KEY", "VALUE"),
                 "VECTOR": ("ELEM",),
                 "SET": ("ELEM",),
                 "LIST": ("ELEM",),
                 "PAIR": ("ITEM1", "ITEM2")}

variable_length_types = ["MAP", "LIST", "SET", "VECTOR"]

def get_setup(t, depth=0, prefix="", HDF5_type="tb_type", child_index='j'):
    """Get nodes representing C++ setup. 
    
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
            item_type_var = get_variable("item_type", depth=depth, prefix=prefix)
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
    """Get node representing C++ type declaration. 
    
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
    """Represents a body using the reinterpret_cast method.
    
    This includes int, double, float, etc.
    """
    x = get_variable("x", depth=depth, prefix=prefix)
    tree = Block(nodes=[
                 ExprStmt(child=Assign(target=Var(name=x), 
                            value=FuncCall(name=Raw(code="*reinterpret_cast"),
                            targs=[Raw(code=t.cpp+"*")], 
                            args=[Raw(code=base_offset)])))])
    return tree

def string_body(t, depth=0, prefix="", base_offset="buf+offset", variable=None):
    """Represents body for the C++ String primitive."""
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
    """Represents the body for the VL_String primitive."""
    
    if variable == None:
        variable = get_variable("x", depth=depth, prefix=prefix)
    
    tree = Block(nodes=[
                 ExprStmt(child=Assign(target=Var(name=variable), 
                            value=FuncCall(name=Raw(code="VLRead"),
                               args=[Raw(code=base_offset)],
                               targs=[Raw(code=t.cpp), Raw(code=t.db)])))])
    return tree

def uuid_body(t, depth=0, prefix="", base_offset="buf+offset"):
    """Represents the body for the boost::uuid primitive."""
    x = get_variable("x", depth=depth, prefix=prefix)
    total_size = get_variable("total_size", depth=depth, prefix=prefix)
    
    tree = Block(nodes=[
                 ExprStmt(child=FuncCall(name=Raw(code="memcpy"), 
                                         args=[Raw(code="&"+x), 
                                               Raw(code=base_offset),
                                               Raw(code=total_size)]))])
    return tree

def vl_body(t, depth=0, prefix="", base_offset="buf+offset"):
    """Represents the body for all C++ VL types."""
    x = get_variable("x", depth=depth, prefix=prefix)
    node = Block(nodes=[ExprStmt(child=Assign(target=Var(name=x),
                            value=FuncCall(name=Var(name="VLRead"),
                                           args=[Raw(code=base_offset)],
                                           targs=[Raw(code=t.cpp), 
                                                  Raw(code=t.db)])))])
    return node

def map_body(t, depth=0, prefix="", base_offset="buf+offset"):
    """Represents the body for C++ map type."""
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
    """Represents body for C++ pair type."""
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
    """Represents body of C++ Vector<primitive> types."""
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
    """Represents body of C++ Vector<non-primitive> types."""
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
    """Represents body of C++ Vector<std::string> types."""
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
    """Represents body of C++ set<primitive> types."""
    
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
    """Represents body of C++ set<non-primitive> types."""
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
    """Represents body of C++ set<std::string> types."""
    x = get_variable("x", depth=depth, prefix=prefix)
    k = get_variable("k", depth=depth, prefix=prefix) 
    
    fieldlen = get_variable("fieldlen", depth=depth, prefix=prefix)
    
    string_prefix = get_prefix(prefix, t, 0)
    string_size = get_variable("total_size", depth=depth+1, prefix=string_prefix)
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
    """Represents body of C++ list<primitive> types."""
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
    """Represents body of C++ list<non-primitive> types."""
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
    """Get body nodes for a C++ type.
    
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

#teardown functions

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

NOT_VL = []

VARIATION_DICT = {}
ORIGIN_DICT = {}

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
    if isinstance(canon, str):
        print(result)
    return tuple(result)  

def get_vl_cond(t):
    vl_count = 0
    vl_potential_count = 0
    op_list = []
    shape_len, dim_shape = get_dim_shape(t.canon)
    
    flat_canon = flatten(t.canon)
    flat_shape = zip(flat_canon, [x for x in range(shape_len)])
    
    for sub_type, index in flat_shape:
        node = CANON_TO_NODE[sub_type]
        #This type is VL
        if DB_TO_VL[node.db]:
            vl_count += 1
            vl_potential_count += 1
            op_list.append(BinOp(x=Raw(code="shape["+str(index)+"]"), 
                                 op="<", y=Raw(code="1")))
        #Find out if type could be VL
        else:
            orig_type = ORIGIN_DICT[sub_type]
            if VARIATION_DICT[orig_type]:
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
    
def get_vl_body(t, current_shape_index=0):
    body = Block(nodes=[])
    body.nodes.append(ExprStmt(child=Raw(code="shape=shapes[i]")))
    body.nodes.append(ExprStmt(child=Raw(code="dbtypes[i]="+ t.db)))
    #do this for every variation.
    body.nodes.append(get_item_type(t))
    type_var = get_variable("item_type", prefix="", depth=0)
    size_expression = Raw(code=get_item_size(t))
    body.nodes.append(ExprStmt(child=BinOp(x=Raw(code="dst_sizes[i]"),
                                               op="=",
                                               y=size_expression)))
    if DB_TO_VL[t.db]:
        body.nodes.append(ExprStmt(child=BinOp(x=Raw(code="field_types[i]"),
                                           op="=",
                                           y=Raw(code="sha1_type_"))))
        body.nodes.append(VL_ADD_BLOCK(t, type_var))
    else:
        body.nodes.append(ExprStmt(child=BinOp(x=Raw(code="field_types[i]"),
                                           op="=",
                                           y=Raw(code=type_var))))
        body.nodes.append(ExprStmt(child=FuncCall(
                                        name=Raw(code="opened_types_.insert"),
                                        args=[Raw(code="field_types[i]")])))
    return body

HDF5_TYPES = {"INT": "H5T_NATIVE_INT",
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

def get_item_type(t, shape_array=None, prefix="", depth=0):
    #We need to keep a persistant shape array, unless initial call.
    if shape_array == None:
        shape_len, dim_shape = get_dim_shape(t.canon)
    else:
        dim_shape = shape_array
    node = Block(nodes=[])
    type_var = get_variable("item_type", prefix=prefix, depth=depth)
    node.nodes.append(ExprStmt(child=Decl(type=Type(cpp="hid_t"), 
                                          name=Var(name=type_var))))
    #handle primitive
    if isinstance(t.canon, str):
        if DB_TO_VL[t.db]:
            node.nodes.append(ExprStmt(child=Assign(
                                                 target=Var(name=type_var),
                                                 value=Raw(code="sha1_type_"))))
            return node
        else:
            primitive_type = Raw(code=HDF5_TYPES[t.db].format(size="shape["
                                                              +str(dim_shape[0])
                                                              +"]"))
            node.nodes.append(ExprStmt(child=Assign(target=Var(name=type_var),
                                                    value=primitive_type)))
            return node
    #dependent types
    else:
        container_type = t.canon[0]
        if DB_TO_VL[t.db]:
            container_type = VL_TO_FL_CONTAINERS[t.canon[0]]
        canon_shape = list(zip(t.canon, dim_shape))
        shape_var = get_variable("shape0", prefix="", depth=depth+1)
        node.nodes.append(ExprStmt(
                                child=DeclAssign(
                                         type=Type(cpp="hsize_t"),
                                         target=Var(name=shape_var),
                                         value=Raw(code="shape["
                                                   +str(dim_shape[0])+"]"))))
        if len(canon_shape[1:]) == 1:
            #not a compound type.
            item_canon, item_shape = canon_shape[1]
            #get nodes initializing our child type
            child_array = (item_shape if isinstance(item_shape, list) 
                                      else [item_shape])
            node.nodes.append(get_item_type(CANON_TO_NODE[item_canon],
                                            shape_array=child_array,
                                            prefix=template_args[container_type][0],
                                            depth=depth+1))
        else:
            #this is a compound type.
            child_dict = OrderedDict()
            #1. Get all child item type nodes, recursively.
            for i in range(1, len(canon_shape)):
                item_canon, item_shape = canon_shape[i]
                item_node = CANON_TO_NODE[item_canon]
                child_array = (item_shape if isinstance(item_shape, list)
                                          else [item_shape])
                node.nodes.append(get_item_type(item_node,
                                                shape_array=child_array,
                                                prefix=template_args[container_type][i-1],
                                                depth=depth+1))
                item_var = get_variable("item_type", 
                                        prefix=template_args[container_type][i-1],
                                        depth=depth+1)
                #2. Get item sizes.
                child_dict[item_var] = get_item_size(item_node, child_array,
                                                     depth=depth+1)
            #3. Create compound type using total item size.
            compound = H5Tcreate_compound(list(child_dict.values()))
            node.nodes.append(ExprStmt(child=Assign(target=Raw(code=type_var),
                                                    value=compound)))
            #4. Insert individual children into the compound type.            
            node.nodes.append(H5Tinsert(container_type, type_var, child_dict))
            
        if container_type in variable_length_types:
            child_item_var = get_variable("item_type", prefix="", depth=depth+1)
            array_node = ExprStmt(child=Assign(target=Var(name=type_var),
                                               value=H5Tarray_create2(
                                                           child_item_var,
                                                           rank=1,
                                                           dims="&"+shape_var)))
            
            node.nodes.append(array_node)
    return node

def get_item_size(t, shape_array=None, depth=0):
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
            return "shape[" + str(shape_array[0]) + "]"
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
                                           depth=depth+1))
                size += "+".join(children)
            else:
                child_array = shape_array[1]
                if not isinstance(child_array, list):
                    child_array = [child_array]
                size += get_item_size(CANON_TO_NODE[t.canon[1]], child_array,
                                      depth=depth+1)
            size += ")"
            if t.canon[0] in variable_length_types:
                size += "*" + "shape[" + str(shape_array[0]) + "]"
        size += ")"
        return size    

def H5Tarray_create2(item_variable, rank=1, dims="&shape0"):
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

def H5Tcreate_compound(sizes):
    """Node representation of the HDF5 compound type creation function.
    
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
    
def H5Tinsert(container_type, compound_var, types_sizes_dict):
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

def main():
    global NOT_VL
    global VARIATION_DICT
    global ORIGIN_DICT
    
    output = ""
    CPPGEN = CppGen()
    try:
        gen_instruction = sys.argv[1]
    except:
        raise ValueError("No valid generation instruction provided")    
    if gen_instruction == "QUERY":
        for type in CANON_SET:
            type_node = CANON_TO_NODE[type]
            setup = get_setup(type_node)
            body = get_body(type_node)
            teardown = get_teardown(type_node)
            read_x = Block(nodes=[setup, body, teardown])
            output += CPPGEN.visit(case_template(type_node, read_x))
        print(output)
    elif gen_instruction == "VL_ADD":
        node = Block(nodes=[])
        vl_types = set([CANON_TO_NODE[t] for t in CANON_SET 
                                         if DB_TO_VL[CANON_TO_NODE[t].db]])
        for t in vl_types:
            is_compound = (variable_length_types[t.canon[0]] 
                           and len(t.canon[1:]) > 1)
            
            #this needs to be either a primitive HDF5 type,
            #or a compound item_type built from the size of
            #the individual primitives.
            item_type = ""
            if_statement = If(cond=BinOp(x=FuncCall(
                                              name=Raw(code="vldts_.count"),
                                              args=[Raw(code=t.db)]),
                                         op="==",
                                         y=Raw(code="0")),
                              body=[ExprStmt(child=Assign(
                                        target=Raw(code="vldts_["+t.db+"]"),
                                        value=FuncCall(
                                            name=Raw(code="H5Tvlen_create"),
                                            args=[Raw(code=item_type)]))),
                                    ExprStmt(child=FuncCall(
                                            name=Raw(
                                               code="opened_types_.insert"), 
                                            args=[Raw(
                                               code="vldts_["+t.db+"]")]))])
            node.nodes.append(if_statement)
        output += CPPGEN.visit(node)
        print(output)
    elif gen_instruction == "CREATE":
        
        #get all base types as node.
        fixed_length_types = set(t for t in CANON_SET 
                                 if no_vl(CANON_TO_NODE[t]))
        
        #Use base types as keys, vals are lists of VL variant nodes.
        VARIATION_DICT = {n: [CANON_TO_NODE[x] for x in CANON_SET 
                                                    if (CANON_TO_NODE[x].cpp 
                                                        == CANON_TO_NODE[n].cpp)
                                                    and (CANON_TO_NODE[x].db 
                                                        != CANON_TO_NODE[n].db)]
                          for n in fixed_length_types}
        
        #Add Blob because it was initially filtered out as a VL type
        VARIATION_DICT[('BLOB')] = []
        
        for i in VARIATION_DICT.keys():
            ORIGIN_DICT[i] = i
            if VARIATION_DICT[i] != []:
                for j in VARIATION_DICT[i]:
                    ORIGIN_DICT[j.canon] = i
                    
        NOT_VL = [x for x in VARIATION_DICT.keys() if not VARIATION_DICT[x]] 
        
        #bodies of base types to use as top level if/else if stmts
        if_bodies = {n: Block(nodes=[]) for n in VARIATION_DICT.keys()}
        
        for n in VARIATION_DICT.keys():
            variations = VARIATION_DICT[n]
            key_node = CANON_TO_NODE[n]
            try:
                initial_type = variations.pop()
                sub_if = If(cond=get_vl_cond(initial_type),
                            body=[get_vl_body(initial_type)],
                            elifs=[(get_vl_cond(v), 
                                   [get_vl_body(v)]) 
                                  for v in variations],
                            el=Block(nodes=[get_vl_body(key_node)]))
                #Nodes for every base type
                if_bodies[n].nodes.append(sub_if)
            except IndexError:
                lone_node = get_vl_body(key_node)
                if_bodies[n].nodes.append(lone_node)

        initial_node, initial_body = if_bodies.popitem()
        if_statement = If(cond=BinOp(x=Var(name="valtype"), op="==", 
                                     y=typeid(initial_node)),
                          body=[initial_body],
                          elifs=[(BinOp(x=Var(name="valtype"), op="==",
                                        y=typeid(t)), [if_bodies[t]])
                                        for t in if_bodies.keys()],
                          el=Nothing())
        output += CPPGEN.visit(if_statement)
        print(output)
    elif gen_instruction == "FILL":
        print(output)
    elif gen_instruction == "VL":
        print(output)
    else:
        raise ValueError("No valid generation instruction provided")   
if __name__ == '__main__':
    main()

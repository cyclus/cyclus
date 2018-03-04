#!/usr/bin/env python
"""This module generates HDF5 backend code found in src/hdf5_back.cc

There are 8 distinct code generation options, one of which must be passed
as an argument to this module. They are CREATE, QUERY, VL_DATASET,
FILL_BUF, WRITE, VAL_TO_BUF_H, VAL_TO_BUF, and BUF_TO_VAL. Each of these
generates a different section of Hdf5 backend code. All are invoked by
src/CMakeLists.txt prior to C++ compilation. However, for debugging purposes,
each section can be printed individually by passing that section's identifier
as a command line argument. The entry point for each of these generation
routines is the function main_XXXX, where XXXX is a generation option.

Example
-------
To generate the code found in src/hdf5_back.cc::Query, use

    $ python hdf5_back_gen.py QUERY

"""
import os
import sys
import json
from pprint import pformat
from itertools import chain
from collections import OrderedDict

MAIN_DISPATCH = {}

CANON_TO_NODE = {}
CANON_TYPES = []
DB_TO_CPP = {}
CANON_TO_DB = {}
DB_TO_VL = {}
INDENT = '  '

NOT_VL = []
VARIATION_DICT = OrderedDict()
ORIGIN_DICT = OrderedDict()
ORIGIN_TO_VL = {}

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
    def __init__(self, tree=None, indent='  ', debug=False):
        super(CppGen, self).__init__(tree=tree)
        self.level = 0
        self.indent = indent
        self.debug = debug

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
        s += ";"
        if self.debug:
            s = "std::cout << \"HDF5_DEBUG: " + s.replace('"', '\\"') + "\" << std::endl;\n" + s
        s += '\n'
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
            # We've taken care of the targs and no longer need them to appear
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
            setup_nodes.append(ExprStmt(child=DeclAssign(type=Type(cpp="unsigned int"),
                                                         target=Var(name=total_size_var),
                                                         value=Raw(code="CYCLUS_SHA1_SIZE"))))
            return Block(nodes=setup_nodes)

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
    if t.db != "BLOB" and DB_TO_VL[t.db]:
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
        body.nodes.append(ExprStmt(child=BinOp(x=Raw(code="field_types[i]"),
                                               op="=",
                                               y=Raw(code="sha1_type_"))))
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
            pre_opened_len = len(opened_stack)
            node.nodes.append(child_node)
            opened_stack.extend(child_opened)
            if pre_opened_len < len(opened_stack):
                item_var = opened_stack[-1]
            else:
                item_var = get_variable("item_type", prefix=new_prefix,
                                        depth=depth+1)

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
                if (DB_TO_VL[t.db] and item_canon[0] in variable_length_types):
                    child_item_var = get_variable("item_type",
                                                  prefix=new_prefix,
                                                  depth=depth+1)
                elif DB_TO_VL[item_node.db]:
                    child_item_var = get_variable("item_type",
                                                  prefix=new_prefix,
                                                  depth=depth+1)
                else:
                    if len(opened_stack) == pre_opened_len:
                        child_item_var = get_variable("item_type",
                                                      prefix=new_prefix,
                                                      depth=depth+1)
                    # However, if the current opened stack is longer, the first new
                    # variable there will be our child variable.
                    else:
                        child_item_var = opened_stack[-1]
                # 2. Get item sizes.
                child_dict[child_item_var] = get_item_size(item_node,
                                                           child_array,
                                                           vl_flag=is_vl,
                                                           depth=depth+1)
            # 3. Create compound type using total item size.
            compound = hdf5_create_compound(list(child_dict.values()))

            item_var = get_variable("item_type", prefix=prefix+'compound', depth=depth+1)
            node.nodes.append(ExprStmt(child=Decl(type=Type(cpp="hid_t"),
                                                  name=Raw(code=item_var))))

            node.nodes.append(ExprStmt(child=Assign(target=Raw(code=item_var),
                                                    value=compound)))

            opened_stack.append(item_var)
            # 4. Insert individual children into the compound type.
            node.nodes.append(hdf5_insert(container_type, item_var, child_dict))

        if is_vl and t.canon not in NOT_VL:
            node.nodes.append(ExprStmt(child=Assign(target=Raw(code=type_var),
                                                    value=Raw(code='sha1_type_'))))
            if vl_flag:
                node.nodes.append(VL_ADD_BLOCK(ORIGIN_TO_VL[ORIGIN_DICT[t.canon]], item_var))
            else:
                node.nodes.append(VL_ADD_BLOCK(t, item_var))
            opened_stack.append(type_var)

        elif container_type in variable_length_types and not DB_TO_VL[t.db]:
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
        if DB_TO_VL[t.db] or (vl_flag and t.canon not in NOT_VL):
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
    CPPGEN = CppGen(debug=False)
    output = ""
    for type in CANON_TYPES:
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
    CPPGEN = CppGen(debug=False)
    output = ""

    outer_if_bodies = OrderedDict()
    for n in VARIATION_DICT.keys():
        outer_if_bodies[n] = Block(nodes=[])

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
    """HDF5 VL_DATASET: Generate the VLDataset function code."""
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
    """HDF5 FILL_BUF: Generates the FillBuf function code."""
    CPPGEN = CppGen()
    output = ""
    for i in CANON_TYPES:
        node = CANON_TO_NODE[i]
        write_to_buf = FuncCall(name=Var(name="WriteToBuf"),
                                targs=[Raw(code=node.db)],
                                args=[Raw(code="buf+offset"),
                                      Raw(code="shapes[col]"),
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
if (vlkeys_[{t.db}].count({key}) != 1) {{
  AppendVLKey({keysds}, {t.db}, {key});
  InsertVLVal({valsds}, {t.db}, {key}, {var});
}}\n"""

vl_write_blob = """hasher_.Clear();
hasher_.Update({var});
Digest {key} = hasher_.digest();
hid_t {keysds} = VLDataset({t.db}, true);
hid_t {valsds} = VLDataset({t.db}, false);
if (vlkeys_[{t.db}].count({key}) != 1) {{
  AppendVLKey({keysds}, {t.db}, {key});
  InsertVLVal({valsds}, {t.db}, {key}, ({var}).str());
}}\n"""

VL_SPECIAL_TYPES = {"VL_STRING": vl_write_vl_string,
                    "BLOB": vl_write_blob}

def vl_write(t, variable, depth=0, prefix="", pointer=False):
    """HDF5 Write: Return code previously found in VLWrite."""
    buf_variable = get_variable("buf", depth=depth, prefix=prefix)
    key_variable = get_variable("key", depth=depth, prefix=prefix)
    keysds_variable = get_variable("keysds", depth=depth, prefix=prefix)
    valsds_variable = get_variable("valsds", depth=depth, prefix=prefix)
    if pointer:
        variable = "*" + variable
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
    node = Raw(code=node_str.format(var=variable, no_p_var=variable.strip("*"),
                                    key=key_variable,
                                    keysds=keysds_variable, t=t,
                                    valsds=valsds_variable,
                                    buf=buf_variable))
    return node

def memcpy(dest, src, size):
    """HDF5 Write: Node representation of memcpy function."""
    return ExprStmt(child=FuncCall(name=Var(name="memcpy"), args=[Raw(code=dest),
                                                                  Raw(code=src),
                                                                  Raw(code=size)]))

def memset(dest, src, size):
    """HDF5 Write: Node representation of memset function."""
    return ExprStmt(child=FuncCall(name=Var(name="memset"), args=[Raw(code=dest),
                                                                  Raw(code=src),
                                                                  Raw(code=size)]))

def a_cast(t, depth=0, prefix=""):
    """HDF5 Write: Node representation of boost hold_any casting."""
    cast = Block(nodes=[])
    val = get_variable("val", depth=depth, prefix=prefix)
    if (is_primitive(t) and t.db not in WRITE_BODY_PRIMITIVES
        and not DB_TO_VL[t.db]):
        cast.nodes.append(ExprStmt(child=Decl(type=Type(cpp="const void*"),
                                              name=Var(name=val))))
        cast_string = "a->castsmallvoid()"
        cast.nodes.append(ExprStmt(child=Assign(target=Var(name=val),
                                                value=Raw(code=cast_string))))
    else:
        cast_string = "a->cast<" + t.cpp + ">()"
        cast.nodes.append(ExprStmt(child=DeclAssign(
                                                  type=t,
                                                  target=Var(name=val),
                                                  value=Raw(code=cast_string))))
    return cast

def get_write_setup(t, shape_array, depth=0, prefix=""):
    """HDF5 Write: Creates setup variables (lengths, sizes) for function body.

    This function recursively declares the sizes, lengths and other necessary
    variables for the parent and children types. Called by get_write_body.

    Parameters
    ----------
    t : Type
    shape_array : list
    depth : int
    prefix : str

    Returns
    -------
    setup : Block
    """
    setup = Block(nodes=[])
    variable = get_variable("item_size", depth=depth, prefix=prefix)
    setup.nodes.append(ExprStmt(child=DeclAssign(type=Type(cpp="size_t"),
                                                 target=Var(name=variable),
                                                 value=Raw(code=get_item_size(t,
                                                                shape_array)))))
    if t.db == "STRING":
        valuelen = get_variable("valuelen", depth=depth, prefix=prefix)
        setup.nodes.append(ExprStmt(child=Decl(type=Type(cpp="size_t"),
                                               name=Var(name=(valuelen)))))
    if is_primitive(t):
        return setup
    else:
        # Setup prefixes and container-level length variable.
        container = t.canon[0]
        if DB_TO_VL[t.db]:
            container = VL_TO_FL_CONTAINERS[container]
        elif t.canon[0] in variable_length_types:
            length = get_variable("length", depth=depth, prefix=prefix)
            setup.nodes.append(ExprStmt(child=DeclAssign(
                                                    type=Type(cpp="size_t"),
                                                    target=Var(name=length),
                                                    value=Raw(code=
                                                            "shape["
                                                            +str(shape_array[0])
                                                            +"]"))))
        prefixes = template_args[container]

        # Add setup of any children.
        for c, s, p in zip(t.canon[1:], shape_array[1:], prefixes):
            node = CANON_TO_NODE[c]
            if isinstance(s, int):
                s = [s]
            setup.nodes.append(get_write_setup(node, s, depth=depth+1,
                                               prefix=prefix+p))
        total_item_size = get_variable("total_item_size", depth=depth,
                                       prefix=prefix)

        # Put together total_item_size variable.
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
        # remove elements that exceed column.
        if depth == 0 and not DB_TO_VL[t.db]:
            if container in variable_length_types:
                variable = get_variable("val", depth=depth, prefix=prefix)
                init = t.cpp + "::iterator eraseit=" + variable + ".begin()"
                advance = "std::advance(eraseit, column/" + total_item_size + ")"
                erase = variable + ".erase(eraseit," + variable + ".end())"
                column_check = If(cond=BinOp(x=Raw(code=total_item_size+"*"
                                                        +variable+".size()"),
                                             op=">", y=Raw(code='column')),
                                  body=[ExprStmt(child=Raw(code=init)),
                                        ExprStmt(child=Raw(code=advance)),
                                        ExprStmt(child=Raw(code=erase))])
                setup.nodes.append(column_check)

    return setup

def write_body_string(t, depth=0, prefix="", variable=None, offset="buf",
                      pointer=False):
    """HDF5 Write: Specialization of the write_body function for STRING type"""
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
                                                   Raw(code=variable+size),
                                                   Raw(code=item_size)]))))
    node.nodes.append(memcpy(offset, variable+c_str, valuelen))
    node.nodes.append(memset(offset+"+"+valuelen, "0", item_size+"-"+valuelen))
    return node

def write_body_uuid(t, depth=0, prefix="", variable=None, offset="buf",
                    pointer=False):
    """HDF5 Write: Specialization of the write_body function for UUID type"""
    if variable is None:
        variable = get_variable("val", depth=depth, prefix=prefix)
    node = Block(nodes=[])
    size = get_variable("item_size", depth=depth, prefix=prefix)
    if pointer:
        variable = "*" + variable
    variable = "&(" + variable + ")"
    node.nodes.append(memcpy(offset, variable, size))
    return node

def write_body_primitive(t, depth=0, prefix="", variable=None, offset="buf",
                         pointer=False):
    """HDF5 Write: Specialization of the write_body function for primitives"""
    if variable is None:
        variable = get_variable("val", depth=depth, prefix=prefix)
    node = Block(nodes=[])
    size = get_variable("item_size", depth=depth, prefix=prefix)
    if depth == 0:
        size = "column"
    else:
        if pointer:
            variable = "*" + variable
        variable = "&(" + variable + ")"
    node.nodes.append(memcpy(offset, variable, size))
    return node

WRITE_BODY_PRIMITIVES = {"STRING": write_body_string,
                         "UUID": write_body_uuid}

CONTAINER_INSERT_STRINGS = {"MAP": "{var}[{child0}] = {child1}",
                            "LIST": "{var}.push_back({child0})",
                            "SET": "{var}.insert({child0})",
                            "VECTOR": "{var}.push_back({child0})",
                            "PAIR": "{var} = std::make_pair({child0},{child1})"}

def is_all_vl(t):
    """HDF5 Write: Determines if type is entirely VL.

    A type is entirely VL if the top level type is VL, as well as all children
    that have the potential to be VL. This means that VL_VECTOR_INT will return
    True here, but VL_VECTOR_STRING will return False.

    Parameters
    ----------
    t : Type

    Returns
    -------
    result : bool
        True if type is entirely VL, else False
    """
    if is_primitive(t):
        if DB_TO_VL[t.db]:
            return True
    result = False
    flat = flatten(t.canon)
    for i in range(len(flat)):
        canon = flat[i]
        node = CANON_TO_NODE[canon]
        if DB_TO_VL[node.db]:
            result = True
            continue
        else:
            if i == 0:
                break
            elif node.canon in NOT_VL:
                continue
            else:
                result = False
                break
    return result

def pad_children(t, variable, fixed_var=None, depth=0, prefix="", called_depth=0):
    """HDF5 Write: Pads FL children of VL parent types.

    This function is used on top-level VL container types which contain 1 or
    more FL child types (i.e. VL_VECTOR_STRING). These children should be
    padded to their max length if they do not already meet it. This is done
    recursively.

    Parameters
    ----------
    t : Type
    variable : str
    fixed_var : None or str, optional
    depth : int, optional
    prefix : str, optional
    called_depth : int, optional
        Records the origin depth to determine when we're at relative depth=0

    Returns
    -------
    result : Block
        Nodes required for padding
    """
    if DB_TO_VL[t.db]:
        container = VL_TO_FL_CONTAINERS[t.canon[0]]
    else:
        container = t.canon[0]
    result = Block(nodes=[])
    body_nodes = []
    keywords = {}
    # Depth 0 should have no specified fixed_var variable. If this type is a
    # child, we'll want to use the child variable that was created for it
    # by its parent.
    if fixed_var == None:
        fixed_var = get_variable("fixed_val", depth=depth, prefix=prefix)
    result.nodes.append(ExprStmt(child=Decl(type=Type(cpp=t.cpp),
                                            name=Raw(code=fixed_var))))
    iterator = get_variable("it", depth=depth, prefix=prefix)
    pad_count = get_variable("pad_count", depth=depth, prefix=prefix)
    result.nodes.append(ExprStmt(child=DeclAssign(
                                                  type=Type(cpp='unsigned int'),
                                                  target=Var(name=pad_count),
                                                  value=Raw(code='0'))))
    prefixes = template_args[container]
    keywords['var'] = fixed_var
    num = len(t.canon[1:])
    if num == 1:
        children = ["(*" + iterator + ")"]
    else:
        if container in variable_length_types:
            members = ['->first', '->second']
            children = ["{}{}".format(a, b) for a, b in zip([iterator]*num,
                                                            members)]
        else:
            members = ['.first', '.second']
            children = ["{}{}".format(a, b) for a, b in zip([variable]*num,
                                                            members)]
    count = 0
    for i in t.canon[1:]:
        child_node = CANON_TO_NODE[i]
        child_keyword = "child" + str(count)
        child_variable = get_variable("child", depth=depth+1,
                                      prefix=prefix+prefixes[count])
        if is_primitive(child_node):
            # Strings are the only primitive we are looking for
            item_size = get_variable("item_size", depth=depth+1,
                                     prefix=prefix+prefixes[count])
            if child_node.db == 'STRING':
                constructor = ("std::string(" + children[count] + ",0,"
                              + item_size + ")")
                body_nodes.append(ExprStmt(child=DeclAssign(
                                                   type=child_node,
                                                   target=Var(
                                                           name=child_variable),
                                                   value=Raw(
                                                           code=constructor))))
                keywords[child_keyword] = child_variable
            # Leave other primitives alone.
            else:
                keywords[child_keyword] = children[count]
        else:
            # All VL containers
            if DB_TO_VL[child_node.db]:
                if is_all_vl(child_node):
                    # Skip child
                    keywords[child_keyword] = children[count]
                else:
                    # Recursion for VL containers may work this way
                    body_nodes.append(pad_children(child_node, children[count],
                                                   fixed_var=child_variable,
                                                   depth=depth+1,
                                                   prefix=prefix
                                                          +prefixes[count],
                                                   called_depth=called_depth))
                    keywords[child_keyword] = child_variable
            # FL variable length containers
            elif child_node.canon[0] in variable_length_types:
                child_length = get_variable("length", depth=depth+1,
                                      prefix=prefix+prefixes[count])
                child_pad_count = get_variable("pad_count", depth=depth+1,
                                         prefix=prefix+prefixes[count])
                body_nodes.append(pad_children(child_node, children[count],
                                               fixed_var=child_variable,
                                               depth=depth+1,
                                               prefix=prefix+prefixes[count],
                                               called_depth=called_depth))
                # attempt to resize container
                if child_node.canon[0] == 'VECTOR':
                    body_nodes.append(ExprStmt(child=Raw(code=child_variable+".resize("+child_length+")")))
                    size = "(" + child_length + "-" + child_pad_count + ")" + "*" + item_size
                    body_nodes.append(memset("&"+child_variable, str(0), size))
                keywords[child_keyword] = child_variable
            # PAIRS, etc.
            else:
                # Recursive call on the PAIR, using the parent iterator as the
                # new origin variable. We specify that the fixed variable should
                # simply be the 'child' variable we created in this loop.
                body_nodes.append(pad_children(child_node, children[count],
                                               fixed_var=child_variable,
                                               depth=depth+1,
                                               prefix=prefix+prefixes[count],
                                               called_depth=called_depth))
                keywords[child_keyword] = child_variable
        count += 1

    assignment = CONTAINER_INSERT_STRINGS[container].format(**keywords)
    body_nodes.append(ExprStmt(child=Raw(code=assignment)))
    if container in variable_length_types:
        body_nodes.append(ExprStmt(child=Raw(code="++" + pad_count)))
        if depth > called_depth:
            result.nodes.append(ExprStmt(child=DeclAssign(
                                                  type=Type(cpp=t.cpp
                                                                +"::iterator"),
                                                  target=Raw(code=iterator),
                                                  value=Raw(code=variable
                                                                 +".begin()"))))
        result.nodes.append(For(cond=BinOp(x=Var(name=iterator), op="!=",
                                           y=Var(name=variable+".end()")),
                                incr=Raw(code="++" + iterator),
                                body=body_nodes))
    else:
        result.nodes.extend(body_nodes)
    return result

def get_write_body(t, shape_array, depth=0, prefix="", variable="a",
                   offset="buf", pointer=False):
    """HDF5 Write: Generates the body of the WriteToBuf function definition.

    Parameters
    ----------
    t : Type
        Node representing the desired C++ type
    shape_array : list
        Dimensioned list of shape array indicies corresponding to types in
        t.canon
    depth : int, optional
        Recursive depth
    prefix : str, optional
        Used for recursive variable naming convention
    variable : str, optional
        Name of the type's C++ variable
    offset : str, optional
        Location of current memory offset
    pointer : bool, optional
        Denotes if current variable is a pointer, and whether member access
        should be performed via arrow or dot notation

    Returns
    -------
    result : Block
        Nodes required for body of the function definition
    """
    result = Block(nodes=[])
    # Determine if type is entirely variable length
    all_vl = is_all_vl(t)
    # Declare and assign the 'val' variable
    if depth == 0:
        variable = get_variable("val", depth=depth, prefix=prefix)
        result.nodes.append(a_cast(t))
        result.nodes.append(get_write_setup(t, shape_array))

    # If entirely variable length, we can simply use the VLWrite definition
    if all_vl:
        result.nodes.append(vl_write(t, variable, depth=depth, prefix=prefix,
                                     pointer=pointer))
        key = get_variable("key", depth=depth, prefix=prefix)
        result.nodes.append(memcpy(offset, key + ".val", "CYCLUS_SHA1_SIZE"))
        return result
    # Handle primitive bodies
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
    # Handle potentially variable length bodies
    else:
        # Declare count and iterator variables for the loop.
        count = get_variable("count", depth=depth, prefix=prefix)
        result.nodes.append(ExprStmt(child=DeclAssign(
                                                  type=Type(cpp="unsigned int"),
                                                  target=Var(name=count),
                                                  value=Raw(code="0"))))
        iterator = get_variable("it", depth=depth, prefix=prefix)
        total_size = get_variable("total_item_size", depth=depth, prefix=prefix)
        # Recursively gather child bodies
        child_bodies = []
        container = t.canon[0]
        # Handle variable length container
        if DB_TO_VL[t.db]:
            container = VL_TO_FL_CONTAINERS[container]
            prefixes = template_args[container]
            result.nodes.append(ExprStmt(child=DeclAssign(
                                              type=Type(cpp=t.cpp+"::iterator"),
                                              target=Raw(code=iterator),
                                              value=Raw(code=variable
                                                             +".begin()"))))
            result.nodes.append(pad_children(t, variable, depth=depth,
                                             prefix=prefix, called_depth=depth))
            fixed_val = get_variable("fixed_val", depth=depth, prefix=prefix)
            result.nodes.append(vl_write(t, fixed_val, depth=depth,
                                         prefix=prefix))
            key = get_variable("key", depth=depth, prefix=prefix)
            result.nodes.append(memcpy(offset, key + ".val", "CYCLUS_SHA1_SIZE"))
        # Handle fixed length containers
        else:
            new_variable = variable
            if container in variable_length_types:
                new_variable = iterator
            prefixes = template_args[container]
            if len(t.canon[1:]) == 1:
                child_node = CANON_TO_NODE[t.canon[1]]
                child_size = get_variable("item_size", depth=depth+1,
                                          prefix=prefix+prefixes[0])
                child_bodies.append(get_write_body(child_node, shape_array[1],
                                                   depth=depth+1,
                                                   prefix=prefix+prefixes[0],
                                                   variable=new_variable,
                                                   offset=offset+"+"+child_size
                                                          +"*"+count,
                                                   pointer=True))
            else:
                partial_size = "0"
                if container in variable_length_types:
                    labels = ['->first', '->second']
                elif pointer:
                    labels = ['->first', '->second']
                else:
                    labels = ['.first', '.second']
                for c, s, p, l in zip(t.canon[1:], shape_array[1:], prefixes,
                                      labels):
                    child_node = CANON_TO_NODE[c]
                    item_label = new_variable+l
                    child_size = get_variable("item_size", depth=depth+1,
                                              prefix=prefix+p)
                    child_bodies.append(get_write_body(child_node, s,
                                                       depth=depth+1,
                                                       prefix=prefix+p,
                                                       variable=item_label,
                                                       offset=offset+"+("+count
                                                              +"*"+total_size
                                                              +")+"
                                                              +partial_size,
                                                       pointer=False))
                    partial_size += "+" + child_size
            if container in variable_length_types:
                labels = ['->first', '->second']
                # For loop uses child bodies
                result.nodes.append(ExprStmt(child=DeclAssign(
                                                  type=Type(cpp=t.cpp
                                                                +"::iterator"),
                                                  target=Raw(code=new_variable),
                                                  value=Raw(code=variable
                                                                 +".begin()"))))
                child_bodies.append(ExprStmt(child=LeftUnaryOp(op="++",
                                                               name=Var(
                                                                     name=count))))
                result.nodes.append(For(cond=BinOp(x=Var(name=new_variable),
                                                   op="!=",
                                                   y=Var(name=variable
                                                              +".end()")),
                                        incr=Raw(code="++" + new_variable),
                                        body=child_bodies))
                # Add memset statement outside of loop
                container_length = get_variable("length", depth=depth,
                                                prefix=prefix)
                dest = offset + "+" + total_size + "*" + count
                length = (total_size + "*" + "(" + container_length + "-"
                          + count + ")")
                if depth == 0:
                    result.nodes.append(If(cond=BinOp(
                                                  x=Raw(code=total_size+"*"
                                                             +container_length),
                                                  op="<", y=Raw(code="column")),
                                           body=[memset(dest, str(0), length)]))
                else:
                    result.nodes.append(memset(dest, str(0), length))
            else:
                result.nodes.extend(child_bodies)
        return result

def main_write():
    """HDF5 Write: Generate the WriteToBuf templated function definitions."""
    CPPGEN = CppGen(debug=False)
    output = ""
    for i in CANON_TYPES:
        block = Block(nodes=[])
        t = CANON_TO_NODE[i]
        node = FuncDef(type=Raw(code="void"),
                       name=Var(name="Hdf5Back::WriteToBuf"),
                       targs=[Raw(code=t.db)],
                       args=[Decl(type=Type(cpp="char*"), name=Var(name="buf")),
                             Decl(type=Type(cpp="std::vector<int>&"),
                                  name=Var(name="shape")),
                             Decl(type=Type(
                                          cpp="const boost::spirit::hold_any*"),
                                  name=Var(name="a")),
                             Decl(type=Type(cpp="size_t"),
                                  name=Var(name="column"))],
                       body=[get_write_body(t, get_dim_shape(t.canon)[1])],
                       tspecial=True)
        block.nodes.append(node)
        output += CPPGEN.visit(block)
    return output

def to_from_buf_setup(t, depth=0, prefix="", spec=None):
    """HDF5 VAL_TO_BUF and BUF_TO_VAL: Generate setup for both functions.

    This setup is to be called one time for each type. It returns nodes for
    initial buffer/type declaration, item sizes, and a potentially dimensioned
    list describing which child types within the initial container t are VL.
    These are denoted by a 1, where fixed-length primitive types are denoted by
    a 0. Fixed-length containers (i.e. pairs) are denoted by a nested list of
    1's and 0's.

    Parameters
    ----------
    t : Type
        C++ type node.
    depth : int, optional
        Recursive depth counter, used for variable names.
    prefix : str, optional
        Current prefix, used for variable name uniqueness.
    spec : str or None, optional
        Determines whether extra nodes are added for VAL_TO_BUF or BUF_TO_VAL

    Returns
    -------
    node : Block
        All setup nodes.
    vl_list : list
        Potentially dimensioned list cooresponding to child types, with values
        of 0 and 1 representing FL and VL types, respectively.
    """
    node = Block(nodes=[])
    # Handle specializations for VLValToBuf and VLBufToVal functions.
    if depth == 0:
        if spec == 'TO_BUF':
            node.nodes.append(ExprStmt(child=Decl(type=Type(cpp="hvl_t"),
                                                  name=Var(name="buf"))))
            node.nodes.append(ExprStmt(child=Assign(
                                                   target=Var(name="buf.len"),
                                                   value=Raw(code="x.size()"))))
        elif spec == 'TO_VAL':
            val = get_variable('x', depth=depth, prefix=prefix)
            node.nodes.append(ExprStmt(child=Decl(type=t, name=Var(name=val))))
            node.nodes.append(ExprStmt(child=DeclAssign(
                                                type=Type(cpp='char*'),
                                                target=Var(name='p'),
                                                value=reinterpret_cast(
                                                              Type(cpp='char'),
                                                              'buf.p'))))
    child_sizes = OrderedDict()
    container = t.canon[0]
    prefixes = template_args[container]
    children = t.canon[1:]
    vl_list = []
    # Iterate, determine sizes and whether type is VL
    for c, p in zip(children, prefixes):
        child_node = CANON_TO_NODE[c]
        variable = get_variable("item_size", depth=depth+1, prefix=prefix+p)
        if is_primitive(child_node):
            if child_node.db == "BLOB":
                child_sizes[variable] = "CYCLUS_SHA1_SIZE"
                vl_list.append(1)
            elif VARIATION_DICT[c]:
                child_sizes[variable] = "CYCLUS_SHA1_SIZE"
                vl_list.append(1)
            else:
                child_sizes[variable] = PRIMITIVE_SIZES[child_node.db]
                vl_list.append(0)
        elif child_node.canon[0] in variable_length_types:
            child_sizes[variable] = "CYCLUS_SHA1_SIZE"
            vl_list.append(1)
        else:
            child_sizes[variable] = get_variable("total_item_size", depth+1,
                                                 prefix=prefix+p)
            new_node, new_list = to_from_buf_setup(child_node, depth=depth+1,
                                                   prefix=prefix+p)
            node.nodes.append(new_node)
            vl_list.append(new_list)
    # Unpack and declare all child sizes.
    for k, v in child_sizes.items():
        node.nodes.append(ExprStmt(child=DeclAssign(type=Type(cpp="size_t"),
                                                    target=Raw(code=k),
                                                    value=Raw(code=v))))
    total_var = get_variable("total_item_size", depth=depth, prefix=prefix)
    node.nodes.append(ExprStmt(child=DeclAssign(
                                        type=Type(cpp="size_t"),
                                        target=Raw(code=total_var),
                                        value=Raw(
                                           code="+".join(child_sizes.keys())))))
    # Further specializations.
    if depth == 0:
        if spec == 'TO_BUF':
            node.nodes.append(ExprStmt(child=DeclAssign(
                                                  type=Type(cpp="size_t"),
                                                  target=Var(name="nbytes"),
                                                  value=Raw(code=total_var
                                                                 +"*buf.len"))))
            node.nodes.append(ExprStmt(child=Assign(
                                              target=Var(name="buf.p"),
                                              value=Raw(
                                                     code="new char[nbytes]"))))
    return node, vl_list

def to_buf_body(t, vl_list, depth=0, prefix="", variable=None,
                offset="reinterpret_cast<char*>(buf.p)"):
    """HDF5 VAL_TO_BUF: Generates the body of the VLValToBuf function.

    The VLValToBuf function creates a new VL buffer from an original C++ data
    type. All potentially variable length types are passed to VLWrite and a
    SHA1 hash is added to the buffer in place of the actual type data.
    Primitives and remaining container types are written as-is to the buffer.

    Parameters
    ----------
    t : Type
        Node representing current C++ type
    vl_list : list
        Potentially dimensioned list of 1's and 0's, corresponding to each
        child type and whether it is variable length or not, respectively.
    depth : int, optional
        Current recursive depth, used for naming variables.
    prefix : str, optional
        Current variable prefix, used to ensure unique variable names.
    variable : str, optional
        Current container variable name.
    offset : str or None, optional
        Current offset into data.

    Returns
    -------
    block : Block
        Nodes representing the body.
    """
    if variable == None:
        variable = 'x'
    block = Block(nodes=[])
    total_size_var = get_variable("total_item_size", depth=depth, prefix=prefix)
    prefixes = template_args[t.canon[0]]
    children = t.canon[1:]
    loop_block = Block(nodes=[])
    new_offset = offset
    # If a container is VL and has multiple children, we'll need to use pointer
    # notation to access the child data. Otherwise, we can use normal dot
    # notation.
    if t.canon[0] in variable_length_types:
        count_var = get_variable("count", depth=depth, prefix=prefix)
        block.nodes.append(ExprStmt(child=DeclAssign(type=Type(
                                                            cpp="unsigned int"),
                                                     target=Var(name=count_var),
                                                     value=Raw(code="0"))))
        iter_var = get_variable("it", depth=depth, prefix=prefix)
        block.nodes.append(ExprStmt(child=DeclAssign(
                                             type=Type(cpp=t.cpp
                                                           +"::const_iterator"),
                                             target=Var(name=iter_var),
                                             value=Raw(code=variable
                                                            +".begin()"))))
        new_variable = iter_var
        labels = ['->first', '->second']
        new_offset += "+(" + total_size_var + "*" + count_var + ")"
    else:
        labels = ['.first', '.second']
        new_variable = variable
    # Containers with only one child can simply use the iterator to reference
    # their child data.
    if len(children) == 1:
        new_variable = "(*" + new_variable + ")"
        labels = ['']
    for child, part, vl, label in zip(children, prefixes, vl_list, labels):
        child_node = CANON_TO_NODE[child]
        child_var = new_variable + label
        item_size = get_variable("item_size", depth=depth+1, prefix=prefix+part)
        # For variable length types we must use VLWrite to get the SHA1
        if vl == 1:
            loop_block.nodes.append(vl_write(ORIGIN_TO_VL[child], child_var,
                                             depth=depth+1, prefix=prefix+part))
            key_var = get_variable("key", depth=depth+1, prefix=prefix+part)
            loop_block.nodes.append(memcpy(new_offset, key_var+".val",
                                           item_size))
        # Other primitives can be copied
        elif vl == 0:
            loop_block.nodes.append(memcpy(new_offset, "&("+child_var+")",
                                          item_size))
        # Other containers must be handled recursively.
        else:
            loop_block.nodes.append(to_buf_body(child_node, vl, depth=depth+1,
                                    prefix=prefix+part, variable=child_var,
                                    offset=new_offset))
        # Update current offset.
        new_offset += "+" + item_size

    # For variable length containers, add the for loop.
    if t.canon[0] in variable_length_types:
        block.nodes.append(For(cond=BinOp(x=Var(name=iter_var), op="!=",
                                          y=Var(name=variable+".end()")),
                               incr=Raw(code="++"+iter_var),
                               body=[loop_block,
                                     ExprStmt(child=LeftUnaryOp(
                                                      op="++",
                                                      name=Var(
                                                            name=count_var)))]))
    # Other containers don't need a loop (i.e. pair).
    else:
        block.nodes.append(loop_block)
    if depth == 0:
        block.nodes.append(ExprStmt(child=Raw(code="return buf")))
    return block

def main_val_to_buf():
    """HDF5 VAL_TO_BUF: Generates VLValToBuf function."""
    CPPGEN = CppGen(debug=False)
    output = ""
    block = Block(nodes=[])
    for i in VARIATION_DICT:
        if VARIATION_DICT[i]:
            t = CANON_TO_NODE[i]
            if t.canon[0] in variable_length_types:
                setup, vl_list = to_from_buf_setup(t, spec='TO_BUF')
                node = FuncDef(type=Type(cpp="hvl_t"),
                               name=Var(name="Hdf5Back::VLValToBuf"),
                               args=[Decl(type=Type(cpp="const "+t.cpp+"&"),
                                          name=Var(name="x"))],
                               body=[setup, to_buf_body(t, vl_list)])
                block.nodes.append(node)
    output += CPPGEN.visit(block)
    return output

def main_val_to_buf_h():
    """HDF5 VAL_TO_BUF_H: Generates header declarations for VLValToBuf function."""
    CPPGEN = CppGen()
    output = ""
    block = Block(nodes=[])
    for i in VARIATION_DICT:
        if VARIATION_DICT[i]:
            node = CANON_TO_NODE[i]
            if node.canon[0] in variable_length_types:
                decl = ExprStmt(child=Decl(type=Type(cpp="hvl_t"),
                                           name=FuncCall(
                                                 name=Var(name="VLValToBuf"),
                                                 args=[Decl(type=Type(
                                                                    cpp="const "
                                                                        +node.cpp
                                                                        +"&"),
                                                            name=Raw(code="x"))])))
                block.nodes.append(decl)
    output += CPPGEN.visit(block)
    output = indent(output, INDENT)
    return output

def vl_read(t, offset):
    """Representation of C++ VLRead function.

    Parameters
    ----------
    t : Type
        C++ type node.
    offset : str
        Memory location of SHA1 hash.

    Returns
    -------
    node : FuncCall
        The final function call.
    """
    node = FuncCall(name=Var(name='VLRead'),
                    targs=[Raw(code=t.cpp),
                           Raw(code=ORIGIN_TO_VL[t.canon].db)],
                    args=[Raw(code=offset)])
    return node

def reinterpret_cast(t, offset, deref=False):
    """Representation of C++ reinterpret_cast function.

    Parameters
    ----------
    t : Type
        C++ type to cast as.
    offset : str
        Memory location of the data to cast.
    deref : bool, optional
        Should the function be dereferenced? (This returns the newly casted
        data, rather than a pointer)

    Returns
    -------
    node : FuncCall
        The final function call.
    """
    if deref:
        func_name = '*reinterpret_cast'
    else:
        func_name = 'reinterpret_cast'
    node = FuncCall(name=Var(name=func_name),
                    targs=[Raw(code=t.cpp+'*')], args=[Raw(code=offset)])
    return node

def to_val_body(t, vl_list, depth=0, prefix='', variable='x0', offset=None):
    """Generates the body of the VLBufToVal function.

    The VLBufToVal function is responsible for reading the bytes of a VL buffer
    back into a C++ value. Importantly, we assume that all types which have the
    capability of being VL *are* VL. When we encounter one of these types, we
    call VLRead, passing in the respective SHA1 hash value. Otherwise, we read
    in the fixed length number of bytes associated with the type.

    Parameters
    ----------
    t : Type
        Node representing current C++ type
    vl_list : list
        Potentially dimensioned list of 1's and 0's, corresponding to each
        child type and whether it is variable length or not, respectively.
    depth : int, optional
        Current recursive depth, used for naming variables.
    prefix : str, optional
        Current variable prefix, used to ensure unique variable names.
    variable : str, optional
        Current container variable name.
    offset : str or None, optional
        Current offset into buffer.

    Returns
    -------
    block : Block
        Nodes representing the body.
    """
    block = Block(nodes=[])
    child_count = 0
    # argument dict, for unpacking later in str formatting
    args = {}
    args['var'] = variable
    total_item_size = get_variable('total_item_size', depth=depth,
                                   prefix=prefix)
    count = get_variable('count', depth=depth, prefix=prefix)
    # set default offset if none given
    if offset == None:
        offset = 'p+' + "(" + total_item_size + '*' + count + ")"
    container = t.canon[0]
    loop_block = Block(nodes=[])
    for child, part, vl in zip(t.canon[1:], template_args[t.canon[0]], vl_list):
        type_node = CANON_TO_NODE[child]
        child_var = get_variable('x', depth=depth+1, prefix=prefix+part)
        child_size = get_variable('item_size', depth=depth+1,
                                  prefix=prefix+part)
        child_arg = 'child' + str(child_count)
        # any variable length type can be resolved using VLRead, even containers
        if vl == 1:
            loop_block.nodes.append(ExprStmt(child=DeclAssign(
                                                     type=type_node,
                                                     target=Var(name=child_var),
                                                     value=vl_read(type_node,
                                                                   offset))))
        # read in primitive, fixed-length types
        elif vl == 0:
            loop_block.nodes.append(ExprStmt(child=DeclAssign(
                                                     type=type_node,
                                                     target=Var(name=child_var),
                                                     value=reinterpret_cast(
                                                                  type_node,
                                                                  offset,
                                                                  deref=True))))
        # structures which are neither primitive nor VL, i.e. std::pairs, must
        # be handled recursively
        else:
            loop_block.nodes.append(ExprStmt(child=Decl(
                                                     type=type_node,
                                                     name=Var(name=child_var))))
            loop_block.nodes.append(to_val_body(type_node, vl, depth=depth+1,
                                                prefix=prefix+part,
                                                offset=offset,
                                                variable=child_var))
        args[child_arg] = child_var
        offset += "+" + child_size
        child_count += 1
    # This is the expression which adds a type to a given container, formatted
    # with the variables associated with child types. This must be placed in a
    # for loop if the container is variable length.
    container_expr = CONTAINER_INSERT_STRINGS[container].format(**args)
    if container in variable_length_types:
        block.nodes.append(ExprStmt(child=DeclAssign(type=Type(
                                                            cpp='unsigned int'),
                                                     target=Var(name=count),
                                                     value=Raw(code='0'))))
        block.nodes.append(For(cond=BinOp(x=Var(name=count), op='<',
                                          y=Var(name='buf.len')),
                               incr=Raw(code='++'+count),
                               body=[loop_block,
                                     ExprStmt(child=Raw(code=container_expr))]))
    else:
        block.nodes.append(loop_block)
        block.nodes.append(ExprStmt(child=Raw(code=container_expr)))
    if depth == 0:
        block.nodes.append(ExprStmt(child=Raw(code='return ' + variable)))
    return block

def main_buf_to_val():
    """HDF5 BUF_TO_VAL: Generates the VLBufToVal function code."""
    CPPGEN = CppGen()
    output = ""
    block = Block(nodes=[])
    for i in VARIATION_DICT:
        if VARIATION_DICT[i]:
            t = CANON_TO_NODE[i]
            if t.canon[0] in variable_length_types:
                setup, vl_list = to_from_buf_setup(t, spec='TO_VAL')
                node = FuncDef(type=Type(cpp=t.cpp),
                               name=Var(name="Hdf5Back::VLBufToVal"),
                               targs=[Raw(code=t.cpp)],
                               args=[Decl(type=Type(cpp="const hvl_t&"),
                                          name=Var(name="buf"))],
                               body=[setup, to_val_body(t, vl_list)],
                               tspecial=True)
                block.nodes.append(node)
    output += CPPGEN.visit(block)
    return output

def setup():
    global NOT_VL
    global MAIN_DISPATCH

    with open(os.path.join(os.path.dirname(__file__), '..', 'share',
                       'dbtypes.json')) as f:
        raw_table = resolve_unicode(json.load(f))

    version = ""
    table_start = 0
    table_end = 0
    for row in range(len(raw_table)):
        current = tuple(raw_table[row])
        if current[4] == "HDF5":
            if current[5] > version:
                version = current[5]
                table_start = row
            if current[5] == version:
                table_end = row

    types_table = list(tuple(row) for row in raw_table[table_start:table_end+1])

    for row in types_table:
        if row[6] == 1 and row[4] == "HDF5" and row[5] == version:
            db = row[1]
            cpp = row[2]
            canon = convert_canonical(row[7])
            if canon not in CANON_TYPES:
                CANON_TYPES.append(canon)
            DB_TO_CPP[db] = cpp
            CANON_TO_DB[canon] = db
            CANON_TO_NODE[canon] = Type(cpp=cpp, db=db, canon=canon)
            DB_TO_VL[db] = row[8]

    fixed_length_types = []
    for n in CANON_TYPES:
        if no_vl(CANON_TO_NODE[n]) and n not in fixed_length_types:
            fixed_length_types.append(n)

    for n in fixed_length_types:
        key = CANON_TO_NODE[n]
        vals = []
        for x in CANON_TYPES:
            val_node = CANON_TO_NODE[x]
            if val_node.cpp == key.cpp and val_node.db != key.db:
                vals.append(val_node)
        VARIATION_DICT[n] = vals

    VARIATION_DICT['BLOB'] = []
    VARIATION_DICT['STRING'] = [CANON_TO_NODE['VL_STRING']]

    for i in VARIATION_DICT:
        ORIGIN_DICT[i] = i
        if VARIATION_DICT[i] != []:
            for j in VARIATION_DICT[i]:
                ORIGIN_DICT[j.canon] = i

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

    for n in CANON_TYPES:
        node = CANON_TO_NODE[n]
        if is_all_vl(node):
            ORIGIN_TO_VL[ORIGIN_DICT[n]] = node

    MAIN_DISPATCH = {"QUERY": main_query,
                     "CREATE": main_create,
                     "VL_DATASET": main_vl_dataset,
                     "FILL_BUF": main_fill_buf,
                     "WRITE": main_write,
                     "VAL_TO_BUF_H": main_val_to_buf_h,
                     "VAL_TO_BUF": main_val_to_buf,
                     "BUF_TO_VAL": main_buf_to_val}

def main():
    try:
        gen_instruction = sys.argv[1]
    except:
        raise ValueError("No generation instruction provided")

    # Setup for global util dictionaries
    setup()

    # Dispatch to requested generation function
    function = MAIN_DISPATCH[gen_instruction]
    print(function())

if __name__ == '__main__':
    main()

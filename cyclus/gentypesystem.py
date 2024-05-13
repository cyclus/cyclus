#!/usr/bin/env python3
"""Generates Cyclus Type System bindings.

Module history:

- 2016-10-12: scopatz: This file used to be called genapi.py in cymetric.
"""
from __future__ import print_function, unicode_literals

import io
import os
import sys
import json
import argparse
import platform
import warnings
import itertools
import subprocess
from glob import glob
from distutils import core, dir_util
from pprint import pprint, pformat
from collections import defaultdict
if sys.version_info[0] > 2:
    from urllib.request import urlopen
    str_types = (str, bytes)
    unicode_types = (str,)
else:
    from urllib2 import urlopen
    str_types = (str, unicode)
    unicode_types = (str, unicode)

import jinja2

#
# Type System
#

class TypeSystem(object):
    """A type system for cyclus code generation."""

    def __init__(self, table, cycver, rawver=None, cpp_typesystem='cpp_typesystem'):
        """Parameters
        ----------
        table : list
            A table of possible types. The first row must be the column names.
        cycver : tuple of ints
            Cyclus version number.
        rawver : string, optional
            A full, raw version string, if available
        cpp_typesystem : str, optional
            The namespace of the C++ wrapper header.

        Attributes
        ----------
        table : list
            A stripped down table of type information.
        cols : dict
            Maps column names to column number in table.
        cycver : tuple of ints
            Cyclus version number.
        verstr : str
            A version string of the format 'vX.X'.
        types : set of str
            The type names in the type system.
        ids : dict
            Maps types to integer identifier.
        cpptypes : dict
            Maps types to C++ type.
        ranks : dict
            Maps types to shape rank.
        norms : dict
            Maps types to programmatic normal form, ie INT -> 'int' and
            VECTOR_STRING -> ('std::vector', 'std::string').
        dbtypes : list of str
            The type names in the type system, sorted by id.
        uniquetypes : list of str
            The type names in the type system, sorted by id,
            which map to a unique C++ type.
        """
        self.cpp_typesystem = cpp_typesystem
        self.cycver = cycver
        self.rawver = rawver
        self.verstr = verstr = 'v{0}.{1}'.format(*cycver)
        self.cols = cols = {x: i for i, x in enumerate(table[0])}
        id, name, version = cols['id'], cols['name'], cols['version']
        cpptype, rank = cols['C++ type'], cols['shape rank']
        tab = []
        if rawver is not None:
            tab = [row for row in table if row[version] == rawver]
        if len(tab) == 0:
            tab = [row for row in table if row[version].startswith(verstr)]
            if len(tab) == 0:
                raise ValueError("Cyclus version could not be found in table!")
        self.table = table = tab
        self.types = types = set()
        self.ids = ids = {}
        self.cpptypes = cpptypes = {}
        self.ranks = ranks = {}
        for row in table:
            t = row[name]
            types.add(t)
            ids[t] = row[id]
            cpptypes[t] = row[cpptype]
            ranks[t] = row[rank]
        self.norms = {t: parse_template(c) for t, c in cpptypes.items()}
        self.dbtypes = sorted(ids.keys(), key=lambda t: ids[t])
        # find unique types
        seen = set()
        self.uniquetypes = uniquetypes = []
        for t in self.dbtypes:
            normt = self.norms[t]
            if normt in seen:
                continue
            else:
                uniquetypes.append(t)
                seen.add(normt)
        self.resources = tuple(RESOURCES)
        self.inventory_types = inventory_types = []
        for t in uniquetypes:
            normt = self.norms[t]
            if normt in INVENTORIES or normt[0] in INVENTORIES:
                inventory_types.append(t)

        # caches
        self._cython_cpp_name = {}
        self._cython_types = dict(CYTHON_TYPES)
        self._shared_ptrs = {}
        self._funcnames = dict(FUNCNAMES)
        self._classnames = dict(CLASSNAMES)
        self._vars_to_py = dict(VARS_TO_PY)
        self._vars_to_cpp = dict(VARS_TO_CPP)
        self._nptypes = dict(NPTYPES)
        self._new_py_insts = dict(NEW_PY_INSTS)
        self._to_py_converters = dict(TO_PY_CONVERTERS)
        self._to_cpp_converters = dict(TO_CPP_CONVERTERS)
        self._use_shared_ptr = defaultdict(lambda: False,
                                           {k: True for k in USE_SHARED_PTR})


    def cython_cpp_name(self, t):
        """Returns the C++ name of the type, eg INT -> cpp_typesystem.INT."""
        if t not in self._cython_cpp_name:
            self._cython_cpp_name[t] = '{0}.{1}'.format(self.cpp_typesystem, t)
        return self._cython_cpp_name[t]

    def cython_type(self, t):
        """Returns the Cython spelling of the type."""
        if t in self._cython_types:
            return self._cython_types[t]
        if isinstance(t, str_types):
            n = self.norms[t]
            return self.cython_type(n)
        # must be teplate type
        cyt = list(map(self.cython_type, t))
        cyt = '{0}[{1}]'.format(cyt[0], ', '.join(cyt[1:]))
        self._cython_types[t] = cyt
        return cyt

    def possibly_shared_cython_type(self, t):
        """Returns the Cython type, or if it is a shared pointer type,
        return the shared pointer version.
        """
        if self._use_shared_ptr[t]:
            cyt = self._shared_ptrs.get(t, None)
            if cyt is None:
                self._shared_ptrs[t] = 'shared_ptr[' + self.cython_type(t) + ']'
                cyt = self._shared_ptrs[t]
        else:
            cyt = self.cython_type(t)
        return cyt

    def funcname(self, t):
        """Returns a version of the type name suitable for use in a function name.
        """
        if t in self._funcnames:
            return self._funcnames[t]
        if isinstance(t, str_types):
            n = self.norms[t]
            return self.funcname(n)
        f = '_'.join(map(self.funcname, t))
        self._funcnames[t] = f
        return f

    def classname(self, t):
        """Returns a version of the type name suitable for use in a class name.
        """
        if t in self._classnames:
            return self._classnames[t]
        if isinstance(t, str_types):
            n = self.norms[t]
            return self.classname(n)
        c = ''.join(map(self.classname, t))
        self._classnames[t] = c
        return c

    def var_to_py(self, x, t):
        """Returns an expression for converting an object to Python."""
        n = self.norms.get(t, t)
        expr = self._vars_to_py.get(n, None)
        if expr is None:
            f = self.funcname(t)
            expr = f + '_to_py({var})'
            self._vars_to_py[n] = expr
        return expr.format(var=x)

    def hold_any_to_py(self, x, t):
        """Returns an expression for converting a hold_any object to Python."""
        cyt = self.possibly_shared_cython_type(t)
        cast = '{0}.cast[{1}]()'.format(x, cyt)
        return self.var_to_py(cast, t)

    def var_to_cpp(self, x, t):
        """Returns an expression for converting a Python object to C++."""
        n = self.norms.get(t, t)
        expr = self._vars_to_cpp.get(n, None)
        if expr is None:
            f = self.funcname(t)
            expr = f + '_to_cpp({var})'
            self._vars_to_cpp[n] = expr
        return expr.format(var=x)

    def py_to_any(self, a, val, t):
        """Returns an expression for assigning a Python object (val) to an any
        object (a)."""
        cyt = self.possibly_shared_cython_type(t)
        cpp = self.var_to_cpp(val, t)
        rtn = '{a}.assign[{cyt}]({cpp})'.format(a=a, cyt=cyt, cpp=cpp)
        return rtn

    def nptype(self, n):
        """Returns the numpy type for a normal form element."""
        npt = self._nptypes.get(n, None)
        if npt is None:
            npt = 'np.NPY_OBJECT'
            self._nptypes[n] = npt
        return npt

    def new_py_inst(self, t):
        """Returns the new instance for the type."""
        n = self.norms.get(t, t)
        inst = self._new_py_insts.get(n, None)
        if inst is None:
            # look up base type
            inst = self._new_py_insts.get(n[0], None)
        if inst is None:
            inst = 'None'
            self._new_py_insts[n] = inst
        return inst

    def convert_to_py(self, x, t):
        """Converts a C++ variable to python.

        Parameters
        ----------
        x : str
            variable name
        t : str
            variable type

        Returns
        -------
        decl : str
            Declarations needed for conversion, may be many lines.
        body : str
            Actual conversion implementation, may be many lines.
        rtn : str
            Return expression.
        """
        n = self.norms.get(t, t)
        ctx = {'type': self.cython_type(t), 'var': x, 'nptypes': [],
               'classname': self.classname(t), 'funcname': self.funcname(t)}
        if n in self._to_py_converters:
            # basic type or str
            n0 = ()
            decl, body, expr = self._to_py_converters[n]
        elif n[0] == 'std::vector' and self.nptype(n[1]) == 'np.NPY_OBJECT':
            # vector of type that should become an object
            n0 = n[0]
            decl, body, expr = self._to_py_converters['np.ndarray', 'np.NPY_OBJECT']
            ctx['elem_to_py'] = self.var_to_py(x + '[i]', n[1])
        else:
            # must be a template already
            n0 = n[0]
            decl, body, expr = self._to_py_converters[n0]
        for targ, n_i in zip(TEMPLATE_ARGS.get(n0, ()), n[1:]):
            x_i = x + '_' +  targ
            ctx[targ+'name'] = x_i
            ctx[targ+'type'] = self.cython_type(n_i)
            dbe_i = self.convert_to_py(x_i, n_i)
            dbe_i = map(Indenter, dbe_i)
            ctx[targ+'decl'], ctx[targ+'body'], ctx[targ+'expr'] = dbe_i
            ctx['nptypes'].append(self.nptype(n_i))
        decl = decl.format(**ctx)
        body = body.format(**ctx)
        expr = expr.format(**ctx)
        return decl, body, expr

    def convert_to_cpp(self, x, t):
        """Converts a Python variable to C++.

        Parameters
        ----------
        x : str
            variable name
        t : str
            variable type

        Returns
        -------
        decl : str
            Declarations needed for conversion, may be many lines.
        body : str
            Actual conversion implementation, may be many lines.
        rtn : str
            Return expression.
        """
        n = self.norms.get(t, t)
        ctx = {'type': self.cython_type(t), 'var': x, 'nptypes': [],
               'classname': self.classname(t), 'funcname': self.funcname(t)}
        if n in self._to_cpp_converters:
            # basic type or str
            n0 = ()
            decl, body, expr = self._to_cpp_converters[n]
        elif n[0] == 'std::vector' and self.nptype(n[1]) in ('np.NPY_OBJECT',
                                                             'np.NPY_BOOL'):
            # vector of type that should become an object
            n0 = n[0]
            decl, body, expr = self._to_cpp_converters['np.ndarray', 'np.NPY_OBJECT']
        else:
            # must be a template already
            n0 = n[0]
            decl, body, expr = self._to_cpp_converters[n0]
        for targ, n_i in zip(TEMPLATE_ARGS.get(n0, ()), n[1:]):
            x_i = x + '_' +  targ
            ctx[targ+'name'] = x_i
            ctx[targ+'type'] = self.cython_type(n_i)
            dbe_i = self.convert_to_cpp(x_i, n_i)
            dbe_i = map(Indenter, dbe_i)
            ctx[targ+'decl'], ctx[targ+'body'], ctx[targ+'expr'] = dbe_i
            ctx['nptypes'].append(self.nptype(n_i))
            ctx[targ+'_to_cpp'] = self.var_to_cpp(x_i, n_i)
        decl = decl.format(**ctx)
        body = body.format(**ctx)
        expr = expr.format(**ctx)
        return decl, body, expr


CYTHON_TYPES = {
    # type system types
    'BOOL': 'cpp_bool',
    'INT': 'int',
    'FLOAT': 'float',
    'DOUBLE': 'double',
    'STRING': 'std_string',
    'VL_STRING': 'std_string',
    'BLOB': 'cpp_cyclus.Blob',
    'UUID': 'cpp_cyclus.uuid',
    'MATERIAL': 'cpp_cyclus.Material',
    'PRODUCT': 'cpp_cyclus.Product',
    # C++ normal types
    'bool': 'cpp_bool',
    'int': 'int',
    'float': 'float',
    'double': 'double',
    'std::string': 'std_string',
    'std::string': 'std_string',
    'cyclus::Blob': 'cpp_cyclus.Blob',
    'boost::uuids::uuid': 'cpp_cyclus.uuid',
    'cyclus::Material': 'cpp_cyclus.Material',
    'cyclus::Product': 'cpp_cyclus.Product',
    # Template Types
    'std::set': 'std_set',
    'std::map': 'std_map',
    'std::pair': 'std_pair',
    'std::list': 'std_list',
    'std::vector': 'std_vector',
    'cyclus::toolkit::ResBuf': 'cpp_cyclus.ResBuf',
    'cyclus::toolkit::TotalInvTracker': 'cpp_cyclus.TotalInvTracker',
    'cyclus::toolkit::ResMap': 'cpp_cyclus.ResMap',
    }

# Don't include the base resource class here since it is pure virtual.
RESOURCES = ['MATERIAL', 'PRODUCT']

INVENTORIES = ['cyclus::toolkit::ResBuf',
               'cyclus::toolkit::TotalInvTracker', 'cyclus::toolkit::ResMap']

USE_SHARED_PTR = ('MATERIAL', 'PRODUCT', 
                  'cyclus::Material', 'cyclus::Product')

FUNCNAMES = {
    # type system types
    'BOOL': 'bool',
    'INT': 'int',
    'FLOAT': 'float',
    'DOUBLE': 'double',
    'STRING': 'std_string',
    'VL_STRING': 'std_string',
    'BLOB': 'blob',
    'UUID': 'uuid',
    'MATERIAL': 'material',
    'PRODUCT': 'product',
    # C++ normal types
    'bool': 'bool',
    'int': 'int',
    'float': 'float',
    'double': 'double',
    'std::string': 'std_string',
    'cyclus::Blob': 'blob',
    'boost::uuids::uuid': 'uuid',
    'cyclus::Material': 'material',
    'cyclus::Product': 'product',
    # Template Types
    'std::set': 'std_set',
    'std::map': 'std_map',
    'std::pair': 'std_pair',
    'std::list': 'std_list',
    'std::vector': 'std_vector',
    'cyclus::toolkit::ResBuf': 'res_buf',
    'cyclus::toolkit::TotalInvTracker': 'total_inv_tracker',
    'cyclus::toolkit::ResMap': 'res_map',
    }

CLASSNAMES = {
    # type system types
    'BOOL': 'Bool',
    'INT': 'Int',
    'FLOAT': 'Float',
    'DOUBLE': 'Double',
    'STRING': 'String',
    'VL_STRING': 'String',
    'BLOB': 'Blob',
    'UUID': 'Uuid',
    'MATERIAL': 'Material',
    'PRODUCT': 'Product',
    # C++ normal types
    'bool': 'Bool',
    'int': 'Int',
    'float': 'Float',
    'double': 'Double',
    'std::string': 'String',
    'cyclus::Blob': 'Blob',
    'boost::uuids::uuid': 'Uuid',
    'cyclus::Material': 'Material',
    'cyclus::Product': 'Product',
    # Template Types
    'std::set': 'Set',
    'std::map': 'Map',
    'std::pair': 'Pair',
    'std::list': 'List',
    'std::vector': 'Vector',
    'cyclus::toolkit::ResBuf': 'ResBuf',
    'cyclus::toolkit::TotalInvTracker': 'total_inv_tracker',
    'cyclus::toolkit::ResMap': 'ResMap',
    }



# note that this maps normal forms to python
VARS_TO_PY = {
    'bool': '{var}',
    'int': '{var}',
    'float': '{var}',
    'double': '{var}',
    'std::string': 'bytes({var}).decode()',
    'cyclus::Blob': 'blob_to_bytes({var})',
    'boost::uuids::uuid': 'uuid_cpp_to_py({var})',
    }

# note that this maps normal forms to python
VARS_TO_CPP = {
    'bool': '<bint> {var}',
    'int': '<int> {var}',
    'float': '<float> {var}',
    'double': '<double> {var}',
    'std::string': 'str_py_to_cpp({var})',
    'cyclus::Blob': 'cpp_cyclus.Blob(std_string(<const char*> {var}))',
    'boost::uuids::uuid': 'uuid_py_to_cpp({var})',
    }

TEMPLATE_ARGS = {
    'std::set': ('val',),
    'std::map': ('key', 'val'),
    'std::pair': ('first', 'second'),
    'std::list': ('val',),
    'std::vector': ('val',),
    'cyclus::toolkit::ResBuf': ('val',),
    'cyclus::toolkit::TotalInvTracker': ('val',),
    'cyclus::toolkit::ResMap': ('key', 'val'),
    }

NPTYPES = {
    'bool': 'np.NPY_BOOL',
    'int': 'np.NPY_INT32',
    'float': 'np.NPY_FLOAT32',
    'double': 'np.NPY_FLOAT64',
    'std::string': 'np.NPY_OBJECT',
    'cyclus::Blob': 'np.NPY_OBJECT',
    'boost::uuids::uuid': 'np.NPY_OBJECT',
    'cyclus::Material': 'np.NPY_OBJECT',
    'cyclus::Product': 'np.NPY_OBJECT',
    'std::set': 'np.NPY_OBJECT',
    'std::map': 'np.NPY_OBJECT',
    'std::pair': 'np.NPY_OBJECT',
    'std::list': 'np.NPY_OBJECT',
    'std::vector': 'np.NPY_OBJECT',
    'cyclus::toolkit::ResBuf': 'np.NPY_OBJECT',
    'cyclus::toolkit::TotalInvTracker': 'np.NPY_OBJECT',
    'cyclus::toolkit::ResMap': 'np.NPY_OBJECT',
    }

NEW_PY_INSTS = {
    'bool': 'False',
    'int': '0',
    'float': '0.0',
    'double': '0.0',
    'std::string': '""',
    'cyclus::Blob': 'b""',
    'boost::uuids::uuid': 'uuid.UUID(int=0)',
    'cyclus::Material': 'None',
    'cyclus::Product': 'None',
    'std::set': 'set()',
    'std::map': '{}',
    'std::pair': '(None, None)',
    'std::list': '[]',
    'std::vector': '[]',
    'cyclus::toolkit::ResBuf': 'None',
    'cyclus::toolkit::TotalInvTracker': 'None',
    'cyclus::toolkit::ResMap': 'None',
    }

# note that this maps normal forms to python
TO_PY_CONVERTERS = {
    # base types
    'bool': ('', '', '{var}'),
    'int': ('', '', '{var}'),
    'float': ('', '', '{var}'),
    'double': ('', '', '{var}'),
    'std::string': ('\n', '\npy{var} = {var}\npy{var} = py{var}.decode()\n',
                    'py{var}'),
    'cyclus::Blob': ('', '', 'blob_to_bytes({var})'),
    'boost::uuids::uuid': ('', '', 'uuid_cpp_to_py({var})'),
    'cyclus::Material': (
        'cdef _Material pyx_{var}',
        'pyx_{var} = Material()\n'
        'pyx_{var}.ptx = cpp_cyclus.reinterpret_pointer_cast[cpp_cyclus.Resource, '
                            'cpp_cyclus.Material]({var})\n'
        'py_{var} = pyx_{var}\n',
        'py_{var}'),
    'cyclus::Product': (
        'cdef _Product pyx_{var}',
        'pyx_{var} = Product()\n'
        'pyx_{var}.ptx = cpp_cyclus.reinterpret_pointer_cast[cpp_cyclus.Resource, '
                            'cpp_cyclus.Product]({var})\n'
        'py_{var} = pyx_{var}\n',
        'py_{var}'),
    # templates
    'std::set': (
        '{valdecl}\n'
        'cdef {valtype} {valname}\n'
        'cdef std_set[{valtype}].iterator it{var}\n'
        'cdef set py{var}\n',
        'py{var} = set()\n'
        'it{var} = {var}.begin()\n'
        'while it{var} != {var}.end():\n'
        '    {valname} = deref(it{var})\n'
        '    {valbody.indent4}\n'
        '    pyval = {valexpr}\n'
        '    py{var}.add(pyval)\n'
        '    inc(it{var})\n',
        'py{var}'),
    'std::map': (
        '{keydecl}\n'
        '{valdecl}\n'
        'cdef {keytype} {keyname}\n'
        'cdef {valtype} {valname}\n'
        'cdef {type}.iterator it{var}\n'
        'cdef dict py{var}\n',
        'py{var} = {{}}\n'
        'it{var} = {var}.begin()\n'
        'while it{var} != {var}.end():\n'
        '    {keyname} = deref(it{var}).first\n'
        '    {keybody.indent4}\n'
        '    pykey = {keyexpr}\n'
        '    {valname} = deref(it{var}).second\n'
        '    {valbody.indent4}\n'
        '    pyval = {valexpr}\n'
        '    pykey = {keyexpr}\n'
        '    py{var}[pykey] = pyval\n'
        '    inc(it{var})\n',
        'py{var}'),
    'std::pair': (
        '{firstdecl}\n'
        '{seconddecl}\n'
        'cdef {firsttype} {firstname}\n'
        'cdef {secondtype} {secondname}\n',
        '{firstname} = {var}.first\n'
        '{firstbody}\n'
        'pyfirst = {firstexpr}\n'
        '{secondname} = {var}.second\n'
        '{secondbody}\n'
        'pyfirst = {firstexpr}\n'
        'pysecond = {secondexpr}\n'
        'py{var} = (pyfirst, pysecond)\n',
        'py{var}'),
    'std::list': (
        '{valdecl}\n'
        'cdef {valtype} {valname}\n'
        'cdef std_list[{valtype}].iterator it{var}\n'
        'cdef list py{var}\n',
        'py{var} = []\n'
        'it{var} = {var}.begin()\n'
        'while it{var} != {var}.end():\n'
        '    {valname} = deref(it{var})\n'
        '    {valbody.indent4}\n'
        '    pyval = {valexpr}\n'
        '    py{var}.append(pyval)\n'
        '    inc(it{var})\n',
        'py{var}'),
    'std::vector': (
        'cdef np.npy_intp {var}_shape[1]\n',
        '{var}_shape[0] = <np.npy_intp> {var}.size()\n'
        'py{var} = np.PyArray_SimpleNewFromData(1, {var}_shape, {nptypes[0]}, '
            '&{var}[0])\n'
        'py{var} = np.PyArray_Copy(py{var})\n',
        'py{var}'),
    ('std::vector', 'bool'): (
        'cdef int i\n'
        'cdef np.npy_intp {var}_shape[1]\n',
        '{var}_shape[0] = <np.npy_intp> {var}.size()\n'
        'py{var} = np.PyArray_SimpleNew(1, {var}_shape, np.NPY_BOOL)\n'
        'for i in range({var}_shape[0]):\n'
        '    py{var}[i] = {var}[i]\n',
        'py{var}'),
    ('np.ndarray', 'np.NPY_OBJECT'): (
        'cdef int i\n'
        'cdef np.npy_intp {var}_shape[1]\n',
        '{var}_shape[0] = <np.npy_intp> {var}.size()\n'
        'py{var} = np.PyArray_SimpleNew(1, {var}_shape, np.NPY_OBJECT)\n'
        'for i in range({var}_shape[0]):\n'
        '    {var}_i = {elem_to_py}\n'
        '    py{var}[i] = {var}_i\n',
        'py{var}'),
    'cyclus::toolkit::ResBuf': ('', '', 'None'),
    'cyclus::toolkit::TotalInvTracker': ('', '', 'None'),
    'cyclus::toolkit::ResMap': ('', '', 'None'),
    }

TO_CPP_CONVERTERS = {
    # base types
    'bool': ('', '', '<bint> {var}'),
    'int': ('', '', '<int> {var}'),
    'float': ('', '', '<float> {var}'),
    'double': ('', '', '<double> {var}'),
    'std::string': ('cdef bytes b_{var}',
        'if isinstance({var}, str):\n'
        '   b_{var} = {var}.encode()\n'
        'elif isinstance({var}, str):\n'
        '   b_{var} = {var}\n'
        'else:\n'
        '   b_{var} = bytes({var})\n',
        'std_string(<const char*> b_{var})'),
    'cyclus::Blob': ('', '', 'cpp_cyclus.Blob(std_string(<const char*> {var}))'),
    'boost::uuids::uuid': ('', '', 'uuid_py_to_cpp({var})'),
    'cyclus::Material': (
        'cdef _Material py{var}\n'
        'cdef shared_ptr[cpp_cyclus.Material] cpp{var}\n',
        'py{var} = <_Material> {var}\n'
        'cpp{var} = reinterpret_pointer_cast[cpp_cyclus.Material, '
                         'cpp_cyclus.Resource](py{var}.ptx)\n',
        'cpp{var}'),
    'cyclus::Product': (
        'cdef _Material py{var}\n'
        'cdef shared_ptr[cpp_cyclus.Product] cpp{var}\n',
        'py{var} = <_Product> {var}\n'
        'cpp{var} = reinterpret_pointer_cast[cpp_cyclus.Product, '
                         'cpp_cyclus.Resource](py{var}.ptx)\n',
        'cpp{var}'),
    # templates
    'std::set': (
        '{valdecl}\n'
        'cdef std_set[{valtype}] cpp{var}\n',
        'cpp{var} = std_set[{valtype}]()\n'
        'for {valname} in {var}:\n'
        '    {valbody.indent4}\n'
        '    cpp{var}.insert({valexpr})\n',
        'cpp{var}'),
    'std::map': (
        '{keydecl}\n'
        '{valdecl}\n'
        'cdef {type} cpp{var}\n',
        'cpp{var} = {type}()\n'
        'if not isinstance({var}, collections.abc.Mapping):\n'
        '    {var} = dict({var})\n'
        'for {keyname}, {valname} in {var}.items():\n'
        '    {keybody.indent4}\n'
        '    {valbody.indent4}\n'
        '    cpp{var}[{keyexpr}] = {valexpr}\n',
        'cpp{var}'),
    'std::pair': (
        '{firstdecl}\n'
        '{seconddecl}\n'
        'cdef {type} cpp{var}\n',
        '{firstname} = {var}[0]\n'
        '{firstbody}\n'
        'cpp{var}.first = {firstexpr}\n'
        '{secondname} = {var}[1]\n'
        '{secondbody}\n'
        'cpp{var}.second = {secondexpr}\n',
        'cpp{var}'),
    'std::list': (
        '{valdecl}\n'
        'cdef std_list[{valtype}] cpp{var}\n',
        'cpp{var} = std_list[{valtype}]()\n'
        'for {valname} in {var}:\n'
        '    {valbody.indent4}\n'
        '    cpp{var}.push_back({valexpr})\n',
        'cpp{var}'),
    'std::vector': (
        'cdef int i\n'
        'cdef int {var}_size\n'
        'cdef {type} cpp{var}\n'
        'cdef {valtype} * {var}_data\n',
        'cpp{var} = {type}()\n'
        '{var}_size = len({var})\n'
        'if isinstance({var}, np.ndarray) and '
        '(<np.ndarray> {var}).descr.type_num == {nptypes[0]}:\n'
        '    {var}_data = <{valtype} *> np.PyArray_DATA(<np.ndarray> {var})\n'
        '    cpp{var}.resize(<size_t> {var}_size)\n'
        '    memcpy(<void*> &cpp{var}[0], {var}_data, sizeof({valtype}) * {var}_size)\n'
        'else:\n'
        '    cpp{var}.resize(<size_t> {var}_size)\n'
        '    for i, {valname} in enumerate({var}):\n'
        '        cpp{var}[i] = {val_to_cpp}\n',
        'cpp{var}'),
    ('np.ndarray', 'np.NPY_OBJECT'): (
        'cdef int i\n'
        'cdef int {var}_size\n'
        'cdef {type} cpp{var}\n',
        'cpp{var} = {type}()\n'
        '{var}_size = len({var})\n'
        'cpp{var}.resize(<size_t> {var}_size)\n'
        'for i, {valname} in enumerate({var}):\n'
        '    cpp{var}[i] = {val_to_cpp}\n',
        'cpp{var}'),
    'cyclus::toolkit::ResBuf': (
        'cdef _{classname} py{var}\n'
        'cdef cpp_cyclus.ResBuf[{valtype}] cpp{var}\n',
        'py{var} = <_{classname}> {var}\n'
        'cpp{var} = deref(py{var}.ptx)\n',
        'cpp{var}'),
    'cyclus::toolkit::TotalInvTracker': (
        'cdef _{classname} py{var}\n'
        'cdef cpp_cyclus.TotalInvTracker[{valtype}] cpp{var}\n',
        'py{var} = <_{classname}> {var}\n'
        'cpp{var} = deref(py{var}.ptx)\n',
        'cpp{var}'),
    'cyclus::toolkit::ResMap': (
        'cdef _{classname} py{var}\n'
        'cdef cpp_cyclus.ResMap[{keytype}, {valtype}] cpp{var}\n',
        'py{var} = <_{classname}> {var}\n'
        'cpp{var} = deref(py{var}.ptx)\n',
        'cpp{var}'),
    }

# annotation info key (pyname), C++ name,  cython type names, init snippet
ANNOTATIONS = [
    ('name', 'name', 'object', 'None'),
    ('type', 'type', 'object', 'None'),
    ('index', 'index', 'int', '-1'),
    ('default', 'dflt', 'object', 'None'),
    ('internal', 'internal', 'bint', 'False'),
    ('shape', 'shape', 'object', 'None'),
    ('doc', 'doc', 'str', '""'),
    ('tooltip', 'tooltip', 'object', 'None'),
    ('units', 'units', 'str', '""'),
    ('userlevel', 'userlevel', 'int', '0'),
    ('alias', 'alias', 'object', 'None'),
    ('uilabel', 'uilabel', 'object', 'None'),
    ('uitype', 'uitype', 'object', 'None'),
    ('range', 'range', 'object', 'None'),
    ('categorical', 'categorical', 'object', 'None'),
    ('schematype', 'schematype', 'object', 'None'),
    ('initfromcopy', 'initfromcopy', 'str', '""'),
    ('initfromdb', 'initfromdb', 'str', '""'),
    ('infiletodb', 'infiletodb', 'str', '""'),
    ('schema', 'schema', 'str', '""'),
    ('snapshot', 'snapshot', 'str', '""'),
    ('snapshotinv', 'snapshotinv', 'str', '""'),
    ('initinv', 'initinv', 'str', '""'),
    ('uniquetypeid', 'uniquetypeid', 'int', '-1'),
    ]


def split_template_args(s, open_brace='<', close_brace='>', separator=','):
    """Takes a string with template specialization and returns a list
    of the argument values as strings. Mostly cribbed from xdress.
    """
    targs = []
    ns = s.split(open_brace, 1)[-1].rsplit(close_brace, 1)[0].split(separator)
    count = 0
    targ_name = ''
    for n in ns:
        count += n.count(open_brace)
        count -= n.count(close_brace)
        if len(targ_name) > 0:
            targ_name += separator
        targ_name += n
        if count == 0:
            targs.append(targ_name.strip())
            targ_name = ''
    return targs


def parse_template(s, open_brace='<', close_brace='>', separator=','):
    """Takes a string -- which may represent a template specialization --
    and returns the corresponding type. Mostly cribbed from xdress.
    """
    if open_brace not in s and close_brace not in s:
        return s
    t = [s.split(open_brace, 1)[0]]
    targs = split_template_args(s, open_brace=open_brace,
                                close_brace=close_brace, separator=separator)
    for targ in targs:
        t.append(parse_template(targ, open_brace=open_brace,
                                close_brace=close_brace, separator=separator))
    t = tuple(t)
    return t


class Indenter(object):
    """Handles indentations."""
    def __init__(self, s):
        """Constructor for string object."""
        self._s = s

    def __str__(self):
        """Returns a string."""
        return self._s

    def __getattr__(self, key):
        """Replaces an indentation with a newline and spaces."""
        if key.startswith('indent'):
            n = int(key[6:])
            return self._s.replace('\n', '\n' + ' '*n)
        return self.__dict__[key]

def safe_output(cmd, shell=False, *args, **kwargs):
    """Checks that a command successfully runs with/without shell=True.
    Returns the output.
    """
    try:
        out = subprocess.check_output(cmd, shell=False, *args, **kwargs)
    except (subprocess.CalledProcessError, OSError):
        cmd = ' '.join(cmd)
        out = subprocess.check_output(cmd, shell=True, *args, **kwargs)
    return out

#
# Code Generation
#

JENV = jinja2.Environment(undefined=jinja2.StrictUndefined)

CG_WARNING = """
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# !!!!! WARNING - THIS FILE HAS BEEN !!!!!!
# !!!!!   AUTOGENERATED BY CYCLUS    !!!!!!
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
""".strip()

STL_CIMPORTS = """
# Cython standard library imports
from libcpp.map cimport map as std_map
from libcpp.set cimport set as std_set
from libcpp.list cimport list as std_list
from libcpp.vector cimport vector as std_vector
from libcpp.utility cimport pair as std_pair
from libcpp.string cimport string as std_string
from libcpp.typeinfo cimport type_info
from cython.operator cimport dereference as deref
from cython.operator cimport preincrement as inc
from cython.operator cimport typeid
from libc.stdlib cimport malloc, free
from libc.string cimport memcpy
from libcpp cimport bool as cpp_bool
from libcpp.cast cimport const_cast
from libcpp.cast cimport reinterpret_cast, dynamic_cast
""".strip()

NPY_IMPORTS = """
# numpy imports & startup
cimport numpy as np
import numpy as np
np.import_array()
np.import_ufunc()
""".strip()

CPP_TYPESYSTEM = JENV.from_string("""
{{ cg_warning }}

cdef extern from "cyclus.h" namespace "cyclus":

    cdef enum DbTypes:
        {{ dbtypes | join('\n') | indent(8) }}

""".strip())

def cpp_typesystem(ts, ns):
    """Creates the Cython header that wraps the Cyclus type system."""
    temp = '{0} = {1}'
    dbtypes = [temp.format(t, ts.ids[t]) for t in ts.dbtypes]
    ctx = dict(
        dbtypes=dbtypes,
        cg_warning=CG_WARNING,
        stl_cimports=STL_CIMPORTS,
        )
    rtn = CPP_TYPESYSTEM.render(ctx)
    return rtn


TYPESYSTEM_PYX = JENV.from_string('''
{{ cg_warning }}

{{ stl_cimports }}

{{ npy_imports }}

from cpython.pycapsule cimport PyCapsule_New, PyCapsule_GetPointer

# local imports
from cyclus cimport cpp_typesystem
from cyclus cimport cpp_cyclus
from cyclus.cpp_cyclus cimport shared_ptr, reinterpret_pointer_cast
from cyclus cimport lib


# pure python imports
import uuid
import collections
from binascii import hexlify

from cyclus import nucname
#
# Resources & Inventories
#

cdef class _Resource:

    @property
    def obj_id(self):
        """The unique id corresponding to this resource object. Can be used
        to track and/or associate other information with this resource object.
        You should NOT track resources by pointer.
        """
        return self.ptx.get().obj_id()

    @property
    def state_id(self):
        """The unique id corresponding to this resource and its current
        state.  All resource id's are unique - even across different resource
        types/implementations. Runtime tracking of resources should generally
        use the obj_id rather than this.
        """
        return self.ptx.get().state_id()

    def bump_state_id(self):
        """Assigns a new, unique internal id to this resource and its state. This
        should be called by resource implementations whenever their state changes.
        A call to bump_state_id is not necessarily accompanied by a change to the
        state id. This should NEVER be called by agents.
        """
        self.ptx.get().BumpStateId()

    @property
    def qual_id(self):
        """Returns an id representing the specific resource implementation's internal
        state that is not accessible via the Resource class public interface.  Any
        change to the qual_id should always be accompanied by a call to
        bump_state_id().
        """
        return self.ptx.get().qual_id()

    @property
    def type(self):
        """A unique type/name for the concrete resource implementation."""
        cdef std_string cpp_t = self.ptx.get().type()
        t = std_string_to_py(cpp_t)
        return t

    def clone(self):
        """Returns an untracked (not part of the simulation) copy of the resource.
        A cloned resource should never record anything in the output database.
        """
        cdef _Resource co = Resource()
        co.ptx = self.ptx.get().Clone()
        copy = co
        return copy

    def record(self, lib._Context ctx):
        """Records the resource's state to the output database.  This method
        should generally NOT record data accessible via the Resource class
        public methods (e.g.  qual_id, units, type, quantity).
        """
        self.ptx.get().Record(ctx.ptx)

    @property
    def units(self):
        """Returns the units this resource is based in (e.g. "kg")."""
        cdef std_string cpp_u = self.ptx.get().units()
        u = std_string_to_py(cpp_u)
        return u

    @property
    def quantity(self):
        """Returns the quantity of this resource with dimensions as specified by
        the return value of units().
        """
        return self.ptx.get().quantity()

    def extract_res(self, double quantity):
        """Splits the resource and returns the extracted portion as a new resource
        object.  Allows for things like ResBuf and Traders to split
        offers/requests of arbitrary resource implementation type.
        """
        cdef _Resource res = Resource()
        res.ptx = self.ptx.get().ExtractRes(quantity)
        respy = res
        return respy


class Resource(_Resource):
    """Resource defines an abstract interface implemented by types that are
    offered, requested, and transferred between simulation agents. Resources
    represent the lifeblood of a simulation.
    """


cdef shared_ptr[cpp_cyclus.Composition] composition_ptr_from_py(object comp,
                                                                object basis):
    """Converts a dict-like to a composition."""
    if not isinstance(comp, dict):
        comp = dict(comp)
    cdef int k
    cdef double v
    cdef cpp_cyclus.CompMap c
    for key, val in comp.items():
        k = nucname.id(key)
        v = val
        c[k] = v
    cdef shared_ptr[cpp_cyclus.Composition] p
    if basis == 'mass':
        p = cpp_cyclus.Composition.CreateFromMass(c)
    elif basis == 'atom':
        p = cpp_cyclus.Composition.CreateFromAtom(c)
    else:
        raise ValueError('Composition basis must be either mass or atom, '
                         'not ' + str(basis))
    return p


cdef object composition_from_cpp(shared_ptr[cpp_cyclus.Composition] comp, object basis):
    """Converts a composition to a dict."""
    cdef cpp_cyclus.CompMap c
    if basis == 'mass':
        c = deref(comp).mass()
    elif basis == 'atom':
        c = deref(comp).atom()
    else:
        raise ValueError('Composition basis must be either mass or atom, '
                         'not ' + str(basis))
    rtn = {}
    for item in c:
        rtn[item.first] = item.second
    return rtn


cdef class _Material(_Resource):

    @staticmethod
    def create(lib._Agent creator, double quantity, c, basis='mass'):
        """Creates a new material resource that is "live" and tracked. creator is a
        pointer to the agent creating the resource (usually will be the caller's
        "this" pointer). All future output data recorded will be done using the
        creator's context.
        """
        cdef shared_ptr[cpp_cyclus.Composition] comp = composition_ptr_from_py(c, basis)
        cdef _Material mat = Material()
        mat.ptx = cpp_cyclus.reinterpret_pointer_cast[cpp_cyclus.Resource,
                                                      cpp_cyclus.Material](
                    cpp_cyclus.Material.Create(lib.dynamic_agent_ptr(creator),
                                               quantity, comp))
        rtn = mat
        return rtn

    @staticmethod
    def create_untracked(double quantity, c, basis='mass'):
        """Creates a new material resource that does not actually exist as part of
        the simulation and is untracked.
        """
        cdef shared_ptr[cpp_cyclus.Composition] comp = composition_ptr_from_py(c, basis)
        cdef _Material mat = Material()
        mat.ptx = reinterpret_pointer_cast[cpp_cyclus.Resource, cpp_cyclus.Material](
                    cpp_cyclus.Material.CreateUntracked(quantity, comp))
        rtn = mat
        return rtn

    def clone(self):
        """Returns an untracked (not part of the simulation) copy of the material.
        """
        cdef _Material co = Material()
        co.ptx = self.ptx.get().Clone()
        copy = co
        return copy

    def extract_qty(self, double quantity):
        """Same as ExtractComp with c = this->comp() and returns a Material,
        not a Resource.
        """
        cdef _Material res = Material()
        res.ptx = reinterpret_pointer_cast[cpp_cyclus.Resource, cpp_cyclus.Material](
                  reinterpret_pointer_cast[cpp_cyclus.Material, cpp_cyclus.Resource](
                    self.ptx).get().ExtractQty(quantity))
        respy = res
        return respy

    def extract_comp(self, double qty, c, basis='mass', threshold=None):
        """Creates a new material by extracting from this one. """
        cdef shared_ptr[cpp_cyclus.Composition] comp = composition_ptr_from_py(c, basis)
        cdef double t
        t = cpp_cyclus.eps_rsrc() if threshold is None else threshold
        cdef _Material res = Material()
        res.ptx = reinterpret_pointer_cast[cpp_cyclus.Resource, cpp_cyclus.Material](
                  reinterpret_pointer_cast[cpp_cyclus.Material, cpp_cyclus.Resource](
                    self.ptx).get().ExtractComp(qty, comp, t))
        respy = res
        return respy

    def absorb(self, _Material mat):
        """Combines material mat with this one.  mat's quantity becomes zero."""
        cdef shared_ptr[cpp_cyclus.Material] p = \
            reinterpret_pointer_cast[cpp_cyclus.Material, cpp_cyclus.Resource](
                mat.ptx)
        reinterpret_pointer_cast[cpp_cyclus.Material, cpp_cyclus.Resource](
            self.ptx).get().Absorb(p)

    def transmute(self, c, basis='mass'):
        """Changes the material's composition to c without changing its mass.  Use
        this method for things like converting fresh to spent fuel via burning in
        a reactor.
        """
        cdef shared_ptr[cpp_cyclus.Composition] comp = composition_ptr_from_py(c, basis)
        reinterpret_pointer_cast[cpp_cyclus.Material, cpp_cyclus.Resource](
            self.ptx).get().Transmute(comp)

    def decay(self, int curr_time):
        """Updates the material's composition by performing a decay calculation.
        This is a special case of Transmute where the new composition is
        calculated automatically.  The time delta is calculated as the difference
        between curr_time and the last time the material's composition was
        updated with a decay calculation (i.e. prev_decay_time).  This may or may
        not result in an updated material composition.  Does nothing if the
        simulation decay mode is set to "never" or none of the nuclides' decay
        constants are significant with respect to the time delta.
        """
        reinterpret_pointer_cast[cpp_cyclus.Material, cpp_cyclus.Resource](
            self.ptx).get().Decay(curr_time)

    @property
    def prev_decay_time(self):
        """The last time step on which a decay calculation was performed
        for the material.  This is not necessarily synonymous with the last time
        step the material's Decay function was called.
        """
        return reinterpret_pointer_cast[cpp_cyclus.Material, cpp_cyclus.Resource](
            self.ptx).get().prev_decay_time()

    def decay_heat(self):
        """Returns a double with the decay heat of the material in units of W/kg."""
        return reinterpret_pointer_cast[cpp_cyclus.Material, cpp_cyclus.Resource](
            self.ptx).get().DecayHeat()

    def comp(self, basis='mass'):
        """Returns the nuclide composition of this material."""
        rtn = composition_from_cpp(
                reinterpret_pointer_cast[cpp_cyclus.Material, cpp_cyclus.Resource](
                    self.ptx).get().comp(), basis)
        return rtn


class Material(_Material, Resource):
    """The material class is primarily responsible for enabling basic material
    manipulation while helping enforce mass conservation.  It also provides the
    ability to easily decay a material up to the current simulation time; it
    does not perform any decay related logic itself.
    """


cdef class _Product(_Resource):

    @staticmethod
    def create(lib._Agent creator, double quantity, quality):
        """Creates a new product resource that is "live" and tracked. creator is a
        pointer to the agent creating the resource (usually will be the caller's
        "this" pointer). All future output data recorded will be done using the
        creator's context.
        """
        cdef _Product prod = Product()
        prod.ptx = reinterpret_pointer_cast[cpp_cyclus.Resource, cpp_cyclus.Product](
                    cpp_cyclus.Product.Create(lib.dynamic_agent_ptr(creator),
                                              quantity, str_py_to_cpp(quality)))
        rtn = prod
        return rtn

    @staticmethod
    def create_untracked(double quantity, quality):
        """Creates a new product that does not actually exist as part of
        the simulation and is untracked.
        """
        cdef _Product prod = Product()
        prod.ptx = reinterpret_pointer_cast[cpp_cyclus.Resource, cpp_cyclus.Product](
                    cpp_cyclus.Product.CreateUntracked(quantity,
                                                       str_py_to_cpp(quality)))
        rtn = prod
        return rtn

    def clone(self):
        """Returns an untracked (not part of the simulation) copy of the product.
        """
        cdef _Product co = Product()
        co.ptx = self.ptx.get().Clone()
        copy = co
        return copy

    def extract(self, double qty):
        """Extracts the specified mass from this resource and returns it as a
        new product object with the same quality/type.
        """
        cdef _Product res = Product()
        res.ptx = reinterpret_pointer_cast[cpp_cyclus.Product, cpp_cyclus.Resource](
                    self.ptx).get().ExtractRes(qty)
        respy = res
        return respy

    def absorb(self, _Product other):
        """Absorbs the contents of the given 'other' resource into this resource."""
        cdef shared_ptr[cpp_cyclus.Product] p = \
            reinterpret_pointer_cast[cpp_cyclus.Product, cpp_cyclus.Resource](
                other.ptx)
        reinterpret_pointer_cast[cpp_cyclus.Product, cpp_cyclus.Resource](
            self.ptx).get().Absorb(p)


class Product(_Product, Resource):
    """A Product is a general type of resource in the Cyclus simulation,
    and is a catch-all for non-standard resources.  It implements the Resource
    class interface in a simple way usable for things such as: bananas,
    water, buying power, etc.
    """


#
# Inventories block
#
{% for t in ts.inventory_types %}{% set tclassname = ts.classname(t) %}
cdef class _{{tclassname}}:

    def __cinit__(self, init=False):
        self._free = init
        if init:
            self.ptx = new {{ts.cython_type(t)}}()
        else:
            self.ptx = NULL

    def __dealloc__(self):
        if self.ptx == NULL or not self._free:
            return
        del self.ptx

    @property
    def quantity(self):
        """The total resource quantity of constituent resource objects in the store."""
        return self.ptx.quantity()

    def empty(self):
        """Returns true if nothing is stored."""
        return self.ptx.empty()

{% if ts.norms[t][0] == 'cyclus::toolkit::ResBuf' %}
{% set r = ts.norms[t][1] %}
{% set rcname = ts.classname(r) %}
    @property
    def capacity(self):
        """The maximum resource quantity this store can hold."""
        return self.ptx.capacity()

    @capacity.setter
    def capacity(self, double val):
        self.ptx.capacity(val)

    @property
    def count(self):
        """the total number of constituent resource objects in the store."""
        return self.ptx.count()

    def __len__(self):
        return self.ptx.count()

    @property
    def space(self):
        """The quantity of space remaining in this store."""
        return self.ptx.space()

    def pop(self, double qty=-1.0, double eps=-1.0):
        """Pops one {{rcname}} object from the store. Neagtive values of qty and eps are
        ignored.
        """
        cdef _{{rcname}} r = {{rcname}}()
        if qty < 0.0:
            r.ptx = reinterpret_pointer_cast[cpp_cyclus.Resource, {{ts.cython_type(r)}}](
                        self.ptx.Pop())
        elif eps < 0.0:
            r.ptx = reinterpret_pointer_cast[cpp_cyclus.Resource, {{ts.cython_type(r)}}](
                        self.ptx.Pop(qty))
        else:
            r.ptx = reinterpret_pointer_cast[cpp_cyclus.Resource, {{ts.cython_type(r)}}](
                        self.ptx.Pop(qty, eps))
        rtn = r
        return rtn

    def pop_n(self, int n):
        """Pops the specified number of {{rcname}}s from the buffer."""
        cdef _{{rcname}} x
        cdef list v = []
        cdef std_vector[shared_ptr[{{ts.cython_type(r)}}]] rs = self.ptx.PopN(n)
        for r in rs:
            x = {{rcname}}()
            x.ptx = reinterpret_pointer_cast[cpp_cyclus.Resource, {{ts.cython_type(r)}}](
                    r)
            v.append(x)
        rtn = v
        return rtn

    def pop_n_res(self, int n):
        """Pops the specified number of resources from the buffer."""
        cdef _Resource x
        cdef list v = []
        cdef cpp_cyclus.ResVec rs = self.ptx.PopNRes(n)
        for r in rs:
            x = Resource()
            x.ptx = r
            v.append(x)
        rtn = v
        return rtn

    def peek(self):
        """Returns the next resource in line to be popped from the buffer
        without actually removing it from the buffer.
        """
        cdef _{{rcname}} r = {{rcname}}()
        r.ptx = reinterpret_pointer_cast[cpp_cyclus.Resource, {{ts.cython_type(r)}}](
                    self.ptx.Peek())
        rtn = r
        return rtn

    def pop_back(self):
        """Same as Pop, except it returns the most recently added resource."""
        cdef _{{rcname}} r = {{rcname}}()
        r.ptx = reinterpret_pointer_cast[cpp_cyclus.Resource, {{ts.cython_type(r)}}](
                    self.ptx.PopBack())
        rtn = r
        return rtn

    def push(self, _{{rcname}} r):
        """Pushes a single resource object to the buffer."""
        self.ptx.Push(r.ptx)

    def pop_all_res(self):
        """A consistent interface for popping all of the resources from the buffer.
        For ResBuf, this is equivalent to self.pop_n_res(len(self))
        """
        rtn = self.pop_n_res(self.ptx.count())
        return rtn

    def push_many(self, rs):
        """A consistent interface for pushing many resources into the buffer.
        For ResBuf, this iteratively calls push().
        """
        for r in rs:
            self.push(r)

{% elif ts.norms[t][0] == 'cyclus::toolkit::ResMap' %}
{% set k = ts.norms[t][1] %}
{% set r = ts.norms[t][2] %}
{% set rcname = ts.classname(r) %}
{% set rcytype = ts.cython_type(r) %}

    @property
    def size(self):
        """The total number of resources in the map."""
        return self.ptx.size()

    def __len__(self):
        return self.ptx.size()

    def __iter__(self):
        cdef _{{rcname}} r
        for kr in deref(self.ptx):
            ktn = {{ ts.funcname(k) }}_to_py(kr.first)
            r = {{rcname}}()
            r.ptx = reinterpret_pointer_cast[cpp_cyclus.Resource, {{ts.cython_type(r)}}](
                        kr.second)
            rtn = r
            yield (ktn, rtn)

    def __getitem__(self, key):
        cdef {{ ts.cython_type(k) }} k = {{ ts.funcname(k) }}_to_cpp(key)
        cdef shared_ptr[{{rcytype}}] p = deref(self.ptx)[k]
        cdef _{{rcname}} r = {{rcname}}()
        r.ptx = reinterpret_pointer_cast[cpp_cyclus.Resource, {{ts.cython_type(r)}}](
                    p)
        rtn = r
        return rtn

    def __setitem__(self, key, _{{rcname}} value):
        cdef {{ ts.cython_type(k) }} k = {{ ts.funcname(k) }}_to_cpp(key)
        cdef shared_ptr[{{rcytype}}] p = \
            reinterpret_pointer_cast[{{ts.cython_type(r)}}, cpp_cyclus.Resource](
                (value.ptx))
        deref(self.ptx)[k] = p

    def __delitem__(self, key):
        cdef {{ ts.cython_type(k) }} k = {{ ts.funcname(k) }}_to_cpp(key)
        self.ptx.erase(k)

    def clear(self):
        """Removes all elements from the map."""
        self.ptx.clear()

    def values(self):
        """Returns a vector of the values in the map"""
        cdef _{{rcname}} x
        cdef list v = []
        cdef std_vector[shared_ptr[{{ts.cython_type(r)}}]] rs = self.ptx.Values()
        for r in rs:
            x = {{rcname}}()
            x.ptx = \
                reinterpret_pointer_cast[cpp_cyclus.Resource, {{ts.cython_type(r)}}](
                    r)
            v.append(x)
        rtn = v
        return rtn

    def res_values(self):
        """Pops the resources from the buffer.
        For the version of this method which sets resources, see push_many().
        """
        cdef _Resource x
        cdef list v = []
        cdef std_vector[shared_ptr[cpp_cyclus.Resource]] rs = self.ptx.ResValues()
        for r in rs:
            x = Resource()
            x.ptx = r
            v.append(x)
        rtn = v
        return rtn

    def pop(self, key):
        """Pops one {{rcname}} object from the store."""
        cdef {{ ts.cython_type(k) }} k = {{ ts.funcname(k) }}_to_cpp(key)
        cdef _{{rcname}} r = {{rcname}}()
        r.ptx = \
            reinterpret_pointer_cast[cpp_cyclus.Resource, {{ts.cython_type(r)}}](
                self.ptx.Pop(k))
        rtn = r
        return rtn

    def pop_all_res(self):
        """A consistent interface for popping all of the resources from the buffer.
        For ResBuf, this is equivalent to self.res_values()
        """
        return self.res_values()

    def push_many(self, rs):
        """A consistent interface for pushing many resources into the buffer.
        For ResMap, this calls ResValues(vals).
        """
        cdef std_vector[shared_ptr[cpp_cyclus.Resource]] cpp_rs = \
            std_vector[shared_ptr[cpp_cyclus.Resource]]()
        for r in rs:
            cpp_rs.push_back((<_Resource> r).ptx)
        self.ptx.ResValues(cpp_rs)

{% endif %}


class {{tclassname}}(_{{tclassname}}):
    """An Inventory wrapper class for {{ts.cpptypes[t]}}."""

{% endfor %}


#
# raw type definitions
#
{% for t in dbtypes %}
{{ t }} = {{ ts.cython_cpp_name(t) }}
{%- endfor %}

cdef dict C_RANKS = {
{%- for t in dbtypes %}
    {{ ts.cython_cpp_name(t) }}: {{ ts.ranks[t] }},
{%- endfor %}
    }
RANKS = C_RANKS

cdef dict C_NAMES = {
{%- for t in dbtypes %}
    {{ ts.cython_cpp_name(t) }}: '{{ t }}',
{%- endfor %}
    }
NAMES = C_NAMES

cdef dict C_IDS = {
{%- for t in dbtypes %}
    '{{ t }}': {{ ts.cython_cpp_name(t) }},
{%- endfor %}
{%- for t in ts.uniquetypes %}
    {{ repr(ts.norms[t]) }}: {{ ts.cython_cpp_name(t) }},
{%- endfor %}
    }
IDS = C_IDS

cdef dict C_CPPTYPES = {
{%- for t in dbtypes %}
    {{ ts.cython_cpp_name(t) }}: '{{ ts.cpptypes[t] }}',
{%- endfor %}
    }
CPPTYPES = C_CPPTYPES

cdef dict C_NORMS = {
{%- for t in dbtypes %}
    {{ ts.cython_cpp_name(t) }}: {{ repr(ts.norms[t]) }},
{%- endfor %}
    }
NORMS = C_NORMS

#
# converters
#
cdef bytes blob_to_bytes(cpp_cyclus.Blob value):
    rtn = value.str()
    return bytes(rtn)


cdef object uuid_cpp_to_py(cpp_cyclus.uuid x):
    cdef int i
    cdef list d = []
    for i in range(16):
        d.append(<unsigned int> x.data[i])
    rtn = uuid.UUID(hex=hexlify(bytearray(d)).decode())
    return rtn


cdef cpp_cyclus.uuid uuid_py_to_cpp(object x):
    cdef char * c
    cdef cpp_cyclus.uuid u
    if isinstance(x, uuid.UUID):
        c = x.bytes
    else:
        c = x
    memcpy(u.data, c, 16)
    return u

cdef std_string str_py_to_cpp(object x):
    if isinstance(x,list):
        x = str(x[0])
    cdef std_string s
    x = x.encode()
    s = std_string(<const char*> x)
    return s


{% for n in sorted(set(ts.norms.values()), key=ts.funcname) %}
{% set decl, body, expr = ts.convert_to_py('x', n) %}
cdef object {{ ts.funcname(n) }}_to_py({{ ts.possibly_shared_cython_type(n) }} x):
    {{ decl | indent(4) }}
    {{ body | indent(4) }}
    return {{ expr }}
{%- endfor %}

{% for n in sorted(set(ts.norms.values()), key=ts.funcname) %}
{% set decl, body, expr = ts.convert_to_py('x', n) %}
cdef object any_{{ ts.funcname(n) }}_to_py(cpp_cyclus.hold_any value):
    cdef {{ ts.possibly_shared_cython_type(n) }} x = value.cast[{{ ts.possibly_shared_cython_type(n) }}]()
    {{ decl | indent(4) }}
    {{ body | indent(4) }}
    return {{ expr }}
{%- endfor %}

{% for n in sorted(set(ts.norms.values()), key=ts.funcname) %}
{% set decl, body, expr = ts.convert_to_cpp('x', n) %}
cdef {{ ts.possibly_shared_cython_type(n) }} {{ ts.funcname(n) }}_to_cpp(object x):
    {{ decl | indent(4) }}
    {{ body | indent(4) }}
    return {{ expr }}
{%- endfor %}


#
# type system functions
#
cdef object db_to_py(cpp_cyclus.hold_any& value, cpp_cyclus.DbTypes dbtype):
    """Converts database types to python objects."""
    cdef object rtn
    {%- for i, t in enumerate(dbtypes) %}
    {% if i > 0 %}el{% endif %}if dbtype == {{ ts.cython_cpp_name(t) }}:
        #rtn = {{ ts.hold_any_to_py('value', t) }}
        rtn = any_{{ ts.funcname(t) }}_to_py(value)
    {%- endfor %}
    else:
        msg = "dbtype {0} could not be found while converting to Python"
        raise TypeError(msg.format(dbtype))
    return rtn


cdef cpp_cyclus.hold_any py_to_any(object value, object t):
    """Converts a Python object into int a hold_any instance by inspecting the
    type.

    Parameters
    ----------
    value : object
        A Python object to encapsulate.
    t : dbtype or norm type (str or tupe of str)
        The type to use in the conversion.
    """
    if isinstance(t, int):
        return py_to_any_by_dbtype(value, t)
    else:
        return py_to_any_by_norm(value, t)


cdef cpp_cyclus.hold_any py_to_any_by_dbtype(object value, cpp_cyclus.DbTypes dbtype):
    """Converts Python object to a hold_any instance by knowing the dbtype."""
    cdef cpp_cyclus.hold_any rtn
    {%- for i, t in enumerate(dbtypes) %}
    {% if i > 0 %}el{% endif %}if dbtype == {{ ts.cython_cpp_name(t) }}:
        rtn = {{ ts.py_to_any('rtn', 'value', t) }}
    {%- endfor %}
    else:
        msg = "dbtype {0} could not be found while converting from Python"
        raise TypeError(msg.format(dbtype))
    return rtn


cdef cpp_cyclus.hold_any py_to_any_by_norm(object value, object norm):
    """Converts Python object to a hold_any instance by knowing the dbtype."""
    cdef cpp_cyclus.hold_any rtn
    if isinstance(norm, str):
        {%- for i, t in enumerate(uniquestrtypes) %}
        {% if i > 0 %}el{% endif %}if norm == {{ repr(ts.norms[t]) }}:
            rtn = {{ ts.py_to_any('rtn', 'value', t) }}
        {%- endfor %}
        else:
            msg = "norm type {0} could not be found while converting from Python"
            raise TypeError(msg.format(norm))
    else:
        norm0 = norm[0]
        normrest = norm[1:]
        {% for i, (key, group) in enumerate(groupby(uniquetuptypes, key=firstfirst)) %}
        {% if i > 0 %}el{% endif %}if norm0 == {{ repr(key) }}:
            {%- for n, (tnorm, t) in enumerate(group) %}
            {% if n > 0 %}el{% endif %}if normrest == {{ repr(tnorm[1:]) }}:
                rtn = {{ ts.py_to_any('rtn', 'value', t) }}
            {%- endfor %}
            else:
                msg = "norm type {0} could not be found while converting from Python"
                raise TypeError(msg.format(norm))
        {% endfor %}
        else:
            msg = "norm type {0} could not be found while converting from Python"
            raise TypeError(msg.format(norm))
    return rtn


cdef object any_to_py(cpp_cyclus.hold_any value):
    """Converts any C++ object to its Python equivalent."""
    cdef object rtn = None
    cdef size_t valhash = value.type().hash_code()
    # Note that we need to use the *_t tyedefs here because of
    # Bug #1561 in Cython
    {%- for i, t in enumerate(ts.uniquetypes) %}
    {% if i > 0 %}el{% endif %}if valhash == typeid({{ ts.funcname(t) }}_t).hash_code():
        rtn = any_{{ ts.funcname(t) }}_to_py(value)
    {%- endfor %}
    else:
        msg = "C++ type could not be found while converting to Python"
        raise TypeError(msg)
    return rtn

def capsule_any_to_py(value):
    """Converts a PyCapsule that holds a boost::spirit::hold_any to a python
    value.
    """
    cdef cpp_cyclus.hold_any* cpp_value = <cpp_cyclus.hold_any*>PyCapsule_GetPointer(value, <char*> b"value")
    py_value = any_to_py(deref(cpp_value))
    return py_value

cdef object new_py_inst(cpp_cyclus.DbTypes dbtype):
    """Creates a new, empty Python instance of a database type."""
    cdef object rtn
    {%- for i, t in enumerate(dbtypes) %}
    {% if i > 0 %}el{% endif %}if dbtype == {{ ts.cython_cpp_name(t) }}:
        rtn = {{ ts.new_py_inst(t) }}
    {%- endfor %}
    else:
        msg = "dbtype {0} could not be found while making a new Python instance"
        raise TypeError(msg.format(dbtype))
    return rtn

#
# State Variable Descriptors
#

cdef int _N_STATEVARS = 0

cdef class StateVar:
    """This class represents a state variable on a Cyclus agent.

    ============ ==============================================================
    key          meaning
    ============ ==============================================================
    type         The C++ type.  Valid types may be found on the :doc:`dbtypes`
                 page. **READ ONLY:** Do not set this key in
                 ``#pragma cyclus var {...}`` as it is set automatically by
                 cycpp. Feel free to use this downstream in your class or in a
                 post-process.
    index        Which number state variable is this, 0-indexed.
                 **READ ONLY:** Do not set this key in
                 ``#pragma cyclus var {...}`` as it is set automatically by
                 cycpp. Feel free to use this downstream in your class or in a
                 post-process.
    default      The default value for this variable that is used if otherwise
                 unspecified. The value must match the type of the variable.
    internal     ``True`` if this state variable is only for
                 archetype-internal usage.  Although the variable will still
                 be persisted in the database and initialized normally (e.g.
                 with any default), it will not be included in the XML schema
                 or input file.
    shape        The shape of a variable length datatypes. If present this must
                 be a list of integers whose length (rank) makes sense for this
                 type. Specifying positive values will (depending on the
                 backend) turn a variable length type into a fixed length one
                 with the length of the given value. Putting a ``-1`` in the
                 shape will retain the variable length nature along that axis.
                 Fixed length variables are normally more performant so it is
                 often a good idea to specify the shape where possible. For
                 example, a length-5 string would have a shape of ``[5]`` and
                 a length-10 vector of variable length strings would have a
                 shape of ``[10, -1]``.
    doc          Documentation string.
    tooltip      Brief documentation string for user interfaces.
    units        The physical units, if any.
    userlevel    Integer from 0 - 10 for representing ease (0) or difficulty (10)
                 in using this variable, default 0.
    alias        The name of the state variable in the schema and input file.
                 If this is not present it defaults to the C++ variable name.
                 The alias may also be a nested list of strings that matches
                 the C++ template type. Each member of the hierarchy will
                 recieve the corresponding alias.  For example, a
                 ``[std::map, int, double]`` could be aliased by
                 ``['recipe', 'id', 'mass']``. For maps, an additional item
                 tag is inserted. To also alias the item tag, make the top
                 alias into a 2-element list, whose first string represents
                 the map and whose second string is the item alias, e.g.
                 ``[['recipe', 'entry'], 'id', 'mass']``
    uilabel      The text string a UI will display as the name of this input on
                 the UI input form.
    uitype       The type of the input field in reference in a UI,
                 currently supported types are; incommodity, outcommodity,
                 commodity, range, combobox, facility, prototype, recipe, nuclide,
                 package, and none.
                 For 'nuclide' when the type is an int, the values will be read in
                 from the input file in human readable string format ('U-235') and
                 automatically converted to results of ``pyne::nucname::id()``
                 (922350000) in the database and on the archetype.
    range        This indicates the range associated with a range type.
                 It must take the form of ``[min, max]`` or
                 ``[min, max, (optional) step size]``.
    categorical  This indicates the decrete values a combobox Type can take. It
                 must take the form of ``[value1, value2, value3, etc]``.
    schematype   This is the data type that is used in the schema for input file
                 validation. This enables you to supply just the data type
                 rather than having to overwrite the full schema for this state
                 variable. In most cases - when the shape is rank 0 or 1 such
                 as for scalars or vectors - this is simply a string. In cases
                 where the rank is 2+ this is a list of strings. Please refer to
                 the `XML Schema Datatypes <http://www.w3.org/TR/xmlschema-2/>`_
                 page for more information. *New in v1.1.*
    initfromcopy Code snippet to use in the ``InitFrom(Agent* m)`` function for
                 this state variable instead of using code generation.
                 This is a string.
    initfromdb   Code snippet to use in the ``InitFrom(QueryableBackend* b)``
                 function for this state variable instead of using code generation.
                 This is a string.
    infiletodb   Code snippets to use in the ``InfileToDb()`` function
                 for this state variable instead of using code generation.
                 This is a dictionary of string values with the two keys 'read'
                 and 'write' which represent reading values from the input file
                 writing them out to the database respectively.
    schema       Code snippet to use in the ``schema()`` function for
                 this state variable instead of using code generation.
                 This is an RNG string. If you supply this then you likely need
                 to supply ``infiletodb`` as well to ensure that your custom
                 schema is read into the database correctly.
    snapshot     Code snippet to use in the ``Snapshot()`` function for
                 this state variable instead of using code generation.
                 This is a string.
    snapshotinv  Code snippet to use in the ``SnapshotInv()`` function for
                 this state variable instead of using code generation.
                 This is a string.
    initinv      Code snippet to use in the ``InitInv()`` function for
                 this state variable instead of using code generation.
                 This is a string.
    uniquetypeid The dbtype id for the type that is unique among all dbtypes
                 for a given C++ representations. **READ ONLY:** Do not set this
                 key!!!
    ============ ==============================================================
    """


    def __cinit__(self, object value=None,
            {%- for pyname, cppname, typename, kwval in annotations -%}
            {{typename}} {{pyname}}={{kwval}},
            {%- endfor -%}):
        global _N_STATEVARS
        self.value = value
        {% for pyname, cppname, _, _ in annotations -%}
        self.{{pyname}} = {{pyname}}
        {% endfor %}
        if index < 0:
            self.index = _N_STATEVARS
            _N_STATEVARS += 1

    #
    # Descriptor interface
    #
    def __get__(self, obj, cls):
        if obj is None:
            return self
        return obj.__dict__[self.name].value

    def __set__(self, obj, val):
        if obj is None:
            self.value = val
        else:
            obj.__dict__[self.name].value = val

    cpdef dict to_dict(self):
        """Returns a representation of this state variable as a dict."""
        return {'value': self.value,
            {%- for pyname, cppname, _, _ in annotations -%}
            '{{pyname}}': self.{{pyname}},
            {%- endfor -%}
            }

    cpdef StateVar copy(self):
        """Copies the state variable into a new instance."""
        return StateVar(value=self.value,
            {%- for pyname, cppname, _, _ in annotations -%}
            {{pyname}}=self.{{pyname}},
            {%- endfor -%}
            )

    def __str__(self):
        s = self.__class__.__name__ + "(" + (
            "value=" + str(self.value) + ", " +
            {%- for pyname, _, _, _ in annotations -%}
            "{{pyname}}=" + str(self.{{pyname}}) + ", " +
            {%- endfor -%}
            ")")
        return s

    def __repr__(self):
        s = "cyclus.typesystem." + self.__class__.__name__ + "(" + (
            "value=" + repr(self.value) + ", " +
            {%- for pyname, _, _, _ in annotations -%}
            "{{pyname}}=" + repr(self.{{pyname}}) + ", " +
            {%- endfor -%}
            ")")
        return s


{% for t in ts.uniquetypes %}
{% if t not in ts.inventory_types %}
{% set tclassname = ts.classname(t) %}
{% if t in ts.resources %}{% set tclassname = tclassname + 'StateVar'%}{% endif %}
cdef class {{tclassname}}(StateVar):
    """State variable descriptor for {{ts.cpptypes[t]}}"""

    def __cinit__(self, object value=None,
            {%- for pyname, cppname, typename, kwval in annotations -%}{%- if pyname not in nonuser_annotations -%}
            {{typename}} {{pyname}}={{kwval}},
            {%- endif -%}{%- endfor -%}):
        global _N_STATEVARS
        self.value = value
        {% for pyname, cppname, _, _ in annotations -%}
        {% if pyname == 'type' %}
        self.type = {{repr(ts.norms[t])}}
        {% elif pyname == 'uniquetypeid' %}
        self.uniquetypeid = {{ts.ids[t]}}
        {%- else %}
        self.{{pyname}} = {{pyname}}
        {%- endif -%}{% endfor %}
        if index < 0:
            self.index = _N_STATEVARS
            _N_STATEVARS += 1

    cpdef {{tclassname}} copy(self):
        """Copies the {{tclassname}} into a new instance."""
        return {{tclassname}}(value=self.value,
            {%- for pyname, cppname, _, _ in annotations -%}{%- if pyname not in nonuser_annotations -%}
            {{pyname}}=self.{{pyname}},
            {%- endif -%}{%- endfor -%}
            )

{% endif %}{% endfor %}

cdef class Inventory:
    """This class represents the special state variables that represent inventories
    on a Cyclus agent. It presents the same duck-type interface to the StateVar
    class, but does not inherit from StateVar.

    In addition to the usual state variable annotations, this also has a
    capacity annotation which can be used to specify another state var to
    read the capcity value from. The capacity state var should be a Float or
    Double.
    """

    def __cinit__(self, object value=None,
            {%- for pyname, cppname, typename, kwval in annotations -%}
            {{typename}} {{pyname}}={{kwval}},
            {%- endfor -%}object capacity=None, object _kind=None):
        global _N_STATEVARS
        self.value = value
        {% for pyname, cppname, _, _ in annotations -%}
        self.{{pyname}} = {{pyname}}
        {% endfor %}
        if index < 0:
            self.index = _N_STATEVARS
            _N_STATEVARS += 1

    def _init(self):
        """This is a delayed initializer for setting the value to a new
        instance of the resource buffer. The reason this exists is because
        inventory instances are present on both agent classes and agent
        instances. We want to make sure that

        1. resource buffers (value) are not shared between the objects and
           their agent class.
        2. resource buffers are not shared between different instances of a
           class.
        3. The class should not actually have a resource buffer (value=None)

        So agent objects, after copying inventories from their class to self,
        should call this method to get a real resource buffer instance.
        """
        self.value = self._kind(init=True)

    #
    # Descriptor interface
    #
    def __get__(self, obj, cls):
        if obj is None:
            return self
        return obj.__dict__[self.name].value

    def __set__(self, obj, val):
        if obj is None:
            self.value = val
        else:
            obj.__dict__[self.name].value = val

    cpdef dict to_dict(self):
        """Returns a representation of this inventory as a dict."""
        return {'value': self.value,
            {%- for pyname, cppname, _, _ in annotations -%}
            '{{pyname}}': self.{{pyname}},
            {%- endfor -%}
            'capacity': self.capacity,
            }

    cpdef Inventory copy(self):
        """Copies the inventory into a new instance. Note that copying an
        inventory does not copy the underlying resource buffer.
        """
        return Inventory(value=self.value,
            {%- for pyname, cppname, _, _ in annotations -%}
            {{pyname}}=self.{{pyname}},
            {%- endfor -%}
            capacity=self.capacity,
            _kind=self._kind,
            )

    def __str__(self):
        s = self.__class__.__name__ + "(" + (
            "value=" + str(self.value) + ", " +
            {%- for pyname, _, _, _ in annotations -%}
            "{{pyname}}=" + str(self.{{pyname}}) + ", " +
            {%- endfor -%}
            "capacity=" + str(self.capacity) + ", " +
            ")")
        return s

    def __repr__(self):
        s = "cyclus.typesystem." + self.__class__.__name__ + "(" + (
            "value=" + repr(self.value) + ", " +
            {%- for pyname, _, _, _ in annotations -%}
            "{{pyname}}=" + repr(self.{{pyname}}) + ", " +
            {%- endfor -%}
            "capacity=" + repr(self.capacity) + ", " +
            ")")
        return s



{% for t in ts.inventory_types %}{% set tclassname = ts.classname(t) %}
cdef class {{tclassname}}Inv(Inventory):
    """Inventory descriptor for {{ts.cpptypes[t]}}"""

    def __cinit__(self, object value=None,
            {%- for pyname, cppname, typename, kwval in annotations -%}{%- if pyname not in nonuser_annotations -%}
            {{typename}} {{pyname}}={{kwval}},
            {%- endif -%}{%- endfor -%}object capacity=None,):
        global _N_STATEVARS
        self.value = value
        {% for pyname, cppname, _, _ in annotations -%}
        {% if pyname == 'type' %}
        self.type = {{repr(ts.norms[t])}}
        {% elif pyname == 'uniquetypeid' %}
        self.uniquetypeid = {{ts.ids[t]}}
        {%- else %}
        self.{{pyname}} = {{pyname}}
        {%- endif -%}{% endfor %}
        self.capacity = capacity
        self._kind = {{tclassname}}
        if index < 0:
            self.index = _N_STATEVARS
            _N_STATEVARS += 1

    cpdef {{tclassname}}Inv copy(self):
        """Copies the {{tclassname}} into a new instance."""
        return {{tclassname}}Inv(value=self.value,
            {%- for pyname, cppname, _, _ in annotations -%}{%- if pyname not in nonuser_annotations -%}
            {{pyname}}=self.{{pyname}},
            {%- endif -%}{%- endfor -%}
            capacity=self.capacity,
            )

{% endfor %}

#
# Requests & Bids
#


{% for r in ts.resources %}
{% set cyr = ts.cython_type(r) %}
{% set rclsname = ts.classname(r) %}
{% set rfname = ts.funcname(r) %}

cdef class _{{rclsname}}Request:

    def __cinit__(self):
        self._target = None
        self._requester = None
        self._commodity = None
        self._preference = None
        self._exclusive = None
        self._cost_function = None

    @property
    def target(self):
        """This request's target {{rfname}}"""
        if self._target is not None:
            return self._target
        cdef _{{rclsname}} r = {{rclsname}}()
        r.ptx = cpp_cyclus.reinterpret_pointer_cast[cpp_cyclus.Resource,
                                                    {{cyr}}](self.ptx.target())
        self._target = r
        return self._target

    @property
    def requester(self):
        """This request's agent"""
        if self._requester is not None:
            return self._requester
        self._requester = lib.agent_to_py(dynamic_cast[agent_ptr](self.ptx.requester()),
                                          None)
        return self._requester

    @property
    def commodity(self):
        """This request's commodity"""
        if self._commodity is not None:
            return self._commodity
        cdef std_string c = self.ptx.commodity()
        self._commodity = std_string_to_py(c)
        return self._commodity

    @property
    def preference(self):
        """This request's preference"""
        if self._preference is not None:
            return self._preference
        self._preference = self.ptx.preference()
        return self._preference

    @property
    def exclusive(self):
        """This request's exclusivity"""
        if self._exclusive is not None:
            return self._exclusive
        cdef cpp_bool ex = self.ptx.exclusive()
        self._exclusive = bool_to_py(ex)
        return self._exclusive

    @property
    def cost_function(self):
        """This request's cost function"""
        raise RuntimeError("Cost functions are not yet supported in the Python "
                           "bindings.")
        #if self._cost_function is not None:
        #    return self._cost_function
        #self._cost_function = self.ptx.cost_function()
        #return self._cost_function

    def __hash__(self):
        return (<size_t> self.ptx)


class {{rclsname}}Request(_{{rclsname}}Request):
    """An representation of a request for a {{rfname}}"""


cdef dict {{rfname}}_commod_map_to_py(cpp_cyclus.CommodMap[{{cyr}}].type m):
    """Converts a CommodMap[{{rclsname}}] to a dict of tuples of requests."""
    cdef dict rtn = {}
    for item in m:
        key = std_string_to_py(item.first)
        val = []
        for x in item.second:
            r = {{rclsname}}Request()
            (<_{{rclsname}}Request> r).ptx = x
            val.append(r)
        rtn[key] = tuple(val)
    return rtn


cdef shared_ptr[cpp_cyclus.RequestPortfolio[{{cyr}}]] {{ ts.funcname(r) }}_request_portfolio_to_cpp(object pyport, cpp_cyclus.Trader* requester):
    cdef shared_ptr[cpp_cyclus.RequestPortfolio[{{cyr}}]] port = \
        shared_ptr[cpp_cyclus.RequestPortfolio[{{cyr}}]](
            new cpp_cyclus.RequestPortfolio[{{cyr}}]()
            )

    cdef std_vector[cpp_cyclus.RequestPortfolio[{{cyr}}].request_ptr] mreqs
    cdef cpp_cyclus.Request[{{cyr}}]* single_request
    cdef std_string commod
    cdef _{{rclsname}} targ
    cdef shared_ptr[{{cyr}}] targ_ptr

    # add requests
    for commodity in pyport['commodities']:
        for name, reqs in commodity.items():
            commod = str_py_to_cpp(name)
            for req in reqs:
                targ = <_{{rclsname}}> req['target']
                targ_ptr = reinterpret_pointer_cast[{{ts.cython_type(r)}},
                                                    cpp_cyclus.Resource](targ.ptx)
                if req['cost'] is not None:
                    raise ValueError('setting cost functions from Python is not yet '
                                    'supported.')
                single_request = port.get().AddRequest(targ_ptr, requester, commod, req['preference'],
                                req['exclusive'])
                mreqs.push_back(single_request)
    port.get().AddMutualReqs(mreqs)
    
    # add constraints
    for constr in pyport['constraints']:
        port.get().AddConstraint(
            cpp_cyclus.CapacityConstraint[{{ts.cython_type(r)}}](constr))
    return port


cdef shared_ptr[cpp_cyclus.BidPortfolio[{{cyr}}]] {{ ts.funcname(r) }}_bid_portfolio_to_cpp(object pyport, cpp_cyclus.Trader* bidder):
    cdef shared_ptr[cpp_cyclus.BidPortfolio[{{cyr}}]] port = \
        shared_ptr[cpp_cyclus.BidPortfolio[{{cyr}}]](
            new cpp_cyclus.BidPortfolio[{{cyr}}]()
            )
    #cdef _{{rclsname}}
    cdef shared_ptr[{{cyr}}] offer_ptr
    # add requests
    for bid in pyport['bids']:
        #targ = <_{{rclsname}}> req['target']
        offer_ptr = reinterpret_pointer_cast[{{ts.cython_type(r)}},
                                             cpp_cyclus.Resource](
                        (<_{{rclsname}}> bid['offer']).ptx)
        port.get().AddBid((<_{{rclsname}}Request> bid['request']).ptx,
                          offer_ptr, bidder, bid['exclusive'], bid['preference'])
    # add constraints
    for constr in pyport['constraints']:
        port.get().AddConstraint(
            cpp_cyclus.CapacityConstraint[{{ts.cython_type(r)}}](constr))
    return port


cdef class _{{rclsname}}Bid:

    def __cinit__(self):
        self._request = None
        self._offer = None
        self._bidder = None
        self._preference = None
        self._exclusive = None


    @property
    def request(self):
        """This bids request."""
        if self._request is not None:
            return self._request
        cdef _{{rclsname}}Request req = {{rclsname}}Request()
        req.ptx = self.ptx.request()
        self._request = req
        return self._request

    @property
    def offer(self):
        """This bid's offer {{rfname}}"""
        if self._offer is not None:
            return self._offer
        cdef _{{rclsname}} r = {{rclsname}}()
        r.ptx = cpp_cyclus.reinterpret_pointer_cast[cpp_cyclus.Resource,
                                                    {{cyr}}](
                    self.ptx.offer())
        self._offer = r
        return self._offer

    @property
    def requester(self):
        """This bid's requester"""
        return self.request.requester

    @property
    def bidder(self):
        """This bid's agent"""
        if self._bidder is not None:
            return self._bidder
        self._bidder = lib.agent_to_py(<cpp_cyclus.Agent*> self.ptx.bidder(), None)
        return self._bidder

    @property
    def commodity(self):
        """This bid's commodity"""
        return self.request._commodity

    @property
    def preference(self):
        """This bid's preference"""
        if self._preference is not None:
            return self._preference
        self._preference = self.ptx.preference()
        return self._preference

    @property
    def exclusive(self):
        """This bid's exclusivity"""
        if self._exclusive is not None:
            return self._exclusive
        cdef cpp_bool ex = self.ptx.exclusive()
        self._exclusive = bool_to_py(ex)
        return self._exclusive

    def __hash__(self):
        return (<size_t> self.ptx)


class {{rclsname}}Bid(_{{rclsname}}Bid):
    """An representation of a bid for a {{rfname}}"""


cdef dict {{rfname}}_pref_map_to_py(cpp_cyclus.PrefMap[{{cyr}}].type& pm):
    """Converts a {{rfname}} prefmap to a Python dict."""
    cdef dict rtn = {}
    for rbd in pm:
        r = {{rclsname}}Request()
        (<_{{rclsname}}Request> r).ptx = rbd.first
        for bd in rbd.second:
            b = {{rclsname}}Bid()
            (<_{{rclsname}}Bid> b).ptx = bd.first
            rtn[(r, b)] = bd.second
    return rtn


cdef class _{{rclsname}}Trade:

    def __cinit__(self):
        self._request = None
        self._bid = None

    @property
    def request(self):
        """This trade's request."""
        if self._request is not None:
            return self._request
        cdef _{{rclsname}}Request req = {{rclsname}}Request()
        req.ptx = self.ptx.request
        self._request = req
        return self._request

    @property
    def bid(self):
        """This trade's bid."""
        if self._bid is not None:
            return self._bid
        cdef _{{rclsname}}Bid bid = {{rclsname}}Bid()
        bid.ptx = self.ptx.bid
        self._bid = bid
        return self._bid

    @property
    def amt(self):
        """The quantity assigned to the Trade. May be less than either the
        request or bid quantity.
        """
        return self.ptx.amt

    @property
    def price(self):
        """Trades have a price member which is not currently used."""
        return self.ptx.price

    def __hash__(self):
        return id(self)


class {{rclsname}}Trade(_{{rclsname}}Trade):
    """ Trade is a simple container that associates a request for a
    resource with a bid for that resource.
    """


cdef tuple {{rfname}}_trade_vector_to_py(const std_vector[cpp_cyclus.Trade[{{cyr}}]]& trades):
    """Converts a vector of {{rfname}} trades to a tuple"""
    cdef list pytrades = []
    cdef int i, n
    n = trades.size()
    for i in range(n):
        t = {{rclsname}}Trade()
        (<_{{rclsname}}Trade> t).ptx = const_cast[{{rfname}}_trade_ptr](&(trades[i]))
        pytrades.append(t)
    cdef tuple rtn = tuple(pytrades)
    return rtn


cdef dict {{rfname}}_responses_to_py(const std_vector[std_pair[cpp_cyclus.Trade[{{cyr}}], shared_ptr[{{cyr}}]]]& responses):
    """Converts a vector of pairs of (trades, {{rfname}}) to a dict"""
    cdef dict rtn = {}
    cdef int i, n
    n = responses.size()
    for i in range(n):
        t = {{rclsname}}Trade()
        (<_{{rclsname}}Trade> t).ptx = const_cast[{{rfname}}_trade_ptr](&(responses[i].first))
        r = {{rclsname}}()
        (<_{{rclsname}}> r).ptx = reinterpret_pointer_cast[cpp_cyclus.Resource,
                                                           {{cyr}}](responses[i].second)
        rtn[t] = r
    return rtn

{% endfor %}


cdef tuple request_types = ({% for r in ts.resources %}{{ ts.classname(r) }}Request, {% endfor %})
cdef tuple bid_types = ({% for r in ts.resources %}{{ ts.classname(r) }}Bid, {% endfor %})
cdef tuple trade_types = ({% for r in ts.resources %}{{ ts.classname(r) }}Trade, {% endfor %})


#
# Helpers
#
def prepare_type_representation(cpptype, othertype):
    """Updates othertype to conform to the length of cpptype using None's.
    """
    cdef int i, n
    if not isinstance(cpptype, str):
        n = len(cpptype)
        if isinstance(othertype, str):
            othertype = [othertype]
        if othertype is None:
            othertype = [None] * n
        elif len(othertype) < n:
            othertype.extend([None] * (n - len(othertype)))
        # recurse down
        for i in range(1, n):
            othertype[i] = prepare_type_representation(cpptype[i], othertype[i])
        return othertype
    else:
        return othertype


'''.lstrip())


def typesystem_pyx(ts, ns):
    """Creates the Cython wrapper for the Cyclus type system."""
    nonuser_annotations = ('type', 'uniquetypeid')
    ctx = dict(
        ts=ts,
        dbtypes=ts.dbtypes,
        cg_warning=CG_WARNING,
        npy_imports=NPY_IMPORTS,
        stl_cimports=STL_CIMPORTS,
        set=set,
        repr=repr,
        sorted=sorted,
        enumerate=enumerate,
        annotations=ANNOTATIONS,
        nonuser_annotations=nonuser_annotations,
        uniquestrtypes = [t for t in ts.uniquetypes
                          if isinstance(ts.norms[t], unicode_types)],
        uniquetuptypes = sorted([(ts.norms[t], t) for t in ts.uniquetypes
                                 if not isinstance(ts.norms[t], unicode_types)], reverse=True,
                                key=lambda x: (x[0][0], x[1])),
        groupby=itertools.groupby,
        firstfirst=lambda x: x[0][0],
        )
    rtn = TYPESYSTEM_PYX.render(ctx)
    return rtn

TYPESYSTEM_PXD = JENV.from_string('''
{{ cg_warning }}

{{ stl_cimports }}

# local imports
from cyclus cimport cpp_typesystem
from cyclus cimport cpp_cyclus
from cyclus.cpp_cyclus cimport shared_ptr, reinterpret_pointer_cast

#
# Resources & Inventories
#

cdef class _Resource:
    cdef shared_ptr[cpp_cyclus.Resource] ptx

cdef shared_ptr[cpp_cyclus.Composition] composition_ptr_from_py(object, object)
cdef object composition_from_cpp(shared_ptr[cpp_cyclus.Composition] comp, object basis)

cdef class _Material(_Resource):
    pass

cdef class _Product(_Resource):
    pass

{% for t in ts.inventory_types %}
{% set tclassname = ts.classname(t)%}
cdef class _{{tclassname}}:
    cdef {{ts.cython_type(t)}}* ptx
    cdef bint _free
{% endfor %}

#
# raw
#
cdef dict C_RANKS
cdef dict C_NAMES
cdef dict C_IDS
cdef dict C_CPPTYPES
cdef dict C_NORMS

#
# typedefs
#
{% for t in ts.uniquetypes %}
ctypedef {{ ts.cython_type(t) }} {{ ts.funcname(t) }}_t
{%- endfor %}
ctypedef cpp_cyclus.Agent* agent_ptr
#
# converters
#
cdef bytes blob_to_bytes(cpp_cyclus.Blob value)

cdef object uuid_cpp_to_py(cpp_cyclus.uuid x)


cdef cpp_cyclus.uuid uuid_py_to_cpp(object x)

cdef std_string str_py_to_cpp(object x)

{% for n in sorted(set(ts.norms.values()), key=ts.funcname) %}
cdef object {{ ts.funcname(n) }}_to_py({{ ts.possibly_shared_cython_type(n) }} x)
{%- endfor %}

{% for n in sorted(set(ts.norms.values()), key=ts.funcname) %}
cdef object any_{{ ts.funcname(n) }}_to_py(cpp_cyclus.hold_any value)
{%- endfor %}

{% for n in sorted(set(ts.norms.values()), key=ts.funcname) %}
cdef {{ ts.possibly_shared_cython_type(n) }} {{ ts.funcname(n) }}_to_cpp(object x)
{%- endfor %}


#
# type system functions
#
cdef object db_to_py(cpp_cyclus.hold_any value, cpp_cyclus.DbTypes dbtype)

cdef cpp_cyclus.hold_any py_to_any(object value, object t)

cdef cpp_cyclus.hold_any py_to_any_by_dbtype(object value, cpp_cyclus.DbTypes dbtype)

cdef cpp_cyclus.hold_any py_to_any_by_norm(object value, object norm)

cdef object any_to_py(cpp_cyclus.hold_any value)

cdef object new_py_inst(cpp_cyclus.DbTypes dbtype)

#
# State Variable Descriptors
#

cdef class StateVar:
    cdef public object value
    {% for pyname, cppname, typename, _ in annotations %}
    cdef public {{typename}} {{pyname}} {% if pyname != cppname %}"{{cppname}}"{% endif %}
    {%- endfor %}
    cpdef dict to_dict(self)
    cpdef StateVar copy(self)

{% for t in ts.uniquetypes %}
{% if t not in ts.inventory_types %}
{% set tclassname = ts.classname(t) %}
{% if t in ts.resources %}{% set tclassname = tclassname + 'StateVar'%}{% endif %}
cdef class {{tclassname}}(StateVar):
    cpdef {{tclassname}} copy(self)
{% endif %}{% endfor %}


cdef class Inventory:
    cdef public object value
    {% for pyname, cppname, typename, _ in annotations %}
    cdef public {{typename}} {{pyname}} {% if pyname != cppname %}"{{cppname}}"{% endif %}
    {%- endfor %}
    cdef public object capacity
    cdef object _kind
    cpdef dict to_dict(self)
    cpdef Inventory copy(self)

{% for t in ts.inventory_types %}{% set tclassname = ts.classname(t) %}
cdef class {{tclassname}}Inv(Inventory):
    cpdef {{tclassname}}Inv copy(self)
{% endfor %}

#
# Requests & Bids
#
{% for r in ts.resources %}
{% set cyr = ts.cython_type(r) %}
{% set rclsname = ts.classname(r) %}
{% set rfname = ts.funcname(r) %}
cdef class _{{rclsname}}Request:
    cdef cpp_cyclus.Request[{{cyr}}]* ptx
    cdef object _target
    cdef object _requester
    cdef object _commodity
    cdef object _preference
    cdef object _exclusive
    cdef object _cost_function

cdef dict {{rfname}}_commod_map_to_py(cpp_cyclus.CommodMap[{{cyr}}].type& m)
cdef shared_ptr[cpp_cyclus.RequestPortfolio[{{cyr}}]] {{ ts.funcname(r) }}_request_portfolio_to_cpp(object pyport, cpp_cyclus.Trader* requester)
cdef shared_ptr[cpp_cyclus.BidPortfolio[{{cyr}}]] {{ ts.funcname(r) }}_bid_portfolio_to_cpp(object pyport, cpp_cyclus.Trader* requester)

cdef class _{{rclsname}}Bid:
    cdef cpp_cyclus.Bid[{{cyr}}]* ptx
    cdef object _request
    cdef object _offer
    cdef object _bidder
    cdef object _preference
    cdef object _exclusive

cdef dict {{rfname}}_pref_map_to_py(cpp_cyclus.PrefMap[{{cyr}}].type& pm)

ctypedef cpp_cyclus.Trade[{{cyr}}]* {{rfname}}_trade_ptr

cdef class _{{rclsname}}Trade:
    cdef cpp_cyclus.Trade[{{cyr}}]* ptx
    cdef object _request
    cdef object _bid


cdef tuple {{rfname}}_trade_vector_to_py(const std_vector[cpp_cyclus.Trade[{{cyr}}]]& trades)
cdef dict {{rfname}}_responses_to_py(const std_vector[std_pair[cpp_cyclus.Trade[{{cyr}}], shared_ptr[{{cyr}}]]]& responses)

{% endfor %}

cdef tuple request_types
cdef tuple bid_types
cdef tuple trade_types
''')

def typesystem_pxd(ts, ns):
    """Creates the Cython wrapper header for the Cyclus type system."""
    ctx = dict(
        ts=ts,
        dbtypes=ts.dbtypes,
        cg_warning=CG_WARNING,
        npy_imports=NPY_IMPORTS,
        stl_cimports=STL_CIMPORTS,
        set=set,
        sorted=sorted,
        enumerate=enumerate,
        annotations=ANNOTATIONS,
        )
    rtn = TYPESYSTEM_PXD.render(ctx)
    return rtn


#
# CLI
#

def parse_args(argv):
    """Parses typesystem arguments for code generation."""
    parser = argparse.ArgumentParser()
    parser.add_argument('--verbose', default=False, action='store_true',
                        dest='verbose',
                        help="whether to give extra information at run time.")
    parser.add_argument('--src-dir', default='.', dest='src_dir',
                        help="the local source directory, default '.'")
    parser.add_argument('--test-dir', default='tests', dest='test_dir',
                        help="the local tests directory, default 'tests'")
    parser.add_argument('--build-dir', default='build', dest='build_dir',
                        help="the local build directory, default 'build'")
    parser.add_argument('--cpp-typesystem', default='cpp_typesystem.pxd',
                        dest='cpp_typesystem',
                        help="the name of the C++ typesystem header, "
                             "default 'cpp_typesystem.pxd'")
    parser.add_argument('--typesystem-pyx', default='typesystem.pyx',
                        dest='typesystem_pyx',
                        help="the name of the Cython typesystem wrapper, "
                             "default 'typesystem.pyx'")
    parser.add_argument('--typesystem-pxd', default='typesystem.pxd',
                        dest='typesystem_pxd',
                        help="the name of the Cython typesystem wrapper header, "
                             "default 'typesystem.pxd'")
    dbtd = os.path.join(os.path.dirname(__file__), '..', 'share', 'dbtypes.json')
    parser.add_argument('--dbtypes-json', default=dbtd,
                        dest='dbtypes_json',
                        help="the path to dbtypes.json file, "
                             "default " + dbtd)
    parser.add_argument('--data-model-version', default=None,
                        dest='data_model_version',
                        help="The Cyclus API version to target."
                        )
    ns = parser.parse_args(argv)
    return ns


def setup(ns):
    """Ensure that we are ready to perform code generation. Returns typesystem."""
    if not os.path.exists(ns.build_dir):
        os.mkdir(ns.build_dir)
    if not os.path.isfile(ns.dbtypes_json):
        try:
            instdir = safe_output(['cyclus', '--install-path'])
        except (subprocess.CalledProcessError, OSError):
            # fallback for conda version of cyclus
            instdir = safe_output(['cyclus_base', '--install-path'])
        ns.dbtypes_json = os.path.join(instdir.strip().decode(), 'share',
                                       'cyclus', 'dbtypes.json')
    with io.open(ns.dbtypes_json, 'r') as f:
        tab = json.load(f)
    # get cyclus version
    verstr = ns.data_model_version
    if verstr is None:
        try:
            verstr = safe_output(['cyclus', '--version']).split()[2]
        except (subprocess.CalledProcessError, OSError):
            # fallback for conda version of cyclus
            try:
                verstr = safe_output(['cyclus_base', '--version']).split()[2]
            except (subprocess.CalledProcessError, OSError):
                # fallback using the most recent value in JSON
                ver = set([row[5] for row in tab[1:]])
                ver = max([tuple(map(int, s[1:].partition('-')[0].split('.'))) for s in ver])
    if verstr is not None:
        if isinstance(verstr, bytes):
            verstr = verstr.decode()
        ns.data_model_version = verstr
        ver = tuple(map(int, verstr.partition('-')[0].split('.')))
    if ns.verbose:
        print('Found cyclus version: ' + verstr, file=sys.stderr)
    # make and return a type system
    ts = TypeSystem(table=tab, cycver=ver, rawver=verstr,
            cpp_typesystem=os.path.splitext(ns.cpp_typesystem)[0])
    return ts


def code_gen(ts, ns):
    """Generates code given a type system and a namespace."""
    cases = [(cpp_typesystem, ns.cpp_typesystem),
             (typesystem_pyx, ns.typesystem_pyx),
             (typesystem_pxd, ns.typesystem_pxd),]
    for func, basename in cases:
        s = func(ts, ns)
        fname = os.path.join(ns.src_dir, basename)
        orig = None
        if os.path.isfile(fname):
            with io.open(fname, 'r') as f:
                orig = f.read()
        if orig is None or orig != s:
            with io.open(fname, 'w') as f:
                f.write(s)


def main(argv=None):
    """Entry point into the code generation. Accepts list of command line arguments."""
    if argv is None:
        argv = sys.argv[1:]
    ns = parse_args(argv)
    ts = setup(ns)
    code_gen(ts, ns)


if __name__ == "__main__":
    main()

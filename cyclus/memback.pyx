"""An in-memory Python backend for Cyclus."""
from __future__ import print_function, unicode_literals
from libcpp.map cimport map as std_map
from libcpp.set cimport set as std_set
from libcpp.vector cimport vector as std_vector
from libcpp.utility cimport pair as std_pair
from libcpp.string cimport string as std_string
from libcpp cimport bool as cpp_bool

from cpython cimport (PyObject, PyDict_New, PyDict_Contains,
    PyDict_GetItemString, PyDict_SetItemString, PyString_FromString,
    PyBytes_FromString, PyDict_GetItem, PyDict_SetItem)

from cyclus cimport cpp_cyclus
from cyclus cimport lib
from cyclus import lib

from cyclus.typesystem cimport (py_to_any, any_to_py, str_py_to_cpp,
    std_string_to_py, bool_to_py, bool_to_cpp, std_set_std_string_to_py,
    std_set_std_string_to_cpp)

from collections import deque
from collections.abc import Set
from ast import (Name, Compare, Load, Eq, NotEq, Lt, LtE, Gt, GtE,
    BinOp, BitAnd, Expression)

# startup numpy
cimport numpy as np
import numpy as np
import pandas as pd

np.import_array()
np.import_ufunc()


cdef cppclass CyclusMemBack "CyclusMemBack" (cpp_cyclus.RecBackend):
    # A C++ class that acts as a rec backend, but stores its data in
    # memory in a Python friendly format.

    dict Init():
        """Initilaizer that returns cache, so that we can keep an extra
        reference to it around.
        """
        this.store_all_tables = 1  # set to true, by default
        c = {}
        this.cache = <PyObject*> c
        return c

    void Notify(cpp_cyclus.DatumList data):
        """Stores the data in a cache."""
        cdef std_map[std_string, cpp_cyclus.DatumList] groups
        cdef std_pair[std_string, cpp_cyclus.DatumList] group
        cdef cpp_cyclus.Datum* d
        cdef std_pair[const char*, cpp_cyclus.hold_any] val
        cdef std_string name, cpp_field
        cdef dict g, res
        cdef PyObject* pyobval
        cdef object results, pyval
        cdef int key_exists, i
        cdef list fields
        # check if there is anything to do
        if not this.store_all_tables and this.registry.size() == 0:
            return
        if data.size() == 0:
            return
        # combine into like groups
        if this.store_all_tables:
            for d in data:
                name = d.title()
                groups[name].push_back(d)
        else:
            for d in data:
                name = d.title()
                if this.registry.count(name) == 0:
                    continue
                groups[name].push_back(d)
            if groups.size() == 0:
                # no tables seen that are in the registry
                return
        # convert groups
        for group in groups:
            # init group
            res = {}
            fields = []
            d = group.second.front()
            for cpp_field in d.fields():
                field = std_string_to_py(cpp_field)
                fields.append(field)
                res[field] = []
            # fill group
            name = group.first
            for d in group.second:
                i = 0
                for val in d.vals():
                    res[fields[i]].append(any_to_py(val.second))
                    i += 1
            results = pd.DataFrame(res, columns=fields[:])
            pyname = std_string_to_py(name)
            key_exists = PyDict_Contains(<object> this.cache, pyname)
            if key_exists:
                pyobval = PyDict_GetItem(<object> this.cache, pyname)
                pyval = <object> pyobval
                results = pd.concat([pyval, results], ignore_index=True)
            PyDict_SetItem(<object> this.cache, pyname, results)

    std_string Name():
        """The name of the backend"""
        return std_string(b"<Python In-Memory Backend>")

    void Flush():
        """No-op for in-memory"""
        pass

    void Close():
        """Closes the In-memory backend by setting the cache to None."""
        c = None
        this.cache = <PyObject*> c


cdef class _MemBack(lib._FullBackend):

    def __cinit__(self, registry=True, fallback=None):
        self.ptx = new CyclusMemBack()
        self.cache = (<CyclusMemBack*> self.ptx).Init()
        self._registry = None
        self.registry = registry
        self.fallback = fallback
        self._query_code_cache = {}
        self._query_code_lru = deque()

    def __dealloc__(self):
        # Note that we have to do it this way since self.ptx is void*
        if self.ptx == NULL:
            return
        cdef CyclusMemBack* cpp_ptx = <CyclusMemBack*> self.ptx
        cpp_ptx.Close()
        del cpp_ptx
        self.ptx = NULL

    #
    # RecBackened Interface
    #
    def query(self, table, conds=None):
        """Queries a database table.

        Parameters
        ----------
        table : str
            The table name.
        conds : iterable, optional
            A list of conditions.

        Returns
        -------
        results : pd.DataFrame
            Pandas DataFrame the represents the table. If None is returned,
            the table could not be found.
        """
        if table in self.cache:
            if conds is None:
                return self.cache[table]
            else:
                return self._apply_conds(self.cache[table], conds)
        elif self.fallback is not None:
            if self.store_all_tables or table in self.registry:
                try:
                    t = self.fallback.query(table, conds=None)
                except RuntimeError:
                    return None
                self.cache[table] = t
                if conds is None:
                    return t
                else:
                    return self._apply_conds(self.cache[table], conds)
            else:
                try:
                    return self.fallback.query(table, conds=conds)
                except RuntimeError:
                    return None
        else:
            return None

    @property
    def tables(self):
        """Retrieves the set of tables present in the database."""
        return frozenset(self.cache.keys())

    @tables.setter
    def tables(self, value):
        raise NotImplementedError

    def flush(self):
        """No-op since in-memory."""
        # this is a no-op

    def close(self):
        """Closes the backend, flushing it in the process."""
        self.flush()  # just in case
        (<CyclusMemBack*> self.ptx).Close()
        self.cache = None

    @property
    def name(self):
        """The name of the database."""
        return "<Python In-Memory Backend at " + str(<unsigned long> self.ptx) + ">"

    #
    # Extra Interface
    #
    @property
    def store_all_tables(self):
        """Whether or not the backend will store all tables or only
        those in the registry.
        """
        return bool_to_py((<CyclusMemBack*> self.ptx).store_all_tables)

    @property
    def registry(self):
        """A set of table names to store when store_all_tables is true.
        Setting this to True will store all tables. Setting this to None
        or False will clear all currently registered table names. Otherwise,
        Setting this to a set of strings will restrict the storage to
        just the specified tables. Setting the registry will also clear
        old cache values.
        """
        if self._registry is None:
            r = std_set_std_string_to_py((<CyclusMemBack*> self.ptx).registry)
            self._registry = frozenset(r)
        return self._registry

    @registry.setter
    def registry(self, val):
        cdef CyclusMemBack* cpp_ptx = <CyclusMemBack*> self.ptx
        cache = self.cache
        if val is None or isinstance(val, bool):
            cpp_ptx.registry.clear()
            if val:
                cpp_ptx.store_all_tables = True
            else:
                cpp_ptx.store_all_tables = False
                cache.clear()
            self._registry = None
        else:
            if not isinstance(val, Set):
                val = frozenset(val)
            old = self.registry
            cpp_ptx.registry = std_set_std_string_to_cpp(val)
            cpp_ptx.store_all_tables = False
            self._registry = None
            # find keys in the old registry but not in the new one and
            # also in the current cache. Then remove them.
            dirty_keys = frozenset(cache.keys()) & (old - val)
            for key in dirty_keys:
                del cache[key]

    #
    # Condition application
    #
    def _apply_conds(self, df, conds):
        """Applies the condition query to data frame."""
        key = ''
        ctx = {'__builtins__': None}
        for i, (field, op, val) in enumerate(conds):
            key += field + op  # the value doesn't affect the key since it is loaded.
            if field not in ctx:
                ctx[field] = df[field]
            ctx['val' + str(i)] = val
        qccache = self._query_code_cache
        if key in qccache:
            code = qccache[key]
        else:
            code = qccache[key] = self._compile_conds(conds)
            qclru = self._query_code_lru
            if len(qclru) == 256:
                del qccache[qclru.popleft()]
            qclru.append(key)
        mask = eval(code, ctx, ctx)
        res = df[mask]
        return res

    def _compile_conds(self, conds):
        """Compiles conditions into a code object."""
        tree = Expression(body=self._parse_conds(conds))
        code = compile(tree, '<MemBack Query>', 'eval')
        return code

    def _parse_conds(self, conds):
        """Parses conditions and then ANDs them together, eg.
        cond0 & cond1 & cond2, etc.
        """
        for i, (field, op, _) in enumerate(conds):
            cond_tree = self._parse_cond(field, op, 'val' + str(i))
            if i == 0:
                tree = cond_tree
            else:
                tree = BinOp(left=tree, op=BitAnd(), right=cond_tree,
                             lineno=1, col_offset=0)
        return tree

    def _parse_cond(self, field, op, val):
        """Parses a single condition into the expression of the form
        field < val, field == val, etc.
        """
        return Compare(left=Name(id=field, ctx=Load(), lineno=1, col_offset=1),
                       ops=[self._cmpnodes[op]()],
                       comparators=[Name(id=val, ctx=Load(),
                                         lineno=1, col_offset=1)],
                       lineno=1, col_offset=1)

    _cmpnodes = {'==': Eq, '!=': NotEq, '<': Lt, '<=': LtE, '>': Gt, '>=': GtE}



class MemBack(_MemBack, lib.FullBackend):
    """An in-memory Python backend database.

    Note that even though the underlying C++ class is a RecBackend,
    the Python wrapper inherits from FullBackend and the QueryableBackend
    interface in implemented in Cython.

    Parameters
    ----------
    registry : set, bool, or None, optional
        The initial registry to start the backend with. Defaults is True,
        which stores all of the tables.
    fallback : QueryableBackend-like, optional
        A backend, which implements query(), that this backend can use
        to look up values if the table is not in the current cache.
    """

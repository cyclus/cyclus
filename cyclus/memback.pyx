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
    std_string_to_py)

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
        # combine into like groups
        for d in data:
            name = d.title()
            groups[name].push_back(d)
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
            results = pd.DataFrame(res, columns=fields)
            pyname = std_string_to_py(name)
            key_exists = PyDict_Contains(<object> this.cache, pyname)
            if key_exists:
                pyobval = PyDict_GetItem(<object> this.cache, pyname)
                pyval = <object> pyobval
                results = pd.concatenate([pyval, results])
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

    def __cinit__(self):
        self.ptx = new CyclusMemBack()
        self.cache = (<CyclusMemBack*> self.ptx).Init()

    def __dealloc__(self):
        # Note that we have to do it this way since self.ptx is void*
        if self.ptx == NULL:
            return
        cdef CyclusMemBack* cpp_ptx = <CyclusMemBack*> self.ptx
        cpp_ptx.Close()
        del cpp_ptx
        self.ptx = NULL

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
            Pandas DataFrame the represents the table
        """
        if table not in self.cache:
            return None
        return self.cache[table]

    @property
    def tables(self):
        return frozenset(self.cache.keys())

    @tables.setter
    def tables(self, value):
        """Retrieves the set of tables present in the database."""
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


class MemBack(_MemBack, lib.FullBackend):
    """An in-memory Python backend database.

    Note that even though the underlying C++ class is a RecBackend,
    the Python wrapper inherits from FullBackend and the QueryableBackend
    interface in implemented in Cython.
    """

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
    PyBytes_FromString)

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
        cdef std_string name
        cdef dict g, res
        cdef list fields
        #cdef bytes bfield
        #cdef str field, pyname
        cdef const char* cname
        cdef PyObject* pyobval
        cdef object results, pyval
        cdef int key_exists
        #print("memnot0")
        # combine into like groups
        for d in data:
            name = d.title()
            groups[name].push_back(d)
        # convert groups
        #print("memnot1")
        for group in groups:
            # init group
            res = {}
            fields = []
            d = group.second[0]
            #print("memnot2")
            for val in d.vals():
                bfield = val.first
                #bfield = PyBytes_FromString(<char*> val.first)
                field = bfield.decode()
                #field = std_string_to_py(std_string(val.first))
                #print("get field", val.first)
                fields.append(field)
                res[field] = []
            # fill group
            #print("memnot3")
            name = group.first
            for d in group.second:
                for val in d.vals():
                    #print("memnot4")
                    bfield = val.first
                    field = bfield.decode()
                    #print("my field", field)
                    res[field].append(any_to_py(val.second))
            print("memnot5")
            print("res", res)
            print("fields", fields)
            results = pd.DataFrame(res, columns=fields)
            print(results)
            print("memnot5.25")
            cname = name.c_str()
            print("memnot5.5")
            pyname = std_string_to_py(name)
            print("memnot5.75")
            key_exists = PyDict_Contains(<object> this.cache, pyname)
            print("memnot6")
            if key_exists:
                pyobval = PyDict_GetItemString(<object> this.cache, cname)
                pyval = <object> pyobval
                results = pd.concatenate([pyval, results])
            print("memnot7")
            PyDict_SetItemString(<object> this.cache, cname, results)
            print("memnot8")
        print("memnot9")


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
        print("me")
        cpp_ptx.Close()
        print("I")
        del cpp_ptx
        print("you")
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
        #cdef dict cache = <dict> (<CyclusMemBack*> self.ptx).cache
        #cache = <dict> (<CyclusMemBack*> self.ptx).cache
        #print("cache id", str(<unsigned long> (<CyclusMemBack*> self.ptx).cache))
        #cdef object rtn
        #cdef char* ctable
        #table = table.encode()
        #ctable = table
        #print("query0")
        #if (<CyclusMemBack*> self.ptx).cache == NULL:
        #    print("Seeing Null")
        #print(cache)
        #if table not in cache:
        #    return None
        return self.cache[table]
        #rtn = <object> PyDict_GetItemString(cache, ctable)
        #return rtn

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

"""An in-memory Python backend for Cyclus."""
from __future__ import print_function, unicode_literals
from libcpp.map cimport map as std_map
from libcpp.set cimport set as std_set
from libcpp.vector cimport vector as std_vector
from libcpp.utility cimport pair as std_pair
from libcpp.string cimport string as std_string
from libcpp cimport bool as cpp_bool

from cpython cimport PyObject, PyDict_New

from cyclus cimport cpp_cyclus
from cyclus cimport lib
from cyclus import lib

from cyclus.typesystem cimport py_to_any, any_to_py

cdef cppclass CyclusMemBack "CyclusMemBack" (cpp_cyclus.RecBackend):
    # A C++ class that acts as a rec backend, but stores its data in
    # memory in a Python friendly format.

    void Init():
        """Initilaizer"""
        cache = PyDict_New()

    void Notify(cpp_cyclus.DatumList data):
        """Stores the data in a cache."""
        cdef std_map[std_string, cpp_cyclus.DatumList] groups
        cdef std_pair[std_string, cpp_cyclus.DatumList] group
        cdef cpp_cyclus.Datum* d
        cdef std_pair[const char*, cpp_cyclus.hold_any] val
        cdef std_string name
        cdef dict g, res
        cdef list fields
        cdef bytes bfield
        cdef str field
        # combine into like groups
        for d in data:
            name = d.title()
            groups[name].push_back(d)
        # convert groups
        for group in groups:
            # init group
            res = {}
            fields = []
            d = group.second[0]
            for val in d.vals():
                bfield = val.first
                field = bfield.decode()
                fields.append(fields)
                res[field] = []
            # fill group
            for d in group.second:
                for val in d.vals():
                    bfield = val.first
                    field = bfield.decode()
                    res[field].append(any_to_py(val.second))



    std_string Name():
        """The name of the backend"""
        return std_string(b"<Python In-Memory Backend>")

    void Flush():
        """No-op for in-memory"""
        pass

    void Close():
        """No-op for in-memory"""
        pass


cdef class _MemBack(lib._FullBackend):

    def __cinit__(self):
        self.ptx = new CyclusMemBack()
        (<CyclusMemBack*> self.ptx).Init()

    def __dealloc__(self):
        cdef CyclusMemBack* cpp_ptx = <CyclusMemBack*> self.ptx
        del cpp_ptx

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
        raise NotImplementedError

    @property
    def tables(self):
        raise NotImplementedError

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

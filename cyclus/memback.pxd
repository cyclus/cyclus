"""An in-memory Python backend for Cyclus."""
from libcpp.map cimport map as std_map
from libcpp.set cimport set as std_set
from libcpp.vector cimport vector as std_vector
from libcpp.utility cimport pair as std_pair
from libcpp.string cimport string as std_string
from libcpp cimport bool as cpp_bool

from cpython cimport PyObject

from cyclus cimport cpp_cyclus
from cyclus cimport lib


cdef cppclass CyclusMemBack "CyclusMemBack" (cpp_cyclus.RecBackend):
    # Recorder backend interface
    void Notify(cpp_cyclus.DatumList) except +
    std_string Name() except +
    void Flush() except +
    void Close() except +
    # Queryable backend interface
    #cpp_cyclus.QueryResult Query(std_string, vector[cpp_cyclus.Cond]*) except +
    #std_map[std_string, cpp_cyclus.DbTypes] ColumnTypes(std_string) except +
    #std_set[std_string] Tables() except +
    void Init() except +
    PyObject* cache

cdef class _MemBack(lib._FullBackend):
    pass  # pointer declared on full backend
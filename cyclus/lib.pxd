"""Python wrapper for cyclus."""
# Cython imports
from libcpp.utility cimport pair as cpp_pair
from libcpp.set cimport set as cpp_set
from libcpp.map cimport map as cpp_map
from libcpp.vector cimport vector as cpp_vector
from libcpp.string cimport string as std_string
from cython.operator cimport dereference as deref
from cython.operator cimport preincrement as inc
from libc.stdlib cimport malloc, free
from libcpp cimport bool as cpp_bool

# local imports
from cyclus cimport cpp_cyclus
from cyclus.cpp_stringstream cimport stringstream


cdef class _Datum:
    cdef void * ptx
    cdef bint _free

cdef class _FullBackend:
    cdef void * ptx

cdef class _SqliteBack(_FullBackend):
    pass

cdef class _Hdf5Back(_FullBackend):
    pass

cdef class _Recorder:
    cdef void * ptx

cdef class _AgentSpec:
    cdef cpp_cyclus.AgentSpec * ptx

cdef class _DynamicModule:
    cdef cpp_cyclus.DynamicModule * ptx

cdef class _Env:
    pass

cdef class _Logger:
    # everything that we use on this class is static
    pass

cdef class _XMLParser:
    cdef cpp_cyclus.XMLParser * ptx

cdef class _InfileTree:
    # if InfileTree ever has C++ subclasses that we then also want
    # to wrap in Cython, the pointer will need to become
    # cdef void * ptx, and we'll need to cast each access.
    # For now, there are no subclasses  so we can get away with
    # the fully typed pointer.
    cdef cpp_cyclus.InfileTree * ptx

cdef class _Timer:
    cdef cpp_cyclus.Timer * ptx
    cdef bint _free

cdef class _SimInit:
    cdef cpp_cyclus.SimInit * ptx

cdef class _Agent:
    cdef void * ptx
    cdef bint _free

cdef class _XMLFileLoader:
    cdef cpp_cyclus.XMLFileLoader * ptx

cdef class _XMLFlatLoader:
    cdef cpp_cyclus.XMLFlatLoader * ptx

cdef class _Context:
    cdef cpp_cyclus.Context * ptx
    cdef bint _free

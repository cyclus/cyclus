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
from cymetric cimport cpp_cyclus


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

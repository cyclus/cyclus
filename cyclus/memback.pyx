"""An in-memory Python backend for Cyclus."""
from cyclus cimport cpp_cyclus
from cyclus cimport lib
from cyclus import lib


cdef cppclass CppMemBack(cpp_cyclus.FullBackend):

    pass


cdef class _MemBack(lib._FullBackend):

    def __cinit__(self):
        self.ptx = new CppMemBack()

    def __dealloc__(self):
        cdef CppMemBack* cpp_ptx = <CppMemBack*> self.ptx
        del cpp_ptx


class MemBack(_MemBack, lib.FullBackend):
    """An in-memory Python backend database."""
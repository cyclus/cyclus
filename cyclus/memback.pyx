"""An in-memory Python backend for Cyclus."""
from __future__ import print_function, unicode_literals
from libcpp.map cimport map
from libcpp.set cimport set
from libcpp.vector cimport vector
from libcpp.utility cimport pair
from libcpp.string cimport string as std_string
from libcpp cimport bool as cpp_bool

from cyclus cimport cpp_cyclus
from cyclus cimport lib
from cyclus import lib


cdef cppclass CyclusMemBack "CyclusMemBack" (cpp_cyclus.FullBackend):
    # A C++ class that acts as a full backend, but stores its data in
    # memory in a Python friendly format.

    void Notify(cpp_cyclus.DatumList dl):
        """Stores the data in a cache."""

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

    def __dealloc__(self):
        cdef CyclusMemBack* cpp_ptx = <CyclusMemBack*> self.ptx
        del cpp_ptx


class MemBack(_MemBack, lib.FullBackend):
    """An in-memory Python backend database."""

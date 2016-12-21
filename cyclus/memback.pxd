"""An in-memory Python backend for Cyclus."""
from cyclus cimport cpp_cyclus
from cyclus cimport lib


cdef cppclass CppMemBack(cpp_cyclus.FullBackend):
    pass


cdef class _MemBack(lib._FullBackend):
    pass  # pointer declared on full backend
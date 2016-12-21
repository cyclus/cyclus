"""An in-memory Python backend for Cyclus."""
from libcpp.vector cimport vector
from libcpp.utility cimport pair
from libcpp.string cimport string as std_string
from libcpp cimport bool as cpp_bool

from cyclus cimport cpp_cyclus
from cyclus cimport lib


cdef cppclass CyclusMemBack "CyclusMemBack" (cpp_cyclus.FullBackend):
    void Notify(cpp_cyclus.DatumList) except +
    std_string Name() except +
    void Flush() except +
    void Close() except +


cdef class _MemBack(lib._FullBackend):
    pass  # pointer declared on full backend
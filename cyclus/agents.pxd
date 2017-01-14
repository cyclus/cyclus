"""A Python interface for Agents and their subclasses."""
from libcpp.map cimport map as std_map
from libcpp.set cimport set as std_set
from libcpp.vector cimport vector as std_vector
from libcpp.utility cimport pair as std_pair
from libcpp.string cimport string as std_string
from libcpp cimport bool as cpp_bool

from cpython cimport PyObject

from cyclus cimport cpp_cyclus
from cyclus cimport lib

cdef cppclass CyclusAgentShim "CyclusAgentShim" (cpp_cyclus.Agent):
    # Agent interface
    CyclusAgentShim(cpp_cyclus.Context*)
    # Extra interface


cdef class _Agent(lib._Agent):
    pass  # pointer declared on full backend

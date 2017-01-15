"""A Python interface for Agents and their subclasses."""
from __future__ import print_function, unicode_literals
from libcpp.map cimport map as std_map
from libcpp.set cimport set as std_set
from libcpp.vector cimport vector as std_vector
from libcpp.utility cimport pair as std_pair
from libcpp.string cimport string as std_string
from libcpp cimport bool as cpp_bool

from cpython cimport (PyObject, PyDict_New, PyDict_Contains,
    PyDict_GetItemString, PyDict_SetItemString, PyString_FromString,
    PyBytes_FromString, PyDict_GetItem, PyDict_SetItem)

from cyclus cimport cpp_cyclus
from cyclus cimport lib
from cyclus import lib

from cyclus.typesystem cimport (py_to_any, any_to_py, str_py_to_cpp,
    std_string_to_py, bool_to_py, bool_to_cpp, std_set_std_string_to_py,
    std_set_std_string_to_cpp)
from cyclus cimport typesystem as ts
from cyclus import typesystem as ts

# startup numpy
#cimport numpy as np
#import numpy as np
#import pandas as pd

#np.import_array()
#np.import_ufunc()


cdef cppclass CyclusAgentShim "CyclusAgentShim" (cpp_cyclus.Agent):
    # A C++ class that acts as an Agent. It implements the Agent virtual
    # methods and dispatches the work to a Python/Cython object
    # that is has a reference to, as needed.

    CyclusAgentShim(cpp_cyclus.Context* ctx):  # C++BASES cyclus::Agent(ctx)
        pass

    #std_string version():
    #    rtn = self.version
    #    return str_py_to_cpp(py_rtn)


cdef class _Agent(lib._Agent):

    def __cinit__(self, lib._Context ctx):
        self.ptx = self.shim = new CyclusAgentShim(ctx.ptx)
        self._free = True
        self.shim.self = <PyObject*> self


class Agent(_Agent, lib.Agent):
    """
    """

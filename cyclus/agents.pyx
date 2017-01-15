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

from copy import deepcopy

from cyclus cimport cpp_cyclus
from cyclus cimport lib
from cyclus import lib

from cyclus.typesystem cimport (py_to_any, any_to_py, str_py_to_cpp,
    std_string_to_py, bool_to_py, bool_to_cpp, std_set_std_string_to_py,
    std_set_std_string_to_cpp)
from cyclus cimport typesystem as ts
from cyclus import typesystem as ts

from cyclus.cycpp import VarDeclarationFilter


_VAR_DECL = VarDeclarationFilter()

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

    std_string version():
        rtn = (<object> this.self).version
        return str_py_to_cpp(rtn)

    cpp_cyclus.Agent* Clone():
        cdef lib._Context ctx = lib.Context(init=False)
        (<lib._Context> ctx).ptx = this.context()
        cdef _Agent a = type(<object> this.self)(ctx)
        a.shim.InitFromAgent(this)
        return a.shim

    void InitFromAgent "InitFrom" (CyclusAgentShim* a):
        cpp_cyclus.Agent.InitFromAgent(a)
        (<object> a.self)._init_from_agent(<object> this.self)

    #void InfileToDb(cpp_cyclus.InfileTree*, cpp_cyclus.DbInit)
    #void InitFrom(cpp_cyclus.QueryableBackend*)

    void Snapshot(cpp_cyclus.DbInit di):
        pass

    void InitInv(cpp_cyclus.Inventories& inv):
        pass

    cpp_cyclus.Inventories SnapshotInv():
        cdef cpp_cyclus.Inventories inv = cpp_cyclus.Inventories()
        return inv


cdef class _Agent(lib._Agent):

    def __cinit__(self, lib._Context ctx):
        self.ptx = self.shim = new CyclusAgentShim(ctx.ptx)
        self._free = True
        self.shim.self = <PyObject*> self


class Agent(_Agent, lib.Agent):
    """Python Agent that is subclassable into any kind of agent.

    Parameters
    ----------
    ctx : cyclus.lib.Context
        The simulation execution context.  You don't normally
        need to call the initilaizer.
    """
    _statevars = None

    def __init__(self, ctx):
        super().__init__(ctx)
        # gather the state variables
        cls = type(self)
        if cls._statevars is None:
            cls._init_statevars()
        # copy state vars from class
        svs = []
        for name, var in cls._statevars:
            var = var.copy()
            self.__dict__[name] = var
            svs.append((name, var))
        self._statevars = tuple(svs)

    @classmethod
    def _init_statevars(cls):
        cdef dict vars = {}
        for name in dir(cls):
            attr = getattr(cls, name)
            if not isinstance(attr, ts.StateVar):
                continue
            attr.alias = _VAR_DECL.canonize_alias(attr.type, name, attr.alias)
            vars[name] = attr
        # make sure all state vars have a consistent index
        cdef int nvars = len(vars)
        cdef list svs = [None] * nvars
        cdef list left = []
        for name, var in vars.items():
            if var.index is None:
                left.append((name, var))
            else:
                if svs[var.index] is not None:
                    raise ValueError("two state varaiables cannot have the "
                                     "same index: " + name + " & " +
                                     svs[var.index][0])
                svs[var.index] = (name, var)
        left = sorted(left, reverse=True)
        cdef int i
        for i in range(nvars):
            if svs[i] is None:
                svs[i] = left.pop()
                svs[i][1].index = i
        cls._statevars = tuple(svs)

    def _init_from_agent(self, other):
        for name, var in self._statevars:
            setattr(self, name, deepcopy(getattr(other, name, None)))

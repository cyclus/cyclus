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
from cyclus cimport cpp_typesystem
from cyclus.typesystem cimport (py_to_any, any_to_py, str_py_to_cpp,
    std_string_to_py, bool_to_py, bool_to_cpp, std_set_std_string_to_py,
    std_set_std_string_to_cpp)
from cyclus cimport typesystem as ts
from cyclus import typesystem as ts

from cyclus import nucname

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
        (<object> a.self).init_from_agent(<object> this.self)

    void InfileToDb(cpp_cyclus.InfileTree* tree, cpp_cyclus.DbInit di):
        cpp_cyclus.Agent.InfileToDb(tree, di)
        cdef lib._InfileTree py_tree = lib.InfileTree(free=False)
        py_tree.ptx = tree
        cdef lib._DbInit py_di = lib.DbInit(free=False)
        py_di.ptx = &di
        (<object> this.self).infile_to_db(py_tree, py_di)


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
            attr.tooltip = _VAR_DECL.canonize_tooltip(attr.type, name, attr.tooltip)
            attr.uilabel = _VAR_DECL.canonize_uilabel(attr.type, name, attr.uilabel)
            attr.uitype = ts.prepare_type_representation(attr.uitype)
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

    def init_from_agent(self, other):
        """A dynamic version of InitFrom(Agent) that should work for all
        subclasses. Users should not need to call this ever. However, brave
        users may choose to override it in exceptional cases.
        """
        for name, var in self._statevars:
            setattr(self, name, deepcopy(getattr(other, name, None)))

    def infile_to_db(self, tree, di):
        """A dynamic version of InfileToDb(InfileTree*, DbInit) that should
        work for all subclasses. Users should not need to call this ever.
        However, brave users may choose to override it in exceptional cases.
        """
        sub = tree.subtree("config/*")
        for name, var in self._statevars:
            # set internal state variable values to default
            if var.internal:
                if var.default is None:
                    raise RuntimeError('state variables marked as internal '
                                       'must have a default')
                var.value = var.default
                continue
            query = var.alias if isinstance(var.alias, str) else var.alias[0]
            if var.default is not None and tree.nmatches(query) == 0:
                var.value = var.default
                continue
            else:
                var.value = self._read_from_infile(sub, name, var.alias,
                                                   var.type, var.uitype)
        # Write out to db
        datum = di.new_datum("Info")
        for name, var in self._statevars:
            datum.add_val(name, var.value, shape=var.shape, type=var.uniquetypeid)
        datum.record()

    def _query_infile(self, tree, query, type, uitype, idx=0):
        if uitype == 'nuclide':
            rtn = tree.query(query, cpp_typesystem.STRING, idx)
            rtn = nucname.id(rtn)
        else:
            rtn = tree.query(query, type, idx)
        return rtn

    def _read_from_infile(self, tree, alias, type, uitype, idx=0, path=''):
        if isinstance(type, str):
            # read primitive
            rtn = self._query_infile(tree, path + alias, type, uitype, idx)
        else:
            type0 = type[0]
            sub = tree.subtree(alias[0], idx)
            if type0 == 'std::vector':
                rtn = self._vector_from_infile(sub, alias, type, uitype, idx, path)
            elif type0 == 'std::set':
                rtn = self._set_from_infile(sub, alias, type, uitype, idx, path)
            elif type0 == 'std::list':
                # note we can reuse vector implementation
                rtn = self._vector_from_infile(sub, alias, type, uitype, idx, path)
            elif type0 == 'std::pair':
                rtn = self._pair_from_infile(sub, alias, type, uitype, idx, path)
            elif type0 == 'std::map':
                rtn = self._map_from_infile(sub, alias, type, uitype, idx, path)
            else:
                raise TypeError('type not recognized while reading input file: '
                                + type0 + ' of ' + repr(type) + ' in ' + repr(alias))
        return rtn

    def _vector_from_infile(self, tree, alias, type, uitype, idx=0, path=''):
        cdef int i, n
        cdef list rtn = []
        # get val name
        if alias[1] is None:
            val = 'val'
        elif isinstance(alias[1], str):
            val = alias[1]
        else:
            val = alias[1][0]
        # read in values.
        n = tree.nmatches(val)
        for i in range(n):
            x = self._read_from_infile(tree, val, type[1], uitype[1], i, path)
            rtn.append(x)
        return rtn

    def _set_from_infile(self, tree, alias, type, uitype, idx=0, path=''):
        rtn = self._vector_from_infile(tree, alias, type, uitype, idx)
        rtn = set(rtn)
        return rtn

    def _pair_from_infile(self, tree, alias, type, uitype, idx=0, path=''):
        first = 'first' if alias[1] is None else alias[1]
        second = 'second' if alias[2] is None else alias[2]
        x = self._read_from_infile(tree, first, type[1], uitype[1], idx, path)
        y = self._read_from_infile(tree, second, type[2], uitype[2], idx, path)
        return (x, y)

    def _map_from_infile(self, tree, alias, type, uitype, idx=0, path=''):
        cdef int i, n
        cdef dict rtn = {}
        # get query names
        if isinstance(alias[0], str):
            item = 'item'
        elif alias[0][1] is None:
            item = 'item'
        else:
            item = alias[0][1]
        key = 'key' if alias[1] is None else alias[1]
        val = 'val' if alias[2] is None else alias[2]
        # read in values.
        itempath = item + '/'
        n = tree.nmatches(item)
        for i in range(n):
            k = self._read_from_infile(tree, key, type[1], uitype[1], i, itempath)
            v = self._read_from_infile(tree, val, type[2], uitype[2], i, itempath)
            rtn[k] = v
        return rtn

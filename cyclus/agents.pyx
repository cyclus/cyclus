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
from cyclus.cpp_cyclus cimport shared_ptr
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
        # check the kind so we don't need to override in subclasses.
        #cdef std_string k = this.kind()
        #if k == b"Region":
        #    cpp_cyclus.Region.InfileToDb(tree, di)
        #elif k == std_string("Inst"):
        #    cpp_cyclus.Institution.InfileToDb(tree, di)
        #elif k == std_string("Facility"):
        #    cpp_cyclus.Facility.InfileToDb(tree, di)
        #else:
        #    cpp_cyclus.Agent.InfileToDb(tree, di)
        cpp_cyclus.Agent.InfileToDb(tree, di)
        # wrap interface
        cdef lib._InfileTree py_tree = lib.InfileTree(free=False)
        py_tree.ptx = tree
        cdef lib._DbInit py_di = lib.DbInit(free=False)
        py_di.ptx = &di
        # call generic python
        (<object> this.self).infile_to_db(py_tree, py_di)

    void InitFrom(cpp_cyclus.QueryableBackend* b):
        # check the kind so we don't need to override in subclasses.
        #cdef std_string k = this.kind()
        #if k == std_string("Region"):
        #    cpp_cyclus.Region.InitFrom(b)
        #elif k == std_string("Inst"):
        #    cpp_cyclus.Institution.InitFrom(b)
        #elif k == std_string("Facility"):
        #    cpp_cyclus.Facility.InitFrom(b)
        #else:
        #    cpp_cyclus.Agent.InitFrom(b)
        cpp_cyclus.Agent.InitFrom(b)
        cdef cpp_cyclus.QueryResult qr = b.Query(std_string(<char*> "Info"), NULL)
        res, _ = lib.query_result_to_py(qr)
        # call generic python
        (<object> this.self).init_from_dict(res)

    void Snapshot(cpp_cyclus.DbInit di):
        cdef lib._DbInit py_di = lib.DbInit(free=False)
        py_di.ptx = &di
        (<object> this.self).snapshot(py_di)

    void InitInv(cpp_cyclus.Inventories& invs):
        cdef dict pyinvs = {}
        cdef list value
        cdef ts._Resource r
        for name_value in invs:
            name = std_string_to_py(name_value.first)
            value = []
            for x in name_value.second:
                r = ts.Resource()
                r.ptx = x
                value.append(r)
            pyinvs[name] = value
        (<object> this.self).init_inv(pyinvs)

    cpp_cyclus.Inventories SnapshotInv():
        pyinvs = (<object> this.self).snapshot_inv()
        cdef cpp_cyclus.Inventories invs = cpp_cyclus.Inventories()
        cdef std_string name
        cdef std_vector[shared_ptr[cpp_cyclus.Resource]] value
        for pyname, pyvalue in pyinvs.items():
            name = str_py_to_cpp(pyname)
            value = std_vector[shared_ptr[cpp_cyclus.Resource]]()
            for r in pyvalue:
                value.push_back((<ts._Resource> r).ptx)
        return invs


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
    _inventories = None

    def __init__(self, ctx):
        super().__init__(ctx)
        # gather the state variables and inventories
        cls = type(self)
        if cls._statevars is None:
            cls._init_statevars()
        if cls._inventories is None:
            cls._init_inventories()
        # copy state vars from class
        cdef list svs = []
        for name, var in cls._statevars:
            var = var.copy()
            self.__dict__[name] = var
            svs.append((name, var))
        self._statevars = tuple(svs)
        # copy and init inventories from class
        cdef list invs = []
        for name, inv in cls._inventories:
            inv = inv.copy()
            inv._init()
            self.__dict__[name] = inv
            inv.append((name, inv))
        self._inventories = tuple(invs)

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
        cls._statevars = index_and_sort_vars(vars)

    @classmethod
    def _init_inventories(cls):
        cdef dict invs = {}
        for name in dir(cls):
            attr = getattr(cls, name)
            if not isinstance(attr, ts.Inventory):
                continue
            invs[name] = attr
        cls._inventories = index_and_sort_vars(vars)

    def init_from_agent(self, other):
        """A dynamic version of InitFrom(Agent) that should work for all
        subclasses. Users should not need to call this ever. However, brave
        users may choose to override it in exceptional cases.
        """
        for name, var in self._statevars:
            setattr(self, name, deepcopy(getattr(other, name, None)))
        for name, inv in self._inventories:
            inv.value.capacity = getattr(other, name).capacity

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

    def init_from_dict(self, d):
        """An initializer that reads state varaible values from a dictionary.
        This is used when InitFrom(QueryableBackend) is called.
        Users should not need to call this ever. However, brave
        users may choose to override it in exceptional cases.
        """
        for name, val in d.items():
            setattr(self, name, val)
        for name, inv in self._inventories:
            inv.value.capacity = d[inv.capacity]

    def snapshot(self, di):
        """A dynamic version of Snapshot(DbInit) that should
        work for all subclasses.
        Users should not need to call this ever. However, brave
        users may choose to override it in exceptional cases.
        """
        datum = di.new_datum("Info")
        for name, var in self._statevars:
            datum.add_val(name, var.value, shape=var.shape, type=var.uniquetypeid)
        datum.record()

    def init_inv(self, invs):
        """An initializer that sets up inventories.
        This is used when InitInv(Inventories&) is called.
        Users should not need to call this ever. However, brave
        users may choose to override it in exceptional cases.
        """
        for name, inv in self._inventories:
            if name not in invs:
                continue
            inv.value.push_many(invs[name])

    def snapshot_inv(self):
        """A dynamic version of SnapshotInv() that reports the current
        state of the inventories.
        Users should not need to call this ever. However, brave
        users may choose to override it in exceptional cases.
        """
        cdef dict invs = {}
        for name, inv in self._inventories:
            invs[name] = inv.value.pop_all_res()
            inv.value.push_many(invs[name])
        return invs
#
# Tools
#
cdef tuple index_and_sort_vars(dict vars):
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
    rtn = tuple(svs)
    return rtn

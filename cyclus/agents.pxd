"""A Python interface for Agents and their subclasses."""
from libcpp.map cimport map as std_map
from libcpp.set cimport set as std_set
from libcpp.vector cimport vector as std_vector
from libcpp.utility cimport pair as std_pair
from libcpp.string cimport string as std_string
from libcpp cimport bool as cpp_bool

from cpython cimport PyObject

from cyclus cimport cpp_jsoncpp
from cyclus cimport cpp_cyclus
from cyclus cimport lib

cdef cppclass CyclusAgentShim "CyclusAgentShim" (cpp_cyclus.Agent):  # C++CONSTRUCTORS CyclusAgentShim(cyclus::Context*)
    # Agent interface
    CyclusAgentShim(cpp_cyclus.Context*)
    std_string version()
    cpp_cyclus.Agent* Clone()
    void InfileToDb(cpp_cyclus.InfileTree*, cpp_cyclus.DbInit)
    void InitFromAgent "InitFrom" (CyclusAgentShim*)
    void InitFrom(cpp_cyclus.QueryableBackend*)
    void Snapshot(cpp_cyclus.DbInit)
    void InitInv(cpp_cyclus.Inventories&)
    cpp_cyclus.Inventories SnapshotInv()
    std_string schema()
    cpp_jsoncpp.Value annotations()
    # Extra interface
    PyObject* self  # the Python object we are shimming


ctypedef CyclusAgentShim* agent_shim_ptr

cdef class _Agent(lib._Agent):
    # pointer declared on full backend, but that is untyped, shim is typed
    cdef agent_shim_ptr shim

cdef tuple index_and_sort_vars(dict)
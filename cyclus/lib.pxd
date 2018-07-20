"""Python wrapper for cyclus."""
# Cython imports
from libcpp.utility cimport pair as cpp_pair
from libcpp.set cimport set as cpp_set
from libcpp.map cimport map as cpp_map
from libcpp.vector cimport vector as cpp_vector
from libcpp.string cimport string as std_string
from cython.operator cimport dereference as deref
from cython.operator cimport preincrement as inc
from libc.stdlib cimport malloc, free
from libcpp cimport bool as cpp_bool
from cpython cimport PyObject

# local imports
from cyclus cimport cpp_jsoncpp
from cyclus cimport cpp_cyclus
from cyclus.cpp_stringstream cimport stringstream

ctypedef cpp_cyclus.Agent* agent_ptr
ctypedef cpp_cyclus.Region* region_ptr
ctypedef cpp_cyclus.Institution* institution_ptr
ctypedef cpp_cyclus.Facility* facility_ptr
cdef cpp_cyclus.Agent* dynamic_agent_ptr(object)

cdef class _Datum:
    cdef void * ptx
    cdef bint _free
    cdef list _fieldnames

cdef object query_result_to_py(cpp_cyclus.QueryResult)
cdef object single_query_result_to_py(cpp_cyclus.QueryResult qr, int row)

cdef class _FullBackend:
    cdef void * ptx

cdef class _SqliteBack(_FullBackend):
    pass

cdef class _Hdf5Back(_FullBackend):
    pass

cdef class _Recorder:
    cdef void * ptx

cdef class _AgentSpec:
    cdef cpp_cyclus.AgentSpec * ptx

cdef class _DynamicModule:
    cdef cpp_cyclus.DynamicModule * ptx

cdef class _Env:
    pass

cdef class _Logger:
    # everything that we use on this class is static
    pass

cdef class _XMLParser:
    cdef cpp_cyclus.XMLParser * ptx

cdef class _InfileTree:
    # if InfileTree ever has C++ subclasses that we then also want
    # to wrap in Cython, the pointer will need to become
    # cdef void * ptx, and we'll need to cast each access.
    # For now, there are no subclasses  so we can get away with
    # the fully typed pointer.
    cdef cpp_cyclus.InfileTree * ptx
    cdef bint _free

cdef class _DbInit:
    cdef cpp_cyclus.DbInit * ptx
    cdef bint _free

cdef class _Timer:
    cdef cpp_cyclus.Timer * ptx
    cdef bint _free

cdef class _SimInit:
    cdef cpp_cyclus.SimInit * ptx

cpdef object capsule_agent_to_py(object agent, object ctx)
cdef object agent_to_py(cpp_cyclus.Agent* a_ptr, object ctx)
cdef dict inventories_to_py(cpp_cyclus.Inventories& invs)
cdef cpp_cyclus.Inventories inventories_to_cpp(object pyinvs)

cdef class _Agent:
    cdef void * ptx
    cdef bint _free
    cdef object _context

cdef class _XMLFileLoader:
    cdef cpp_cyclus.XMLFileLoader * ptx

cdef class _XMLFlatLoader:
    cdef cpp_cyclus.XMLFlatLoader * ptx

cdef class _Context:
    cdef cpp_cyclus.Context * ptx
    cdef bint _free

cdef class _ColumnInfo:
    cdef cpp_cyclus.ColumnInfo * ptx
    cdef void copy_from(_ColumnInfo, cpp_cyclus.ColumnInfo)


#
# Position
#
cdef class _Position:
    cdef cpp_cyclus.Position* posptx

#
# Tools
#
cdef cpp_jsoncpp.Value str_to_json_value(object pyanno)
cpdef dict normalize_request_portfolio(object inp)
cdef dict _AGENT_REFS
cpdef object make_py_agent(object libname, object agentname, object ctx)
cpdef void _clear_agent_refs()
cpdef void _del_agent(int i)

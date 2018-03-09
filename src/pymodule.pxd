"""Header for Cyclus Python Input Files."""
from libcpp.string cimport string as std_string
from libcpp.typeinfo cimport type_info
from cpython.pycapsule cimport PyCapsule_New, PyCapsule_GetPointer

cdef extern from "cyclus.h" namespace "boost::spirit":

    cdef cppclass hold_any:
        hold_any() except +
        hold_any(const char*) except +
        hold_any assign[T](T) except +
        T cast[T]() except +
        const type_info& type() except +

cdef extern from "agent.h" namespace "cyclus":

    cdef cppclass Agent

ctypedef Agent* agent_ptr

cdef extern from "region.h" namespace "cyclus":

    cdef cppclass Region

ctypedef Region* region_ptr

cdef extern from "institution.h" namespace "cyclus":

    cdef cppclass Institution

ctypedef Institution* institution_ptr

cdef extern from "facility.h" namespace "cyclus":

    cdef cppclass Facility

ctypedef Facility* facility_ptr

cdef extern from "toolkit/timeseries.h" namespace "cyclus::toolkit":

    cdef enum TimeSeriesType:
        POWER
        ENRICH_SWU
        ENRICH_FEED

cdef std_string str_py_to_cpp(object x)
cdef object std_string_to_py(std_string x)


cdef public std_string py_find_module "CyclusPyFindModule" (std_string cpp_lib) except +

cdef public Agent* make_py_agent "CyclusMakePyAgent" (std_string cpp_lib,
                                                      std_string cpp_agent,
                                                      void* cpp_ctx) except +

cdef public void init_from_py_agent "CyclusInitFromPyAgent" (Agent* cpp_src,
                                                             Agent* cpp_dst,
                                                             void* cpp_ctx) except +

cdef public void clear_pyagent_refs "CyclusClearPyAgentRefs" () except +
cdef public void py_del_agent "CyclusPyDelAgent" (int i) except +

cdef public void py_call_listeners "CyclusPyCallListeners" (std_string cpp_tsname,
                            Agent* cpp_agent, void* cpp_ctx, int time, hold_any cpp_value) except +

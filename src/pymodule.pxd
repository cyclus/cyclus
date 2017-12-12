"""Header for Cyclus Python Input Files."""
from libcpp.string cimport string as std_string
from cpython.pycapsule cimport PyCapsule_New, PyCapsule_GetPointer

cdef extern from "agent.h"namespace "cyclus":

    cdef cppclass Agent

ctypedef Agent* agent_ptr

cdef extern from "region.h"namespace "cyclus":

    cdef cppclass Region

ctypedef Region* region_ptr

cdef extern from "institution.h"namespace "cyclus":

    cdef cppclass Institution

ctypedef Institution* institution_ptr

cdef extern from "facility.h"namespace "cyclus":

    cdef cppclass Facility

ctypedef Facility* facility_ptr

cdef std_string str_py_to_cpp(object x)
cdef object std_string_to_py(std_string x)


cdef public std_string py_find_module "CyclusPyFindModule" (std_string cpp_lib)

cdef public Agent* make_py_agent "CyclusMakePyAgent" (std_string cpp_lib,
                                                      std_string cpp_agent,
                                                      void* cpp_ctx)

cdef public void clear_pyagent_refs "CyclusClearPyAgentRefs" ()
cdef public void py_del_agent "CyclusPyDelAgent" (int i)

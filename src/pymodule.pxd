"""Header for Cyclus Python Input Files."""
from libcpp.string cimport string as std_string
from cpython.pycapsule cimport PyCapsule_New, PyCapsule_GetPointer

cdef std_string str_py_to_cpp(object x)
cdef object std_string_to_py(std_string x)


cdef public std_string py_find_module "CyclusPyFindModule" (std_string cpp_lib)

cdef list AGENT_REFS

cdef public void* make_py_agent "CyclusMakePyAgent" (std_string cpp_lib,
                                                     std_string cpp_agent,
                                                     void* cpp_ctx)

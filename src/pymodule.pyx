"""Cyclus Python module loading tools."""
from __future__ import print_function, unicode_literals
from libcpp.cast cimport reinterpret_cast
from libcpp.string cimport string as std_string
from cpython.pycapsule cimport PyCapsule_New, PyCapsule_GetPointer

from importlib import import_module

import cyclus.lib as cyclib

cdef object std_string_to_py(std_string x):
    pyx = x
    pyx = pyx.decode()
    return pyx


cdef std_string str_py_to_cpp(object x):
    cdef std_string s
    x = x.encode()
    s = std_string(<const char*> x)
    return s



cdef public std_string py_find_module "CyclusPyFindModule" (std_string cpp_lib):
    """Finds a Python module and returns a string of the form '<py>:modulepath'"""
    lib = std_string_to_py(cpp_lib)
    try:
        mod = import_module(lib)
    except ImportError:
        return std_string()
    cdef std_string rtn = str_py_to_cpp(mod.__file__)
    return rtn


cdef list AGENT_REFS = []

cdef public void* make_py_agent "CyclusMakePyAgent" (std_string cpp_lib,
                                                     std_string cpp_agent,
                                                     void* cpp_ctx):
    """Makes a new Python agent instance."""
    libname = std_string_to_py(cpp_lib)
    agentname = std_string_to_py(cpp_agent)
    #mod = import_module(lib)
    #cls = getattr(mod, name)
    #ctx = cyclib.Context()
    #(<cyclib._Context> ctx).ptx = <Context*> cpp_ctx
    #agent = cls(ctx)
    #return (<cyclib._Agent> agent).ptx
    ctx = PyCapsule_New(cpp_ctx, <char*> b"ctx", NULL)
    a = cyclib.make_py_agent(libname, agentname, ctx)
    return PyCapsule_GetPointer(a, <char*> b"agent")


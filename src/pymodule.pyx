"""Cyclus Python module loading tools."""
from __future__ import print_function, unicode_literals
from libcpp.cast cimport reinterpret_cast, dynamic_cast
from libcpp.string cimport string as std_string
from cpython.exc cimport PyErr_CheckSignals
from cpython.pycapsule cimport PyCapsule_New, PyCapsule_GetPointer

from importlib import import_module

import cyclus.lib as cyclib
import cyclus.typesystem as ts

cdef object std_string_to_py(std_string x):
    pyx = x
    pyx = pyx.decode()
    return pyx


cdef std_string str_py_to_cpp(object x):
    cdef std_string s
    x = x.encode()
    s = std_string(<const char*> x)
    return s


cdef public std_string py_find_module "CyclusPyFindModule" (std_string cpp_lib) except +:
    """Finds a Python module and returns a string of the form '<py>:modulepath'"""
    lib = std_string_to_py(cpp_lib)
    try:
        mod = import_module(lib)
    except ImportError:
        return std_string(b"")
    PyErr_CheckSignals()
    cdef std_string rtn = str_py_to_cpp("<py>" + mod.__file__)
    return rtn


cdef public Agent* make_py_agent "CyclusMakePyAgent" (std_string cpp_lib,
                                                     std_string cpp_agent,
                                                     void* cpp_ctx) except +:
    """Makes a new Python agent instance."""
    libname = std_string_to_py(cpp_lib)
    agentname = std_string_to_py(cpp_agent)
    ctx = PyCapsule_New(cpp_ctx, <char*> b"ctx", NULL)
    a, kind = cyclib.make_py_agent(libname, agentname, ctx)
    cdef void* avoid = PyCapsule_GetPointer(a, <char*> b"agent")
    cdef Agent* rtn
    if kind == "Region":
        rtn = dynamic_cast[agent_ptr](reinterpret_cast[region_ptr](avoid))
    elif kind == "Inst":
        rtn = dynamic_cast[agent_ptr](reinterpret_cast[institution_ptr](avoid))
    elif kind == "Facility":
        rtn = dynamic_cast[agent_ptr](reinterpret_cast[facility_ptr](avoid))
    else:
        rtn = dynamic_cast[agent_ptr](reinterpret_cast[agent_ptr](avoid))
    PyErr_CheckSignals()
    return rtn


cdef public void init_from_py_agent "CyclusInitFromPyAgent" (Agent* cpp_src,
                                                             Agent* cpp_dst,
                                                             void* cpp_ctx) except +:
    """Initializes the dst agent with the settings from the src. Users will not
    normally need to call this. Useful for cloning prototypes.
    """
    ctx = PyCapsule_New(cpp_ctx, <char*> b"ctx", NULL)
    src = PyCapsule_New(cpp_src, <char*> b"agent", NULL)
    dst = PyCapsule_New(cpp_dst, <char*> b"agent", NULL)
    py_src = cyclib.capsule_agent_to_py(src, ctx)
    py_dst = cyclib.capsule_agent_to_py(dst, ctx)
    py_dst.init_from_agent(py_src)
    PyErr_CheckSignals()


cdef public void clear_pyagent_refs "CyclusClearPyAgentRefs" () except +:
    """Clears the cache of agent referencess"""
    cyclib._clear_agent_refs()
    PyErr_CheckSignals()


cdef public void py_del_agent "CyclusPyDelAgent" (int i) except +:
    """Clears the cache of a single agent ref"""
    cyclib._del_agent(i)
    PyErr_CheckSignals()


cdef public void py_call_listeners "CyclusPyCallListeners" (std_string cpp_tsname,
                            Agent* cpp_agent, void* cpp_ctx, int time, hold_any cpp_value) except +:
    """Calls the python time series listeners
    """
    ctx = PyCapsule_New(cpp_ctx, <char*> b"ctx", NULL)
    agent = PyCapsule_New(cpp_agent, <char*> b"agent", NULL)
    value = PyCapsule_New(&cpp_value, <char*> b"value", NULL)
    py_tsname = std_string_to_py(cpp_tsname)
    py_agent = cyclib.capsule_agent_to_py(agent, ctx)
    py_value = ts.capsule_any_to_py(value)
    cyclib.call_listeners(py_tsname, py_agent, time, py_value)
    PyErr_CheckSignals()

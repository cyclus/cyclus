"""Cyclus Python input file tools."""
from __future__ import print_function, unicode_literals
from libcpp.string cimport string as std_string

try:
    from pprintpp import pformat
except ImportError:
    from pprint import pformat


cdef object std_string_to_py(std_string x):
    pyx = x
    pyx = pyx.decode()
    return pyx


cdef std_string str_py_to_cpp(object x):
    cdef std_string s
    x = x.encode()
    s = std_string(<const char*> x)
    return s


cdef public std_string py_to_json "CyclusPyToJson" (std_string cpp_infile):
    """Converts a Python file to JSON"""
    infile = std_string_to_py(cpp_infile)
    if not infile.endswith('\n'):
        infile += '\n'
    cpdef dict ctx = {}
    exec(infile, ctx, ctx)
    names = ('simulation', 'SIMULATION', 'Simulation')
    for name in names:
        if name in ctx:
            sim = ctx[name]
            break
    else:
        raise RuntimeError('simulation not found in python file.')
    if callable(sim):
        sim = sim()
    from collections import Mapping
    if isinstance(sim, str):
        pass  # assume in JSON format
    elif isinstance(sim, bytes):
        sim = sim.decode()  # assume in JSON format, get into str
    elif isinstance(sim, Mapping):
        import json
        sim = json.dumps(sim, sort_keys=True, indent=1)
    else:
        raise RuntimeError('top-level simulation object does not have proper type.')
    cdef std_string cpp_rtn = str_py_to_cpp(sim)
    return cpp_rtn


cdef public std_string json_to_py "CyclusJsonToPy" (std_string cpp_infile):
    """Converts a JSON file to Python"""
    infile = std_string_to_py(cpp_infile)
    import json
    sim = json.loads(infile)
    s = 'SIMULATION = ' + pformat(sim, indent=1) + '\n'
    cdef std_string cpp_rtn = str_py_to_cpp(s)
    return cpp_rtn

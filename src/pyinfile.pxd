"""Header for Cyclus Python Input Files."""
from libcpp.string cimport string as std_string

cdef std_string str_py_to_cpp(object x)
cdef object std_string_to_py(std_string x)

cdef public std_string py_to_json "CyclusPyToJson" (std_string)
cdef public std_string json_to_py "CyclusJsonToPy" (std_string)

"""C++ header wrapper for specific parts of cyclus."""
from libcpp.map cimport map
from libcpp.set cimport set
from libcpp.vector cimport vector
from libcpp.utility cimport pair
from libcpp.string cimport string as std_string
from libcpp cimport bool as cpp_bool

from cpp_typesystem cimport DbTypes


cdef extern from "cyclus.h" namespace "boost::spirit":

    cdef cppclass hold_any:
        hold_any() except +
        hold_any(const char*) except +
        hold_any assign[T](T) except +
        T cast[T]() except +

cdef extern from "cyclus.h" namespace "boost::uuids":

    cdef cppclass uuid:
        unsigned char data[16]


cdef extern from "cyclus.h" namespace "cyclus":

    cdef cppclass Datum:
        Datum* AddVal(const char*, hold_any) except +
        Datum* AddVal(const char*, hold_any, vector[int]*) except +
        void Record() except +
        std_string title() except +
        #vector[pair[char*, hold_any]] vals() except +
        vector[vector[int]] shapes() except +


cdef extern from "rec_backend.h" namespace "cyclus":

    ctypedef vector[Datum*] DatumList

    cdef cppclass RecBackend:
        void Notify(DatumList) except +
        std_string Name() except +
        void Flush()  except +
        void Close() except +


cdef extern from "cyclus.h" namespace "cyclus":

    ctypedef vector[hold_any] QueryRow

    cdef enum CmpOpCode:
        LT
        GT
        LE
        GE
        EQ
        NE

    cdef cppclass Blob:
        Blob() except +
        Blob(std_string) except +
        const std_string str() except +

    cdef cppclass Cond:
        Cond() except +
        Cond(std_string, std_string, hold_any) except +
    
        std_string field
        std_string op
        CmpOpCode opcode
        hold_any val

    cdef cppclass QueryResult:
        QueryResult() except +

        void Reset() except +
        T GetVal[T](std_string) except +
        T GetVal[T](std_string, int) except +

        vector[std_string] fields
        vector[DbTypes] types
        vector[QueryRow] rows

    cdef cppclass QueryableBackend:
        QueryResult Query(std_string, vector[Cond]*) except +
        map[std_string, DbTypes] ColumnTypes(std_string) except +
        set[std_string] Tables() except +

    cdef cppclass FullBackend(QueryableBackend, RecBackend):
        FullBackend() except +

    cdef cppclass Recorder:
        Recorder() except +
        Recorder(cpp_bool) except +

        unsigned int dump_count() except +
        void set_dump_count(unsigned int) except +
        cpp_bool inject_sim_id() except +
        void inject_sim_id(cpp_bool) except +
        uuid sim_id() except +
        Datum* NewDatum(std_string) except +
        void RegisterBackend(RecBackend*) except +
        void Flush() except +
        void Close() except +


cdef extern from "sqlite_back.h" namespace "cyclus":
    
    cdef cppclass SqliteBack(FullBackend):
        SqliteBack(std_string) except +


cdef extern from "hdf5_back.h" namespace "cyclus":

    cdef cppclass Hdf5Back(FullBackend):
        Hdf5Back(std_string) except +


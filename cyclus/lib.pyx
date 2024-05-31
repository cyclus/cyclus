"""Python wrapper for cyclus."""
from __future__ import division, unicode_literals, print_function
from cpython.pycapsule cimport PyCapsule_New, PyCapsule_GetPointer

# Cython imports
from libc.stdint cimport intptr_t
from libcpp.utility cimport pair as std_pair
from libcpp.set cimport set as std_set
from libcpp.map cimport map as std_map
from libcpp.vector cimport vector as std_vector
from libcpp.string cimport string as std_string
from libcpp.list cimport list as std_list
from cython.operator cimport dereference as deref
from cython.operator cimport preincrement as inc
from libc.stdlib cimport malloc, free
from libc.string cimport memcpy
from libcpp cimport bool as cpp_bool
from libcpp.cast cimport reinterpret_cast, dynamic_cast
from cpython cimport PyObject
from cpython.pycapsule cimport PyCapsule_GetPointer

from binascii import hexlify
import uuid
import os
from collections import defaultdict
from collections.abc import Mapping, Sequence, Iterable
from importlib import import_module

cimport numpy as np
import numpy as np
import pandas as pd

# local imports
from cyclus cimport cpp_jsoncpp
from cyclus cimport jsoncpp
from cyclus import jsoncpp

from cyclus cimport cpp_cyclus
from cyclus.cpp_cyclus cimport shared_ptr
from cyclus cimport cpp_typesystem
from cyclus cimport typesystem as ts
from cyclus import typesystem as ts
from cyclus.cpp_stringstream cimport stringstream
from cyclus.typesystem cimport py_to_any, db_to_py, uuid_cpp_to_py, \
    str_py_to_cpp, std_string_to_py, std_vector_std_string_to_py, \
    bool_to_py, bool_to_cpp, int_to_py, std_set_std_string_to_py, uuid_cpp_to_py, \
    std_vector_std_string_to_py, C_IDS, blob_to_bytes, std_vector_int_to_py


# startup numpy
np.import_array()
np.import_ufunc()


cdef cpp_cyclus.Agent* dynamic_agent_ptr(object a):
    """Dynamically casts an agent instance to the correct agent pointer"""
    if a is None:
        return NULL
    elif a.kind == "Region":
        return dynamic_cast[agent_ptr](
            reinterpret_cast[region_ptr]((<_Agent> a).ptx))
    elif a.kind == "Inst":
        return dynamic_cast[agent_ptr](
            reinterpret_cast[institution_ptr]((<_Agent> a).ptx))
    elif a.kind == "Facility":
        return dynamic_cast[agent_ptr](
            reinterpret_cast[facility_ptr]((<_Agent> a).ptx))
    else:
        return dynamic_cast[agent_ptr](
            reinterpret_cast[agent_ptr]((<_Agent> a).ptx))


cdef class _Datum:

    def __cinit__(self):
        """Constructor for Datum type conversion."""
        self._free = False
        self.ptx = NULL
        self._fields = []

    def __dealloc__(self):
        """Datum destructor."""
        if self.ptx == NULL:
            return
        cdef cpp_cyclus.Datum* cpp_ptx
        if self._free:
            cpp_ptx = <cpp_cyclus.Datum*> self.ptx
            del cpp_ptx
            self.ptx = NULL

    def add_val(self, field, value, shape=None, type=None):
        """Adds Datum value to current record as the corresponding cyclus data type.

        Parameters
        ----------
        field : str or bytes
            The column name.
        value : object
            Value in table column, optional
        shape : list or tuple of ints
            Length of value.
        type : dbtype or norm type
            Data type as defined by cyclus typesystem

        Returns
        -------
        self : Datum
        """
        cdef int i, n
        cdef std_vector[int] cpp_shape
        if type is None:
            raise TypeError('a database or C++ type must be supplied to add a '
                            'value to the datum, got None.')
        cdef cpp_cyclus.hold_any v = py_to_any(value, type)
        if isinstance(field, str):
            field = field.encode()
        elif isinstance(field, bytes):
            pass
        else:
            raise ValueError('field name must be str or bytes.')
        # have to keep refs around so don't dealloc field names
        self._fields.append(field)
        cdef char* cpp_field = <char*> field
        if shape is None:
            (<cpp_cyclus.Datum*> self.ptx).AddVal(cpp_field, v)
        else:
            n = len(shape)
            cpp_shape.resize(n)
            for i in range(n):
                cpp_shape[i] = <int> shape[i]
            (<cpp_cyclus.Datum*> self.ptx).AddVal(cpp_field, v, &cpp_shape)
        return self

    def record(self):
        """Records the Datum."""
        (<cpp_cyclus.Datum*> self.ptx).Record()
        self._fields.clear()

    @property
    def title(self):
        """The datum name."""
        s = (<cpp_cyclus.Datum*> self.ptx).title()
        return s


class Datum(_Datum):
    """Datum class."""


cdef object query_result_to_py(cpp_cyclus.QueryResult qr):
    """Converts a query result object to a dictionary mapping fields to value lists
    and a list of field names in order.
    """
    cdef int i, j
    cdef int nrows, ncols
    nrows = qr.rows.size()
    ncols = qr.fields.size()
    cdef dict res = {}
    cdef list fields = []
    for j in range(ncols):
        res[j] = []
        f = qr.fields[j]
        fields.append(f.decode())
    for i in range(nrows):
        for j in range(ncols):
            res[j].append(db_to_py(qr.rows[i][j], qr.types[j]))
    res = {fields[j]: v for j, v in res.items()}
    rtn = (res, fields)
    return rtn


cdef object single_query_result_to_py(cpp_cyclus.QueryResult qr, int row):
    """Converts a query result object with only one row to a dictionary mapping
    fields to values and a list of field names in order.
    """
    cdef int i, j
    cdef int nrows, ncols
    nrows = qr.rows.size()
    ncols = qr.fields.size()
    if nrows < row:
        raise ValueError("query result does not have enough rows!")
    cdef dict res = {}
    cdef list fields = []
    for j in range(ncols):
        f = qr.fields[j]
        fields.append(f.decode())
        res[fields[j]] = db_to_py(qr.rows[row][j], qr.types[j])
    rtn = (res, fields)
    return rtn


cdef class _FullBackend:

    def __cinit__(self):
        """Full backend C++ constructor"""
        self._tables = None

    def __dealloc__(self):
        """Full backend C++ destructor."""
        # Note that we have to do it this way since self.ptx is void*
        if self.ptx == NULL:
            return
        cdef cpp_cyclus.FullBackend * cpp_ptx = <cpp_cyclus.FullBackend *> self.ptx
        del cpp_ptx
        self.ptx = NULL

    def query(self, table, conds=None):
        """Queries a database table.

        Parameters
        ----------
        table : str
            The table name.
        conds : iterable, optional
            A list of conditions.

        Returns
        -------
        results : pd.DataFrame
            Pandas DataFrame the represents the table
        """
        cdef std_string tab = str(table).encode()
        cdef std_string field
        cdef cpp_cyclus.QueryResult qr
        cdef std_vector[cpp_cyclus.Cond] cpp_conds
        cdef std_vector[cpp_cyclus.Cond]* conds_ptx
        cdef std_map[std_string, cpp_cyclus.DbTypes] coltypes
        # set up the conditions
        if conds is None:
            conds_ptx = NULL
        else:
            coltypes = (<cpp_cyclus.FullBackend*> self.ptx).ColumnTypes(tab)
            for cond in conds:
                cond0 = cond[0].encode()
                cond1 = cond[1].encode()
                field = std_string(<const char*> cond0)
                if coltypes.count(field) == 0:
                    continue  # skips non-existent columns
                cpp_conds.push_back(cpp_cyclus.Cond(field, cond1,
                    py_to_any(cond[2], coltypes[field])))
            if cpp_conds.size() == 0:
                conds_ptx = NULL
            else:
                conds_ptx = &cpp_conds
        # query, convert, and return
        qr = (<cpp_cyclus.FullBackend*> self.ptx).Query(tab, conds_ptx)
        res, fields = query_result_to_py(qr)
        results = pd.DataFrame(res, columns=fields)
        return results

    def schema(self, table):
        cdef std_string ctable = str_py_to_cpp(table)
        cdef std_list[cpp_cyclus.ColumnInfo] cis = (<cpp_cyclus.QueryableBackend*> self.ptx).Schema(ctable)
        rtn = []
        for ci in cis:
            py_ci = ColumnInfo()
            (<_ColumnInfo> py_ci).copy_from(ci)
            rtn.append(py_ci)
        return rtn

    @property
    def tables(self):
        """Retrieves the set of tables present in the database."""
        if self._tables is not None:
            return self._tables
        cdef std_set[std_string] ctabs = \
            (<cpp_cyclus.FullBackend*> self.ptx).Tables()
        cdef std_set[std_string].iterator it = ctabs.begin()
        cdef set tabs = set()
        while it != ctabs.end():
            tab = deref(it)
            tabs.add(tab.decode())
            inc(it)
        self._tables = tabs
        return self._tables

    @tables.setter
    def tables(self, value):
        self._tables = value


class FullBackend(_FullBackend, object):
    """Full backend cyclus database interface."""

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.close()


cdef class _SqliteBack(_FullBackend):

    def __cinit__(self, path):
        """Full backend C++ constructor"""
        cdef std_string cpp_path = str(path).encode()
        self.ptx = new cpp_cyclus.SqliteBack(cpp_path)

    def __dealloc__(self):
        """Full backend C++ destructor."""
        # Note that we have to do it this way since self.ptx is void*
        if self.ptx == NULL:
            return
        cdef cpp_cyclus.SqliteBack * cpp_ptx = <cpp_cyclus.SqliteBack *> self.ptx
        del cpp_ptx
        self.ptx = NULL

    def flush(self):
        """Flushes the database to disk."""
        (<cpp_cyclus.SqliteBack*> self.ptx).Flush()

    def close(self):
        """Closes the backend, flushing it in the process."""
        self.flush()  # just in case
        (<cpp_cyclus.SqliteBack*> self.ptx).Close()

    @property
    def name(self):
        """The name of the database."""
        name = (<cpp_cyclus.SqliteBack*> self.ptx).Name()
        name = name.decode()
        return name


class SqliteBack(_SqliteBack, FullBackend):
    """SQLite backend cyclus database interface."""


cdef class _Hdf5Back(_FullBackend):

    def __cinit__(self, path):
        """Hdf5 backend C++ constructor"""
        cdef std_string cpp_path = str(path).encode()
        self.ptx = new cpp_cyclus.Hdf5Back(cpp_path)

    def __dealloc__(self):
        """Full backend C++ destructor."""
        # Note that we have to do it this way since self.ptx is void*
        if self.ptx == NULL:
            return
        cdef cpp_cyclus.Hdf5Back * cpp_ptx = <cpp_cyclus.Hdf5Back *> self.ptx
        del cpp_ptx
        self.ptx = NULL

    def flush(self):
        """Flushes the database to disk."""
        (<cpp_cyclus.Hdf5Back*> self.ptx).Flush()

    def close(self):
        """Closes the backend, flushing it in the process."""
        (<cpp_cyclus.Hdf5Back*> self.ptx).Close()

    @property
    def name(self):
        """The name of the database."""
        name = (<cpp_cyclus.Hdf5Back*> self.ptx).Name()
        name = name.decode()
        return name


class Hdf5Back(_Hdf5Back, FullBackend):
    """HDF5 backend cyclus database interface."""


cdef class _Recorder:

    def __cinit__(self, bint inject_sim_id=True):
        """Recorder C++ constructor"""
        self.ptx = new cpp_cyclus.Recorder(<cpp_bool> inject_sim_id)

    def __dealloc__(self):
        """Recorder C++ destructor."""
        if self.ptx == NULL:
            return
        self.close()
        # Note that we have to do it this way since self.ptx is void*
        cdef cpp_cyclus.Recorder * cpp_ptx = <cpp_cyclus.Recorder *> self.ptx
        del cpp_ptx
        self.ptx = NULL

    @property
    def dump_count(self):
        """The frequency of recording."""
        return (<cpp_cyclus.Recorder*> self.ptx).dump_count()

    @dump_count.setter
    def dump_count(self, value):
        (<cpp_cyclus.Recorder*> self.ptx).set_dump_count(<unsigned int> value)

    @property
    def sim_id(self):
        """The simulation id of the recorder."""
        return uuid_cpp_to_py((<cpp_cyclus.Recorder*> self.ptx).sim_id())

    @property
    def inject_sim_id(self):
        """Whether or not to inject the simulation id into the tables."""
        return (<cpp_cyclus.Recorder*> self.ptx).inject_sim_id()

    @inject_sim_id.setter
    def inject_sim_id(self, value):
        (<cpp_cyclus.Recorder*> self.ptx).inject_sim_id(<bint> value)

    def new_datum(self, title):
        """Returns a new datum instance."""
        cdef std_string cpp_title = str_py_to_cpp(title)
        cdef _Datum d = Datum(new=False)
        (<_Datum> d).ptx = (<cpp_cyclus.Recorder*> self.ptx).NewDatum(cpp_title)
        return d

    def register_backend(self, backend):
        """Registers a backend with the recorder."""
        cdef cpp_cyclus.RecBackend* b
        if isinstance(backend, Hdf5Back):
            b = <cpp_cyclus.RecBackend*> (
                <cpp_cyclus.Hdf5Back*> (<_Hdf5Back> backend).ptx)
        elif isinstance(backend, SqliteBack):
            b = <cpp_cyclus.RecBackend*> (
                <cpp_cyclus.SqliteBack*> (<_SqliteBack> backend).ptx)
        elif isinstance(backend, FullBackend):
            b = <cpp_cyclus.RecBackend*> ((<_FullBackend> backend).ptx)
        else:
            raise ValueError("type of backend not recognized for " +
                             str(type(backend)))
        (<cpp_cyclus.Recorder*> self.ptx).RegisterBackend(b)

    def flush(self):
        """Flushes the recorder to disk."""
        (<cpp_cyclus.Recorder*> self.ptx).Flush()

    def close(self):
        """Closes the recorder."""
        (<cpp_cyclus.Recorder*> self.ptx).Close()


class Recorder(_Recorder, object):
    """Cyclus recorder interface."""

#
# Agent Spec
#
cdef class _AgentSpec:

    def __cinit__(self, spec=None, lib=None, agent=None, alias=None):
        cdef std_string cpp_spec, cpp_lib, cpp_agent, cpp_alias
        if spec is None:
            self.ptx = new cpp_cyclus.AgentSpec()
        elif lib is None:
            cpp_spec = str_py_to_cpp(spec)
            self.ptx = new cpp_cyclus.AgentSpec(cpp_spec)
        else:
            cpp_spec = str_py_to_cpp(spec)
            cpp_lib = str_py_to_cpp(lib)
            cpp_agent = str_py_to_cpp(agent)
            cpp_alias = str_py_to_cpp(alias)
            self.ptx = new cpp_cyclus.AgentSpec(cpp_spec, cpp_lib,
                                                cpp_agent, cpp_alias)

    def __dealloc__(self):
        del self.ptx

    def __str__(self):
        cdef std_string cpp_rtn = self.ptx.str()
        rtn = std_string_to_py(cpp_rtn)
        return rtn

    def sanatize(self):
        cdef std_string cpp_rtn = self.ptx.Sanitize()
        rtn = std_string_to_py(cpp_rtn)
        return rtn

    @property
    def path(self):
        cdef std_string cpp_rtn = self.ptx.path()
        rtn = std_string_to_py(cpp_rtn)
        return rtn

    @property
    def lib(self):
        cdef std_string cpp_rtn = self.ptx.lib()
        rtn = std_string_to_py(cpp_rtn)
        return rtn

    @property
    def agent(self):
        cdef std_string cpp_rtn = self.ptx.agent()
        rtn = std_string_to_py(cpp_rtn)
        return rtn

    @property
    def alias(self):
        cdef std_string cpp_rtn = self.ptx.alias()
        rtn = std_string_to_py(cpp_rtn)
        return rtn


class AgentSpec(_AgentSpec):
    """AgentSpec C++ wrapper

    Parameters
    ----------
    spec : str or None, optional
        This repesents either the full string form of the spec or
        just the path.
    lib : str or None, optional
    agent : str or None, optional
    alias : str or None, optional
    """

#
# Dynamic Module
#
cdef class _DynamicModule:

    def __cinit__(self):
        self.ptx = new cpp_cyclus.DynamicModule()

    def __dealloc__(self):
        del self.ptx

    @staticmethod
    def make(ctx, spec):
        """Returns a newly constructed agent for the given module spec.

        Parameters
        ----------
        ctx : Context
        spec : AgentSpec or str

        Returns
        -------
        rtn : Agent
        """
        cdef _AgentSpec cpp_spec
        if isinstance(spec, str):
            spec = AgentSpec(spec)
        cpp_spec = <_AgentSpec> spec
        cdef cpp_cyclus.Agent* a_ptx = cpp_cyclus.DynamicModule.Make(
                                            (<_Context> ctx).ptx,
                                            deref(cpp_spec.ptx),
                                            )
        agent = agent_to_py(a_ptx, ctx)
        return agent

    def exists(self, _AgentSpec spec):
        """Tests whether an agent spec exists."""
        cdef cpp_bool rtn = self.ptx.Exists(deref(spec.ptx))
        return rtn

    def close_all(self):
        """Closes all dynamic modules."""
        self.ptx.CloseAll()

    @property
    def path(self):
        cdef std_string cpp_rtn = self.ptx.path()
        rtn = std_string_to_py(cpp_rtn)
        return rtn


class DynamicModule(_DynamicModule):
    """Dynamic Module wrapper class."""

#
# Env
#
cdef class _Env:

    @staticmethod
    def path_base(path):
        """Effectively basename"""
        cdef std_string cpp_path = str_py_to_cpp(path)
        cdef std_string cpp_rtn = cpp_cyclus.Env.PathBase(cpp_path)
        rtn = std_string_to_py(cpp_rtn)
        return rtn

    @property
    def install_path(self):
        """The Cyclus install path."""
        cdef std_string cpp_rtn = cpp_cyclus.Env.GetInstallPath()
        rtn = std_string_to_py(cpp_rtn)
        return rtn

    @property
    def build_path(self):
        """The Cyclus build path."""
        cdef std_string cpp_rtn = cpp_cyclus.Env.GetBuildPath()
        rtn = std_string_to_py(cpp_rtn)
        return rtn

    @staticmethod
    def get(var):
        """Obtains an environment variable."""
        cdef std_string cpp_var = str_py_to_cpp(var)
        cdef std_string cpp_rtn = cpp_cyclus.Env.GetEnv(cpp_var)
        rtn = std_string_to_py(cpp_rtn)
        return rtn

    @property
    def nuc_data(self):
        """The nuc_data path."""
        cdef std_string cpp_rtn = cpp_cyclus.Env.nuc_data()
        rtn = std_string_to_py(cpp_rtn)
        return rtn

    @staticmethod
    def set_nuc_data_path(path=None):
        """Initializes the path to the cyclus_nuc_data.h5 file

        By default, it is assumed to be located in the path given by
        GetInstallPath()/share; however, paths in environment variable
        CYCLUS_NUC_DATA are checked first.
        """
        cdef std_string cpp_path
        if path is None:
            cpp_cyclus.Env.SetNucDataPath(cpp_cyclus.Env.nuc_data())
        else:
            cpp_path = str_py_to_cpp(path)
            cpp_cyclus.Env.SetNucDataPath(cpp_path)

    @staticmethod
    def rng_schema(flat=False):
        """Returns the current rng schema.  Uses CYCLUS_RNG_SCHEMA env var
        if set; otherwise uses the default install location. If using the
        default ocation, set flat=True for the default flat schema.
        """
        cdef std_string cpp_rtn = cpp_cyclus.Env.rng_schema(flat)
        rtn = std_string_to_py(cpp_rtn)
        return rtn

    @property
    def cyclus_path(self):
        """A tuple of strings representing where cyclus searches for
        modules.
        """
        cdef std_vector[std_string] cpp_rtn = cpp_cyclus.Env.cyclus_path()
        rtn = std_vector_std_string_to_py(cpp_rtn)
        return tuple(rtn)

    @property
    def allow_milps(self):
        """whether or not Cyclus should allow Mixed-Integer Linear Programs
        The default depends on a compile time option DEFAULT_ALLOW_MILPS, but
        may be specified at run time with the ALLOW_MILPS environment variable.
        """
        cdef cpp_bool cpp_rtn = cpp_cyclus.Env.allow_milps()
        rtn = bool_to_py(cpp_rtn)
        return rtn

    @property
    def env_delimiter(self):
        """the correct environment variable delimiter based on the file
        system.
        """
        cdef std_string cpp_rtn = cpp_cyclus.Env.EnvDelimiter()
        rtn = std_string_to_py(cpp_rtn)
        return rtn

    @property
    def path_delimiter(self):
        """the correct path delimiter based on the file
        system.
        """
        cdef std_string cpp_rtn = cpp_cyclus.Env.PathDelimiter()
        rtn = std_string_to_py(cpp_rtn)
        return rtn

    @staticmethod
    def find_module(path):
        """Returns the full path to a module by searching through default
        install and CYCLUS_PATH directories.
        """
        cdef std_string cpp_path = str_py_to_cpp(path)
        cdef std_string cpp_rtn = cpp_cyclus.Env.FindModule(cpp_path)
        rtn = std_string_to_py(cpp_rtn)
        return rtn


class Env(_Env):
    """Environment wrapper class.

    An environment utility to help locate files and find environment
    settings. The environment for a given simulation can be accessed via the
    simulation's Context.
    """


#
# Logger
#

# LogLevel
LEV_ERROR = cpp_cyclus.LEV_ERROR
LEV_WARN = cpp_cyclus.LEV_WARN
LEV_INFO1 = cpp_cyclus.LEV_INFO1
LEV_INFO2 = cpp_cyclus.LEV_INFO2
LEV_INFO3 = cpp_cyclus.LEV_INFO3
LEV_INFO4 = cpp_cyclus.LEV_INFO4
LEV_INFO5 = cpp_cyclus.LEV_INFO5
LEV_DEBUG1 = cpp_cyclus.LEV_INFO5
LEV_DEBUG2 = cpp_cyclus.LEV_DEBUG2
LEV_DEBUG3 = cpp_cyclus.LEV_DEBUG3
LEV_DEBUG4 = cpp_cyclus.LEV_DEBUG4
LEV_DEBUG5 = cpp_cyclus.LEV_DEBUG5


cdef class _Logger:

    @property
    def report_level(self):
        """Use to get/set the (global) log level report cutoff."""
        cdef cpp_cyclus.LogLevel cpp_rtn = cpp_cyclus.Logger.ReportLevel()
        rtn = int_to_py(cpp_rtn)
        return rtn

    @report_level.setter
    def report_level(self, int level):
        cpp_cyclus.Logger.SetReportLevel(<cpp_cyclus.LogLevel> level)

    @property
    def no_agent(self):
        """Set whether or not agent/agent log entries should be printed"""
        cdef cpp_bool cpp_rtn = cpp_cyclus.Logger.NoAgent()
        rtn = bool_to_py(cpp_rtn)
        return rtn

    @no_agent.setter
    def no_agent(self, bint na):
        cpp_cyclus.Logger.SetNoAgent(na)

    @property
    def no_mem(self):
        cdef cpp_bool cpp_rtn = cpp_cyclus.Logger.NoMem()
        rtn = bool_to_py(cpp_rtn)
        return rtn

    @no_mem.setter
    def no_mem(self, bint nm):
        cpp_cyclus.Logger.SetNoMem(nm)

    @staticmethod
    def to_log_level(text):
        """Converts a string into a corresponding LogLevel value.

        For strings that do not correspond to any particular LogLevel enum value
        the method returns the LogLevel value `LEV_ERROR`.  This method is
        primarily intended for translating command line verbosity argument(s) in
        appropriate report levels.  LOG(level) statements
        """
        cdef std_string cpp_text = str_py_to_cpp(text)
        cdef cpp_cyclus.LogLevel cpp_rtn = cpp_cyclus.Logger.ToLogLevel(cpp_text)
        rtn = <int> cpp_rtn
        return rtn

    @staticmethod
    def to_string(level):
        """Converts a LogLevel enum value into a corrsponding string.

        For a level argments that have no corresponding string value, the string
        `BAD_LEVEL` is returned.  This method is primarily intended for translating
        LOG(level) statement levels into appropriate strings for output to stdout.
        """
        cdef cpp_cyclus.LogLevel cpp_level = <cpp_cyclus.LogLevel> level
        cdef std_string cpp_rtn = cpp_cyclus.Logger.ToString(cpp_level)
        rtn = std_string_to_py(cpp_rtn)
        return rtn


class Logger(_Logger):
    """A logging tool providing finer grained control over standard output
    for debugging and other purposes.
    """

#
# Errors
#
def get_warn_limit():
    """Returns the current warning limit."""
    wl = cpp_cyclus.warn_limit
    return wl


def set_warn_limit(unsigned int wl):
    """Sets the warning limit."""
    cpp_cyclus.warn_limit = wl


def get_warn_as_error():
    """Returns the current value for wether warnings should be treated
    as errors.
    """
    wae = bool_to_py(cpp_cyclus.warn_as_error)
    return wae


def set_warn_as_error(bint wae):
    """Sets whether warnings should be treated as errors."""
    cpp_cyclus.warn_as_error = wae


#
# PyHooks
#
def py_append_init_tab():
    """Initializes Cyclus-internal Python import table. This is called
    automatically when cyclus is imported. Users should not need to call
    this function.
    """
    cpp_cyclus.PyAppendInitTab()


def py_import_init():
    """Initializes Cyclus-internal Python imports. This is called
    automatically when cyclus is imported. Users should not need to call
    this function.
    """
    cpp_cyclus.PyImportInit()

#
# XML
#
cdef class _XMLFileLoader:

    def __cinit__(self, recorder, backend, schema_file, input_file="", format="none", ms_print=False):
        cdef std_string cpp_schema_file = str_py_to_cpp(schema_file)
        cdef std_string cpp_input_file = str_py_to_cpp(input_file)
        format = "none" if format is None else format
        cdef std_string cpp_format = str_py_to_cpp(format)
        cdef cpp_bool cpp_msprint = bool_to_cpp(ms_print)
        self.ptx = new cpp_cyclus.XMLFileLoader(
            <cpp_cyclus.Recorder *> (<_Recorder> recorder).ptx,
            <cpp_cyclus.QueryableBackend *> (<_FullBackend> backend).ptx,
            cpp_schema_file, cpp_input_file, cpp_format, cpp_msprint)

    def __dealloc__(self):
        del self.ptx

    def load_sim(self):
        """Load an entire simulation from the inputfile."""
        self.ptx.LoadSim()


class XMLFileLoader(_XMLFileLoader):
    """Handles initialization of a database with information from
    a cyclus xml input file.

    Create a new loader reading from the xml simulation input file and writing
    to and initializing the backends in the recorder. The recorder must
    already have the backend registered. schema_file identifies the main
    xml rng schema used to validate the input file. The format specifies the
    input file format from one of: "none", "xml", "json", or "py".
    """


cdef class _XMLFlatLoader:

    def __cinit__(self, recorder, backend, schema_file, input_file="", format="none", ms_print=False):
        cdef std_string cpp_schema_file = str_py_to_cpp(schema_file)
        cdef std_string cpp_input_file = str_py_to_cpp(input_file)
        format = "none" if format is None else format
        cdef std_string cpp_format = str_py_to_cpp(format)
        cdef cpp_bool cpp_msprint = bool_to_cpp(ms_print)
        self.ptx = new cpp_cyclus.XMLFlatLoader(
            <cpp_cyclus.Recorder *> (<_Recorder> recorder).ptx,
            <cpp_cyclus.QueryableBackend *> (<_FullBackend> backend).ptx,
            cpp_schema_file, cpp_input_file, cpp_format, cpp_msprint)

    def __dealloc__(self):
        del self.ptx

    def load_sim(self):
        """Load an entire simulation from the inputfile."""
        self.ptx.LoadSim()


class XMLFlatLoader(_XMLFlatLoader):
    """Handles initialization of a database with information from
    a cyclus xml input file.

    Create a new loader reading from the xml simulation input file and writing
    to and initializing the backends in the recorder. The recorder must
    already have the backend registered. schema_file identifies the main
    xml rng schema used to validate the input file. The format specifies the
    input file format from one of: "none", "xml", "json", or "py".

    Notes
    -----
    This is not a subclass of the XMLFileLoader Python bindings, even
    though the C++ class is a subclass in C++. Rather, they are duck
    typed by exposing the same interface. This makes handling the
    instance pointers in Cython a little easier.
    """

cdef class _ColumnInfo:
    def __cinit__(self):
        self.ptx = NULL

    cdef void copy_from(self, cpp_cyclus.ColumnInfo cinfo):
        self.ptx = new cpp_cyclus.ColumnInfo(cinfo.table, cinfo.col, cinfo.index,
                                             cinfo.dbtype, cinfo.shape)

    def __dealloc__(self):
        if self.ptx == NULL:
            pass
        else:
            del self.ptx

    def __repr__(self):
        s = 'ColumnInfo(table=' + self.table + ', col=' + self.col + ', index='\
            + str(self.index) + ', dbtype=' + str(self.dbtype) + ', shape=' + repr(self.shape) + ')'
        return s

    @property
    def table(self):
        table = std_string_to_py(self.ptx.table)
        return table

    @property
    def col(self):
        col = std_string_to_py(self.ptx.col)
        return col

    @property
    def index(self):
        return self.ptx.index

    @property
    def dbtype(self):
        return self.ptx.dbtype

    @property
    def shape(self):
        shape = std_vector_int_to_py(self.ptx.shape)
        return shape

class ColumnInfo(_ColumnInfo):
    """Python wrapper for ColumnInfo"""

def load_string_from_file(filename, format=None):
    """Loads an XML file from a path or from a string and a format ('xml', 'json', or 'py')."""
    cdef std_string cpp_filename = str_py_to_cpp(filename)
    cdef std_string none = std_string(b"none")
    cdef std_string cpp_format
    cpp_format = none if format is None else str_py_to_cpp(format)
    cdef std_string cpp_rtn = cpp_cyclus.LoadStringFromFile(cpp_filename, cpp_format)
    rtn = std_string_to_py(cpp_rtn)
    return rtn



cdef class _XMLParser:

    def __cinit__(self, filename=None, format=None, raw=None):
        cdef std_string s, f, inp
        cdef std_string none = std_string(b"none")
        self.ptx = new cpp_cyclus.XMLParser()
        if filename is not None:
            s = str_py_to_cpp(filename)
            f = none if format is None else str_py_to_cpp(format)
            inp = cpp_cyclus.LoadStringFromFile(s, f)
        elif raw is not None:
            if format is None:
                inp = str_py_to_cpp(raw)
            else:
                f = str_py_to_cpp(format)
                inp = cpp_cyclus.LoadStringFromFile(s, f)
        else:
            raise RuntimeError("Either a filename or a raw string "
                               "must be provided to XMLParser")
        self.ptx.Init(inp)

    def __dealloc__(self):
        del self.ptx


class XMLParser(_XMLParser):
    """A helper class to hold xml file data and provide automatic
    validation.

    Parameters
    ----------
    filename : str, optional
        Path to file to load.
    format : str, optional
        The format to read in as: "none", "xml", "json", or "py". Applies to either
        filename or raw, if given.
    raw : str, optional
        XML string to load.
    """


cdef class _InfileTree:

    def __cinit__(self, bint free=False):
        self.ptx = NULL
        self._free = free

    def __dealloc__(self):
        if self.ptx == NULL:
            return
        if self._free:
            del self.ptx

    @classmethod
    def from_parser(cls, _XMLParser parser):
        """Initializes an input file tree from an XML parser.

        Parameters
        ----------
        parser : XMLParser
            An XMLParser instance.
        """
        cdef _InfileTree self = cls(free=True)
        self.ptx = new cpp_cyclus.InfileTree(parser.ptx[0])
        return self

    def nmatches(self, query):
        """Investigates the current status and returns the number of elements
        matching a query.

        Parameters
        ----------
        query : str
            The XML path to count.

        Returns
        -------
        n : int
            The number of query matches.
        """
        cdef std_string cpp_query = str_py_to_cpp(query)
        return self.ptx.NMatches(cpp_query)

    def query(self, path, dbtype, int index=0):
        """A query method for required parameters.

        Parameters
        ----------
        path : str
            The XML path to test if it exists.
        dbtype : str or int
            Represents primitive type in type system. Note that only
            primitive types are queryable, by design, and not all primitive types.
            Blobs and UUIDs are not included because they cannot be lexically cast.
        index : int, optional
            The instance to query.
        """
        cdef std_string cpp_query = str_py_to_cpp(path)
        cdef std_string str_default, str_rtn
        cdef cpp_bool bool_rtn
        cdef cpp_typesystem.DbTypes i
        # get an dbtype
        if isinstance(dbtype, int):
            i = dbtype
        else:
            i = C_IDS[dbtype]
        # run the query
        if i == cpp_typesystem.BOOL:
            bool_rtn = cpp_cyclus.Query[cpp_bool](self.ptx, cpp_query, index)
            rtn = bool_to_py(bool_rtn)
        elif i == cpp_typesystem.INT:
            rtn = cpp_cyclus.Query[int](self.ptx, cpp_query, index)
        elif i == cpp_typesystem.FLOAT:
            rtn = cpp_cyclus.Query[float](self.ptx, cpp_query, index)
        elif i == cpp_typesystem.DOUBLE:
            rtn = cpp_cyclus.Query[double](self.ptx, cpp_query, index)
        elif i == cpp_typesystem.STRING:
            str_rtn = cpp_cyclus.Query[std_string](self.ptx, cpp_query, index)
            rtn = std_string_to_py(str_rtn)
        elif i == cpp_typesystem.VL_STRING:
            str_rtn = cpp_cyclus.Query[std_string](self.ptx, cpp_query, index)
            rtn = std_string_to_py(str_rtn)
        else:
            raise TypeError("Type {} not recognized, only ".format(dbtype) +
                            "primitive types are supported.")
        return rtn

    def optional_query(self, query, default):
        """A query method for optional parameters.

        Parameters
        ----------
        query : str
            The XML path to test if it exists.
        default : any type
            The default value to return if the XML path does not exist in
            the tree. The type of the return value (str, bool, int, etc)
            is determined by the type of the default.
        """
        cdef std_string cpp_query = str_py_to_cpp(query)
        cdef std_string str_default, str_rtn
        if isinstance(default, str):
            str_default = str_py_to_cpp(default)
            str_rtn = cpp_cyclus.OptionalQuery[std_string](self.ptx, cpp_query,
                                                           str_default)
            rtn = std_string_to_py(str_rtn)
        else:
            raise TypeError("Type of default value not recognized, only "
                            "str is currently supported.")
        return rtn

    def subtree(self, query, int index=0):
        """Populates a child infile based on a query and index.

        Parameters
        ----------
        query : str
            The XML path to test if it exists.
        index : int, optional
            The index of the queried element, default 0.

        Returns
        -------
        sub : InfileTree
            A sub-tree repreresenting the query and the index.
        """
        cdef std_string cpp_query = str_py_to_cpp(query)
        cdef _InfileTree sub = InfileTree(free=False)
        sub.ptx = self.ptx.SubTree(cpp_query, index)
        rtn = sub
        return rtn


class InfileTree(_InfileTree):
    """A class for extracting information from a given XML parser

    Parameters
    ----------
    free : bool, optional
        Whether or not to free the C++ instance on deallocation.
    """


cdef class _DbInit:

    def __cinit__(self, bint free=False):
        self.ptx = NULL
        self._free = free

    def __dealloc__(self):
        if self.ptx == NULL:
            return
        if self._free:
            del self.ptx

    def new_datum(self, title):
        """Returns a new datum to be used exactly as the Context.new_datum() method.
        Users must not add fields to the datum that are automatically injected:
        'SimId', 'AgentId', and 'SimTime'.
        """
        cdef std_string cpp_title = str_py_to_cpp(title)
        cdef _Datum d = Datum(new=False)
        (<_Datum> d).ptx = self.ptx.NewDatum(cpp_title)
        return d


class DbInit(_DbInit):
    """DbInit provides an interface for agents to record data to the output db that
    automatically injects the agent's id and current timestep alongside all
    recorded data.  The prefix 'AgentState' + [spec] (e.g. MyReactor) is also added
    to the datum title.

    Parameters
    ----------
    free : bool, optional
        Whether or not to free the C++ instance on deallocation.
    """

#
# Simulation Managment
#

cdef class _Timer:

    def __cinit__(self, bint init=True):
        self._free = init
        if init:
            self.ptx = new cpp_cyclus.Timer()
        else:
            self.ptx == NULL

    def __dealloc__(self):
        if self.ptx == NULL:
            return
        elif self._free:
            del self.ptx

    def run_sim(self):
        """Runs the simulation."""
        self.ptx.RunSim()


class Timer(_Timer):
    """Controls simulation timestepping and inter-timestep phases.

    Parameters
    ----------
    init : bool, optional
        Whether or not we should initialize a new C++ Timer instance.
    """


cdef class _SimInit:

    def __cinit__(self, recorder, backend):
        self.ptx = new cpp_cyclus.SimInit()
        self.ptx.Init(
            <cpp_cyclus.Recorder *> (<_Recorder> recorder).ptx,
            <cpp_cyclus.QueryableBackend *> (<_FullBackend> backend).ptx,
            )
        self._timer = None
        self._context = None

    def __dealloc__(self):
        del self.ptx

    @property
    def timer(self):
        """Returns the initialized timer. Note that either Init, Restart,
        or Branch must be called first.
        """
        if self._timer is None:
            self._timer = Timer(init=False)
            (<_Timer> self._timer).ptx = self.ptx.timer()
        return self._timer

    @property
    def context(self):
        """Returns the initialized context. Note that either Init, Restart, or Branch
        must be called first.
        """
        if self._context is None:
            self._context = Context(init=False)
            (<_Context> self._context).ptx = self.ptx.context()
        return self._context


class SimInit(_SimInit):
    """Handles initialization of a simulation from the output database. After
    calling Init, Restart, or Branch, the initialized Context, Timer, and
    Recorder can be retrieved.

    Parameters
    ----------
    recorder : Recorder
        The recorder class for the simulation.
    backend : QueryableBackend
        A backend to use for this simulation.
    """

#
# Agent
#

cpdef object capsule_agent_to_py(object agent, object ctx):
    """Returns an agent from its id"""
    cdef cpp_cyclus.Agent* avoid = <cpp_cyclus.Agent*> PyCapsule_GetPointer(agent, <char*> b"agent")
    a = agent_to_py(avoid, ctx)
    return a


cdef object agent_to_py(cpp_cyclus.Agent* a_ptx, object ctx):
    """Converts and agent pointer to Python."""
    global _AGENT_REFS
    if a_ptx == NULL:
        return None
    cdef int a_id = a_ptx.id()
    if a_id in _AGENT_REFS:
        return _AGENT_REFS[a_id]
    if ctx is None:
        # have to make new wrapper instance
        ctx = Context(init=False)
        (<_Context> ctx).ptx = a_ptx.context()
    elif not isinstance(ctx, Context):
        # have a pycapsule, need to pull it out
        cap = ctx
        ctx = Context(init=False)
        (<_Context> ctx).ptx = <cpp_cyclus.Context*> PyCapsule_GetPointer(cap, <char*> b"ctx")
    cdef _Agent a = Agent(ctx)
    a.ptx = a_ptx
    _AGENT_REFS[a_id] = a
    return a


cdef dict inventories_to_py(cpp_cyclus.Inventories& invs):
    """Converts inverories map to a dict of lists of Resources."""
    cdef dict pyinvs = {}
    cdef list value
    cdef ts._Resource r
    for name_value in invs:
        name = std_string_to_py(name_value.first)
        value = []
        for x in name_value.second:
            r = ts.Resource()
            r.ptx = x
            value.append(r)
        pyinvs[name] = value
    return pyinvs


cdef cpp_cyclus.Inventories inventories_to_cpp(object pyinvs):
    """Converts a dict of lists of resources to inventories."""
    cdef cpp_cyclus.Inventories invs = cpp_cyclus.Inventories()
    cdef std_string name
    cdef std_vector[shared_ptr[cpp_cyclus.Resource]] value
    for pyname, pyvalue in pyinvs.items():
        name = str_py_to_cpp(pyname)
        value = std_vector[shared_ptr[cpp_cyclus.Resource]]()
        for r in pyvalue:
            value.push_back((<ts._Resource> r).ptx)
    return invs


cdef class _Agent:

    def __cinit__(self, _Context ctx):
        self._free = False
        self.ptx = NULL
        self._annotations = None
        self._context = ctx

    def __dealloc__(self):
        cdef cpp_cyclus.Agent* cpp_ptx
        if self.ptx == NULL:
            return
        elif self._free:
            cpp_ptx = <cpp_cyclus.Agent*> self.ptx
            del cpp_ptx
            self.ptx = NULL
        else:
            self.ptx = NULL

    @property
    def version(self):
        """Agent version string."""
        cdef std_string cpp_rtn = (<cpp_cyclus.Agent*> self.ptx).version()
        rtn = std_string_to_py(cpp_rtn)
        return rtn

    def children_str(self):
        """Returns recursively generated string of the parent-child tree."""
        return std_string_to_py((<cpp_cyclus.Agent*> self.ptx).PrintChildren())

    def tree_strs(self, m):
        """Returns a list of children strings representing the parent-child tree
        at the node for Agent m.
        """
        cdef cpp_cyclus.Agent* cpp_m = <cpp_cyclus.Agent*> (<_Agent> m).ptx
        rtn = std_vector_std_string_to_py(
                (<cpp_cyclus.Agent*> self.ptx).GetTreePrintOuts(cpp_m))
        return rtn

    def in_family_tree(self, other):
        """Returns true if this agent is in the parent-child family tree of an
        other agent.
        """
        cdef cpp_cyclus.Agent* cpp_other = <cpp_cyclus.Agent*> (<_Agent> other).ptx
        rtn = bool_to_py((<cpp_cyclus.Agent*> self.ptx).InFamilyTree(cpp_other))
        return rtn

    def ancestor_of(self, other):
        """Returns true if this agent is an ancestor of an other agent (i.e., resides
        above an other agent in the family tree).
        """
        cdef cpp_cyclus.Agent* cpp_other = <cpp_cyclus.Agent*> (<_Agent> other).ptx
        rtn = bool_to_py((<cpp_cyclus.Agent*> self.ptx).AncestorOf(cpp_other))
        return rtn

    def decendent_of(self, other):
        """Returns true if this agent is an decendent of an other agent (i.e., resides
        above an other agent in the family tree).
        """
        cdef cpp_cyclus.Agent* cpp_other = <cpp_cyclus.Agent*> (<_Agent> other).ptx
        rtn = bool_to_py((<cpp_cyclus.Agent*> self.ptx).DecendentOf(cpp_other))
        return rtn

    def decommission(self):
        """Decommissions the agent, removing it from the simulation. Results in
        destruction of the agent object. If agents write their own decommission()
        function, they must call their superclass' decommission function at the
        END of their decommission() function.
        """
        print('decom_agent')
        (<cpp_cyclus.Agent*> self.ptx).Decommission()

    @property
    def schema(self):
        """An agent's xml rng schema for initializing from input files. All
        concrete agents should override this function. This must validate the same
        xml input that the InfileToDb function receives.
        """
        cdef std_string cpp_rtn = (<cpp_cyclus.Agent*> self.ptx).schema()
        rtn = std_string_to_py(cpp_rtn)
        return rtn

    @property
    def annotations(self):
        """Agent annotations."""
        cdef jsoncpp.Value cpp_rtn = jsoncpp.Value()
        if self._annotations is None:
            cpp_rtn._inst[0] = (<cpp_cyclus.Agent*> self.ptx).annotations()
        self._annotations = cpp_rtn
        return self._annotations

    @property
    def prototype(self):
        """The agent's prototype."""
        rtn = std_string_to_py((<cpp_cyclus.Agent*> self.ptx).get_prototype())
        return rtn

    @prototype.setter
    def prototype(self, str p):
        cdef std_string cpp_p = str_py_to_cpp(p)
        (<cpp_cyclus.Agent*> self.ptx).prototype(cpp_p)

    @property
    def id(self):
        """The agent instance's unique ID within a simulation."""
        return (<cpp_cyclus.Agent*> self.ptx).id()

    def __hash__(self):
        return self.id

    @property
    def spec(self):
        """The agent's spec."""
        rtn = std_string_to_py((<cpp_cyclus.Agent*> self.ptx).get_spec())
        return rtn

    @spec.setter
    def spec(self, str new_impl):
        cdef std_string cpp_new_impl = str_py_to_cpp(new_impl)
        (<cpp_cyclus.Agent*> self.ptx).spec(cpp_new_impl)

    @property
    def kind(self):
        """Returns a string that describes the agent subclass (e.g. Region,
        Facility, etc.)
        """
        rtn = std_string_to_py((<cpp_cyclus.Agent*> self.ptx).kind())
        return rtn

    @property
    def context(self):
        """Returns this agent's simulation context."""
        if self._context is None:
            self._context = Context(init=False)
            (<_Context> self._context).ptx = (<cpp_cyclus.Agent*> self.ptx).context()
        return self._context

    def __str__(self):
        rtn = std_string_to_py((<cpp_cyclus.Agent*> self.ptx).str())
        return rtn

    def parent(self):
        """Returns parent of this agent.  Returns None if the agent has no parent.
        """
        rtn = agent_to_py((<cpp_cyclus.Agent*> self.ptx).parent(), None)
        return rtn

    @property
    def parent_id(self):
        """The id for this agent's parent or -1 if this agent has no parent."""
        return (<cpp_cyclus.Agent*> self.ptx).parent_id()

    @property
    def enter_time(self):
        """The time step at which this agent's Build function was called
        (-1 if the agent has never been built).
        """
        return (<cpp_cyclus.Agent*> self.ptx).enter_time()

    @property
    def lifetime(self):
        """The number of time steps this agent operates between building and
        decommissioning (-1 if the agent has an infinite lifetime)
        """
        return (<cpp_cyclus.Agent*> self.ptx).get_lifetime()

    @lifetime.setter
    def lifetime(self, int n_timesteps):
        (<cpp_cyclus.Agent*> self.ptx).lifetime(n_timesteps)

    def lifetime_force(self, int n_timesteps):
        (<cpp_cyclus.Agent*> self.ptx).lifetime_force(n_timesteps)

    @property
    def exit_time(self):
        """The default time step at which this agent will exit the
        simulation (-1 if the agent has an infinite lifetime).

        Decommissioning happens at the end of a time step. With a lifetime of 1, we
        expect an agent to go through only 1 entire time step. In this case, the
        agent should be decommissioned on the same time step it was
        created. Therefore, for agents with non-infinite lifetimes, the exit_time
        will be the enter time plus its lifetime less 1.
        """
        return (<cpp_cyclus.Agent*> self.ptx).exit_time()

    @property
    def children(self):
        """A frozen set of the children of this agent."""
        kids = []
        for kid_ptx in (<cpp_cyclus.Agent*> self.ptx).children():
            kid = agent_to_py(kid_ptx, None)
            kids.append(kid)
        return frozenset(kids)


class Agent(_Agent):
    """The abstract base class used by all types of agents
    that live and interact in a simulation.
    """


#
# Version Info
#

def describe_version():
    """Describes the Cyclus version."""
    rtn = cpp_cyclus.describe()
    rtn = rtn.decode()
    return rtn


def core_version():
    """Cyclus core version."""
    rtn = cpp_cyclus.core()
    rtn = rtn.decode()
    return rtn


def boost_version():
    """Boost version."""
    rtn = cpp_cyclus.boost()
    rtn = rtn.decode()
    return rtn


def sqlite3_version():
    """SQLite3 version."""
    rtn = cpp_cyclus.sqlite3()
    rtn = rtn.decode()
    return rtn


def hdf5_version():
    """HDF5 version."""
    rtn = cpp_cyclus.hdf5()
    rtn = rtn.decode()
    return rtn


def xml2_version():
    """libxml 2 version."""
    rtn = cpp_cyclus.xml2()
    rtn = rtn.decode()
    return rtn


def xmlpp_version():
    """libxml++ version."""
    rtn = cpp_cyclus.xmlpp()
    rtn = rtn.decode()
    return rtn


def coincbc_version():
    """Coin CBC version."""
    rtn = cpp_cyclus.coincbc()
    rtn = rtn.decode()
    return rtn


def coinclp_version():
    """Coin CLP version."""
    rtn = cpp_cyclus.coinclp()
    rtn = rtn.decode()
    return rtn


def version():
    """Returns string of the cyclus version and its dependencies."""
    s = "Cyclus Core " + core_version() + " (" + describe_version() + ")\n\n"
    s += "Dependencies:\n"
    s += "   Boost    " + boost_version() + "\n"
    s += "   Coin-Cbc " + coincbc_version() + "\n"
    s += "   Coin-Clp " + coinclp_version() + "\n"
    s += "   Hdf5     " + hdf5_version() + "\n"
    s += "   Sqlite3  " + sqlite3_version() + "\n"
    s += "   xml2     " + xml2_version() + "\n"
    s += "   xml++    " + xmlpp_version() + "\n"
    return s

#
# Context
#
cdef class _Context:

    def __cinit__(self, timer=None, recorder=None, init=True):
        self._free = init
        if init:
            self.ptx = new cpp_cyclus.Context(
                (<_Timer> timer).ptx,
                (<cpp_cyclus.Recorder*> (<_Recorder> recorder).ptx),
                )
        else:
            self.ptx = NULL

    def __dealloc__(self):
        if self.ptx == NULL or not self._free:
            return
        del self.ptx

    def add_recipe(self, name, comp, basis):
        """
        Adds a new recipe to a simulation 

        Parameters:
        ----------
        name: str
            name for recipe
        comp: dict
            dictionary mapping nuclides to their compostion fraction
        basis: str
            'atom' or 'mass' to specify the type of composition fraction
        """
        cdef std_string cpp_name = str_py_to_cpp(name)
        cpp_comp = ts.composition_ptr_from_py(comp, basis)
        self.ptx.AddRecipe(cpp_name, cpp_comp)


    def del_agent(self, agent):
        """Destructs and cleans up an agent (and it's children recursively)."""
        self.ptx.DelAgent(dynamic_agent_ptr(agent))

    @property
    def sim_id(self):
        """The simulation ID."""
        cdef cpp_cyclus.uuid cpp_sim_id = self.ptx.sim_id()
        rtn = uuid_cpp_to_py(cpp_sim_id)
        return rtn

    @property
    def time(self):
        """The simulation time step number."""
        return self.ptx.time()

    @property
    def dt(self):
        """The length of timer per time step, in seconds."""
        return self.ptx.dt()

    def get_recipe(self, name, basis='mass'):
        """Retrieve a registered recipe. This is intended for retrieving
        compositions loaded from an input file(s) at the start of a
        simulation and NOT for communicating compositions between facilities
        during the simulation. You must provide the basis as either "mass" or
        "atom".
        """
        c = ts.composition_from_cpp(self.ptx.GetRecipe(str_py_to_cpp(name)), basis)
        return c

    def schedule_build(self, parent, proto_name, int t=-1):
        """Schedules the named prototype to be built for the specified parent at
        timestep t. The default t=-1 results in the build being scheduled for the
        next build phase (i.e. the start of the next timestep).
        """
        self.ptx.SchedBuild(dynamic_agent_ptr(parent),
                            str_py_to_cpp(proto_name), t)

    def schedule_decom(self, agent, int t=-1):
        """Schedules the given Agent to be decommissioned at the specified timestep
        t. The default t=-1 results in the decommission being scheduled for the
        next decommission phase (i.e. the end of the current timestep).
        """
        print('schedule')
        self.ptx.SchedDecom(dynamic_agent_ptr(agent), t)
        print('schedule2')

    def new_datum(self, title):
        """Returns a new datum instance."""
        cdef std_string cpp_title = str_py_to_cpp(title)
        cdef _Datum d = Datum(new=False)
        (<_Datum> d).ptx = self.ptx.NewDatum(cpp_title)
        return d


class Context(_Context):
    """A simulation context provides access to necessary simulation-global
    functions and state. All code that writes to the output database, needs to
    know simulation time, creates/builds facilities, and/or uses loaded
    composition recipes will need a context pointer. In general, all global
    state should be accessed through a simulation context.

    Parameters
    ----------
    timer : Timer, optional
        An instance of the timer class.
    recorder : Recorder, optional
        An instance of the recorder class.
    init : bool, optional
        Whether or not to initialize a new context object.

    Warnings
    --------
    * The context takes ownership of and manages the lifetime/destruction
      of all agents constructed with it (including Cloned agents). Agents should
      generally NEVER be allocated on the stack.
    * The context takes ownership of the solver and will manage its
      destruction.
    """

#
# Discovery
#
def discover_specs(path, library):
    """Discover archetype specifications for a path and library.
    Returns a set of strings.
    """
    cdef std_string cpp_path = str_py_to_cpp(path)
    cdef std_string cpp_library = str_py_to_cpp(library)
    cdef std_set[std_string] cpp_rtn = cpp_cyclus.DiscoverSpecs(cpp_path,
                                                                cpp_library)
    rtn = std_set_std_string_to_py(cpp_rtn)
    return rtn


def discover_specs_in_cyclus_path():
    """Discover archetype specifications that live recursively in CYCLUS_PATH
    directories. Returns a set of strings.
    """
    cdef std_set[std_string] cpp_rtn = cpp_cyclus.DiscoverSpecsInCyclusPath()
    rtn = std_set_std_string_to_py(cpp_rtn)
    return rtn


def discover_metadata_in_cyclus_path():
    """Discover archetype metadata in cyclus path. Returns a Jason.Value
    object.
    """
    cdef jsoncpp.Value cpp_rtn = jsoncpp.Value()
    cpp_rtn._inst[0] = cpp_cyclus.DiscoverMetadataInCyclusPath()
    rtn = cpp_rtn
    return rtn


#
# Infile Converters
#
def json_to_xml(s):
    """Converts a JSON string into an equivalent XML string"""
    cdef std_string cpp_s = str_py_to_cpp(s)
    cdef std_string cpp_rtn = cpp_cyclus.JsonToXml(cpp_s)
    rtn = std_string_to_py(cpp_rtn)
    return rtn


def xml_to_json(s):
    """Converts an XML string into an equivalent JSON string"""
    cdef std_string cpp_s = str_py_to_cpp(s)
    cdef std_string cpp_rtn = cpp_cyclus.XmlToJson(cpp_s)
    rtn = std_string_to_py(cpp_rtn)
    return rtn


def json_to_py(s):
    """Converts a JSON string into an equivalent Python string"""
    cdef std_string cpp_s = str_py_to_cpp(s)
    cdef std_string cpp_rtn = cpp_cyclus.JsonToPy(cpp_s)
    rtn = std_string_to_py(cpp_rtn)
    return rtn


def py_to_json(s):
    """Converts a Python string into an equivalent JSON string"""
    cdef std_string cpp_s = str_py_to_cpp(s)
    cdef std_string cpp_rtn = cpp_cyclus.PyToJson(cpp_s)
    rtn = std_string_to_py(cpp_rtn)
    return rtn


def py_to_xml(s):
    """Converts a Python string into an equivalent XML string"""
    cdef std_string cpp_s = str_py_to_cpp(s)
    cdef std_string cpp_rtn = cpp_cyclus.PyToXml(cpp_s)
    rtn = std_string_to_py(cpp_rtn)
    return rtn


def xml_to_py(s):
    """Converts an XML string into an equivalent Python string"""
    cdef std_string cpp_s = str_py_to_cpp(s)
    cdef std_string cpp_rtn = cpp_cyclus.XmlToPy(cpp_s)
    rtn = std_string_to_py(cpp_rtn)
    return rtn

#
# Tools
#
cdef cpp_jsoncpp.Value str_to_json_value(object pyanno):
    """Converts a string to a JSON tree."""
    cdef std_string anno = str_py_to_cpp(pyanno)
    cdef cpp_jsoncpp.Value root
    cdef cpp_jsoncpp.Reader reader
    cdef cpp_bool parsed_ok = reader.parse(anno, root)
    if not parsed_ok:
        raise ValueError("JSON string is malformed")
    return root


cpdef dict normalize_request_portfolio(object inp):
    """Normalizes a request portfolio into a standard Python form, ready to be traded.
    Note that this does not include the requester object.
    """
    # get initial values
    if not isinstance(inp, Mapping):
        inp = dict(inp)
    if 'commodities' in inp:
        commods = []
        for commodity in inp['commodities']:
            for name, reqs in commodity.items():
                if name == 'preference' or name == 'exclusive':
                    continue

                commods.append({name:reqs})
        constrs = inp.get('constraints', [])
    else:
        commods = []
        for name, reqs in inp.items():
            if name == 'preference' or name == 'exclusive':
                continue
            commods.append({name:reqs})
        constrs = []
    # canonize constraints
    if not isinstance(constrs, Iterable):
        constrs = [constrs]
    # canonize commods
    if not isinstance(commods, Iterable):
        commods = list(commods)
    cdef dict default_req = {'target': None, 'preference': 1.0,
                             'exclusive': False, 'cost': None}
    for index, commodity in enumerate(commods):
        for key, val in commodity.items():
            if isinstance(val, ts.Resource):
                req = default_req.copy()
                req['target'] = val
                if 'commodities' in inp:
                    if 'preference' in inp['commodities'][index]:
                        req['preference'] = inp['commodities'][index]['preference']
                    if 'exclusive' in inp['commodities'][index]:
                        req['exclusive'] = inp['commodities'][index]['exclusive']
                commods[index][key] = [req]

            elif isinstance(val, Mapping):
                req = default_req.copy()
                req.update(val)
                commods[key] = [req]
            elif isinstance(val, Sequence):
                newval = []
                for x in val:
                    req = default_req.copy()
                    if isinstance(x, ts.Resource):
                        req['target'] = x
                    elif isinstance(x, Mapping):
                        req.update(x)
                    else:
                        raise TypeError('Did not recognize type of request while '
                                        'converting to portfolio: ' + repr(inp))
                    newval.append(req)
                commods[key] = newval
            else:
                raise TypeError('Did not recognize type of commodity while '
                                'converting to portfolio: ' + repr(inp))

    cdef dict rtn = {'commodities': commods, 'constraints': constrs}
    return rtn


cpdef dict normalize_bid_portfolio(object inp):
    """Normalizes a bid portfolio into a standard Python form, ready to be traded.
    Note that this does not include the bidder object.
    """
    # get initial values
    if not isinstance(inp, Mapping):
        bids = inp
        constrs = []
    elif 'bids' in inp:
        bids = inp['bids']
        constrs = inp.get('constraints', [])
    else:
        bids = [inp]
        constrs = []
    # canonize constraints
    if not isinstance(constrs, Iterable):
        constrs = [constrs]
    # canonize commods
    if not isinstance(bids, Sequence):
        bids = [bids]
    cdef dict default_bid = {'request': None, 'offer': None,
                             'preference': 1.0, 'exclusive': False}
    cdef int i, n
    cdef list normbids = []
    n = len(bids)
    for i in range(n):
        b = bids[i]
        bid = default_bid.copy()
        if isinstance(b, Mapping):
            bid.update(b)
        elif isinstance(b, Sequence):
            bid['request'], bid['offer'] = b
        elif isinstance(b, ts.request_types):
            bid['request'] = b
            bid['offer'] = b.target
        else:
            raise TypeError('Did not recognize type of bid while '
                            'converting to portfolio: ' + repr(inp))
        normbids.append(bid)
    cdef dict rtn = {'bids': normbids, 'constraints': constrs}
    return rtn


# This a cache for agents so Python doesn't gc them and end up deallocing early
cdef dict _AGENT_REFS = {}

cpdef object make_py_agent(object libname, object agentname, object ctx_capsule):
    """Makes a new Python agent instance."""
    global _AGENT_REFS
    mod = import_module(libname)
    cls = getattr(mod, agentname)
    ctx = Context(init=False)
    (<_Context> ctx).ptx = <cpp_cyclus.Context*> PyCapsule_GetPointer(ctx_capsule,
                                                                      <char*> b"ctx")
    agent = cls(ctx)
    (<_Agent> agent)._free = False
    _AGENT_REFS[agent.id] = agent
    rtn = PyCapsule_New((<_Agent> agent).ptx, <char*> b"agent", NULL)
    return rtn, agent.kind


cpdef void _clear_agent_refs():
    """Clears the agent references cache. Users should never need to call this."""
    global _AGENT_REFS
    _AGENT_REFS.clear()


cpdef void _del_agent(int i):
    """Clears a single agent from the reference cache Users should never need to
    call this.
    """
    global _AGENT_REFS
    if i in _AGENT_REFS:
        del _AGENT_REFS[i]

#
# Functions to allow for time series facilities to interaction with the timeseries
# callbacks.
#


POWER = cpp_cyclus.POWER
ENRICH_SWU = cpp_cyclus.ENRICH_SWU
ENRICH_FEED = cpp_cyclus.ENRICH_FEED

def record_time_series(object tstype, object agent, object value):
    """Python hook into RecordTimeSeries for Python archetypes

    Parameters
    ----------
    tstype : int or string
        Time series type flag; POWER, ENRICH_SWU, etc or the string flag.
    agent : object
        Python agent, usually self when called by an archetype.
    value : float
        The value being recorded in the time series.
    """
    cdef cpp_cyclus.Agent* a_ptr = dynamic_agent_ptr(agent)
    if isinstance(tstype, str):
        if isinstance(value, bool):
            cpp_cyclus.RecordTimeSeries[ts.bool_t](ts.std_string_to_cpp(tstype), a_ptr, ts.bool_to_cpp(value))
        elif isinstance(value, int):
            cpp_cyclus.RecordTimeSeries[int](ts.std_string_to_cpp(tstype), a_ptr, ts.int_to_cpp(value))
        elif isinstance(value, float):
            cpp_cyclus.RecordTimeSeries[double](ts.std_string_to_cpp(tstype), a_ptr, ts.double_to_cpp(value))
        elif isinstance(value, str):
            cpp_cyclus.RecordTimeSeries[ts.std_string_t](ts.std_string_to_cpp(tstype), a_ptr, ts.str_py_to_cpp(value))
        else:
            raise TypeError("Unsupported type in time series record")
    else:
        if tstype == POWER:
            cpp_cyclus.RecordTimeSeriesPower(a_ptr, value)
        elif tstype == ENRICH_SWU:
            cpp_cyclus.RecordTimeSeriesEnrichSWU(a_ptr, value)
        elif tstype == ENRICH_FEED:
            cpp_cyclus.RecordTimeSeriesEnrichFeed(a_ptr, value)


TIME_SERIES_LISTENERS = defaultdict(list)

def call_listeners(tsname, agent, time, value):
    """Calls the time series listener functions of cyclus agents.
    """
    vec = TIME_SERIES_LISTENERS[tsname]
    for f in vec:
        f(agent, time, value, tsname)


EXT_BACKENDS = {'.h5': Hdf5Back, '.sqlite': SqliteBack}

def dbopen(fname):
    """Opens a Cyclus database."""
    _, ext = os.path.splitext(fname)
    if ext not in EXT_BACKENDS:
        msg = ('The backend database type of {0!r} could not be determined from '
               'extension {1!r}.')
        raise ValueError(msg.format(fname, ext))
    db = EXT_BACKENDS[ext](fname)
    return db


#
# Position
#
cdef class _Position:

    def __cinit__(self, double latitude=0.0, double longitude=0.0):
        self.posptx = new cpp_cyclus.Position(latitude, longitude)

    def __dealloc__(self):
        del self.posptx

    def __str__(self):
        s = std_string_to_py(self.posptx.ToString())
        return s

    def __repr__(self):
        return str(self)

    @property
    def latitude(self):
        return self.posptx.latitude()

    @latitude.setter
    def latitude(self, double value):
        self.posptx.latitude(value)

    @property
    def longitude(self):
        return self.posptx.longitude()

    @longitude.setter
    def longitude(self, double value):
        self.posptx.longitude(value)

    def update(self, latitude=None, longitude=None):
        """Updates the latitude and/or longitude"""
        cdef double lat, lon
        lat = self.latitude() if latitude is None else latitude
        lon = self.longitude() if longitude is None else longitude
        self.posptx.set_position(lat, lon)

    def distance(self, other):
        """Computes the distance between this object and another position."""
        if not isinstance(other, _Position):
            msg = ("Can only compute distances between positions, "
                   "{0!r} ({1}) is not a position.")
            raise TypeError(msg.format(other, type(other)))
        d = self.posptx.Distance(deref((<_Position> other).posptx))
        return d


class Position(_Position):
    """a basic class that stores the geographic location
    in latitude and longitude and follows the ISO 6709 standard.
    Longitude and Latitude is stored as seconds of degrees.
    This allows the coordinate elements such as degrees,
    minutes, and seconds to "remain on the integral portion of values, with the
    exception of decimal of seconds, avoiding loss of precision." This is
    calculated by multiplying decimal degrees by 3600.
    example: 05.2169 -> 18780.84
    """

def CY_LARGE_DOUBLE():
    return cpp_cyclus.CY_LARGE_DOUBLE

def CY_LARGE_INT():
    return cpp_cyclus.CY_LARGE_INT

def CY_NEAR_ZERO():
    return cpp_cyclus.CY_NEAR_ZERO
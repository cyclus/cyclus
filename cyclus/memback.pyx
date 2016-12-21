"""An in-memory Python backend for Cyclus."""
from __future__ import print_function, unicode_literals
from libcpp.map cimport map as std_map
from libcpp.set cimport set as std_set
from libcpp.vector cimport vector as std_vector
from libcpp.utility cimport pair as std_pair
from libcpp.string cimport string as std_string
from libcpp cimport bool as cpp_bool

from cyclus cimport cpp_cyclus
from cyclus cimport lib
from cyclus import lib


cdef cppclass CyclusMemBack "CyclusMemBack" (cpp_cyclus.RecBackend):
    # A C++ class that acts as a rec backend, but stores its data in
    # memory in a Python friendly format.

    void Notify(cpp_cyclus.DatumList dl):
        """Stores the data in a cache."""

    std_string Name():
        """The name of the backend"""
        return std_string(b"<Python In-Memory Backend>")

    void Flush():
        """No-op for in-memory"""
        pass

    void Close():
        """No-op for in-memory"""
        pass


cdef class _MemBack(lib._FullBackend):

    def __cinit__(self):
        self.ptx = new CyclusMemBack()

    def __dealloc__(self):
        cdef CyclusMemBack* cpp_ptx = <CyclusMemBack*> self.ptx
        del cpp_ptx

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
        raise NotImplementedError

    @property
    def tables(self):
        raise NotImplementedError

    @tables.setter
    def tables(self, value):
        """Retrieves the set of tables present in the database."""
        raise NotImplementedError


    def flush(self):
        """No-op since in-memory."""
        # this is a no-op

    def close(self):
        """Closes the backend, flushing it in the process."""
        self.flush()  # just in case
        (<CyclusMemBack*> self.ptx).Close()

    @property
    def name(self):
        """The name of the database."""
        return "<Python In-Memory Backend at " + str(self.ptx) + ">"


class MemBack(_MemBack, lib.FullBackend):
    """An in-memory Python backend database.

    Note that even though the underlying C++ class is a RecBackend,
    the Python wrapper inherits from FullBackend and the QueryableBackend
    interface in implemented in Cython.
    """

"""Schemas for cymetric.
"""
from __future__ import print_function, unicode_literals
import sys
from collections import MutableSequence, Iterable
if sys.version_info[0] > 2:
    str_types = (str, bytes)
else:
    str_types = (str, unicode)

from cyclus import typesystem as ts
from cyclus.typesystem import NAMES, RANKS, IDS


def canon_dbtype(dbtype):
    """Returns the canonical form for a Cyclus database type."""
    if dbtype in NAMES:
        rtn = dbtype
    elif dbtype in IDS:
        rtn = IDS[dbtype]
    else:
        msg = 'the Cyclus type of {0!r} could not be determined.'
        raise TypeError(msg.format(dbtype))
    return rtn


def canon_shape(shape, dbtype):
    """Returns the canonical shape for a Cyclus database type."""
    if shape is None:
        rtn = None
    elif isinstance(shape, Iterable):
        if dbtype not in RANKS:
            dbtype = canon_dbtype(dbtype)
        s = []
        for i in shape:
            i = int(i)
            if i < -1:
                msg = '{0!r} of shape {1!r} must be >= -1.'
                raise ValueError(msg.format(i, shape))
            s.append(i)
        rank = RANKS[dbtype]
        if len(s) != rank:
            msg = ('the shape rank for the {0} type must be {1}, got {2} for a '
                   'shape of {3}.')
            raise ValueError(msg.format(NAMES[dbtype], rank, len(s), s))
        rtn = tuple(s)
    elif isinstance(shape, int):
        rtn = canon_shape((shape,), dbtype)
    else:
        msg = 'the shape {0!r} for a Cyclus type of {1!r} could not be determined.'
        raise TypeError(msg.format(shape, dbtype))
    return rtn


def canon_name(name):
    """Returns canonical form of a column name."""
    if not isinstance(name, str_types):
        raise ValueError('column name {0!r} must be a string'.format(name))
    return name


def canon_column(col):
    """Returns canonical form of a column."""
    if len(col) == 2:
        name, dbtype = col
        shape = None
    elif len(col) == 3:
        name, dbtype, shape = col
    else:
        msg = '{0!r} does not have length 2 or 3, cannot convert to schema.'
        raise ValueError(msg.format(col))
    name = canon_name(name)
    dbtype = canon_dbtype(dbtype)
    shape = canon_shape(shape, dbtype)
    return (name, dbtype, shape)


class SchemaProperty(MutableSequence):
    """Represents specific properties of a schema."""

    def __init__(self, obj, prop):
        self.obj = obj
        if prop == 'names':
            i = 0
            c = canon_name
        elif prop == 'dbtypes':
            i = 1
            c = canon_dbtype
        elif prop == 'shapes':
            i = 2
            c = canon_shape
        self.idx = i
        self.canonizer = c

    def _newcol(self, col, val):
        val = self.canonizer(val)
        col = list(col)
        col[self.idx] = val
        return tuple(col)

    def __get__(self, obj, objtype):
        return self

    def __set__(self, obj, val):
        if len(obj) != len(val):
            msg = 'The length of the schema ({0}) does not match the given length {1}.'
            raise ValueError(msg.format(len(obj), len(val)))
        newdescr = []
        for col, v in zip(obj, val):
            newdescr.append(self._newcol(col, v))
        obj.descr[:] = newdescr
        if self.idx == 0:
            obj.byte_names.clear()
            obj.byte_names.update({name: name.encode() for name, _, _ in newdescr})

    def __getitem__(self, i):
        return self.obj.descr[i][self.idx]

    def __setitem__(self, i, val):
        if self.idx == 0:
            del obj.byte_names[self.obj.descr[i][0]]
            obj.byte_names[val] = val.encode()
        self.obj.descr[i] = col = self._newcol(self.obj.descr[i], val)

    def __delitem__(self, i):
        raise AttributeError("May not delete from schema property.")

    def __len__(self):
        return len(self.obj)

    def insert(self, i, val):
        raise AttributeError("May not insert from schema property.")

    def __iter__(self):
        for col in self.obj.descr:
            yield col[self.idx]

    def __str__(self):
        s = '[' + ', '.join([repr(x) for x in self]) + ']'
        return s


class schema(MutableSequence):
    """A type represented compound schema composed of column names,
    associated types from the Cyclus type system, and optional shapes.
    """

    def __init__(self, x):
        """Parameters
        ----------
        x : iterable
            Sequence of 2- or 3-tuples that represent the schema.
        """
        self.descr = [canon_column(val) for val in x]
        self.names = SchemaProperty(self, 'names')
        self.dbtypes = SchemaProperty(self, 'dbtypes')
        self.shapes = SchemaProperty(self, 'shapes')
        self.byte_names = {name: name.encode() for name in self.names}

    def __getitem__(self, i):
        return self.descr[i]

    def __setitem__(self, i, val):
        self.descr[i] = canon_column(val)

    def __delitem__(self, i):
        del self.descr[i]

    def __len__(self):
        return len(self.descr)

    def insert(self, i, val):
        self.descr.insert(i, canon_column(val))

    def __iter__(self):
        for col in self.descr:
            yield col

    def __str__(self):
        s = '[' + ', '.join([repr(x) for x in self]) + ']'
        return s

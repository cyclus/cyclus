"""Execution for cymetric.
"""
from __future__ import unicode_literals, print_function
import re
import sys
import uuid
from collections import MutableMapping, Sized
if sys.version_info[0] > 2:
    str_types = (str, bytes)
else:
    str_types = (str, unicode)

import numpy as np
import pandas as pd

from cymetric.evaluator import Evaluator, METRIC_REGISTRY


class ColumnProxy(object):
    """A proxy object for column that returns condition 3-tuples from
    comparison operations.
    """

    def __init__(self, name):
        """Parameters
        ----------
        name : str
            The column name.
        """
        self.name = name
        self.conds = []

    def __call__(self, *args, **kwargs):
        """Stub function that prevents ColumnProxies from being called."""
        raise TypeError('ColumnProxy object {0!r} is not callable'.format(self.name))

    def __getitem__(self, *args, **kwargs):
        """Stub function that prevents ColumnProxies from being indexed."""
        raise TypeError('ColumnProxy object {0!r} cannot be indexed'.format(self.name))

    def __lt__(self, other):
        """Less than operator for column filtering."""
        self.conds.append((self.name, '<', other))
        return self

    def __gt__(self, other):
        """Greater than operator for column filtering."""
        self.conds.append((self.name, '>', other))
        return self

    def __le__(self, other):
        """Less than or equal operator for column filtering."""
        self.conds.append((self.name, '<=', other))
        return self

    def __ge__(self, other):
        """Greater than or equal operator for column filtering."""
        self.conds.append((self.name, '>=', other))
        return self
        return self.name, '>=', other

    def __eq__(self, other):
        """Equal operator for column filtering."""
        self.conds.append((self.name, '==', other))
        return self

    def __ne__(self, other):
        """Not equal operator for column filtering."""
        self.conds.append((self.name, '!=', other))
        return self


stripper = lambda s: s.strip()

COND_RE = re.compile('\s*(\w+)\s*(<|>|<=|>=|==|!=)\s*(.*)')

def parse_cond(cond):
    """Parses a condition and returns the canonical 3-tuple."""
    if isinstance(cond, ColumnProxy):
        return cond.conds
    if not isinstance(cond, str_types):
        return cond
    m = COND_RE.match(cond)
    if m is None:
        msg = 'Could not parse condition from {0}'
        raise ValueError(msg.format(cond))
    c = tuple(map(stripper, m.groups()))
    return c


EMPTY_SLICE = slice(None)

def has_no_conds(key):
    """Determines if a key means that there are no conditions given."""
    if isinstance(key, ColumnProxy):
        return False
    return ((key is Ellipsis) or (key is None) or (key == EMPTY_SLICE) or
        (isinstance(key, Sized) and len(key) == 0))


class MetricProxy(object):
    """A proxy metric for nicer spelling of metrics in an execution context.
    Objects of this class are bound to an evaluator and will return a data frame
    when indexed. Index parameters are interpreted as query conditions.
    """

    def __init__(self, name, evaler=None):
        """Parameters
        ----------
        name : str
            The metric name.
        evaler : cymetric.evaluator.Evaluator, optional
            The evaluator for the metrics, required if db not given.
        """
        self.name = name
        self.evaler = evaler

    def __getitem__(self, key):
        """Evaluates if conditions are given for evaluation of metric and
        formats them.
        """
        if has_no_conds(key):
            conds = None
        elif isinstance(key, ColumnProxy):
            conds = key.conds
        elif isinstance(key, str_types):
            conds = [parse_cond(key)]
        else:
            conds = []
            for k in key:
                if isinstance(k, ColumnProxy):
                    conds += k.conds
                elif isinstance(k, str_types):
                    conds.append(parse_cond(k))
        return self.evaler.eval(self.name, conds=conds)


class ExecutionContext(MutableMapping):
    """An execution context for the command line or any other situation
    that involves the automatic injection of metric names, column names,
    etc.
    """

    def __init__(self, evaler=None, db=None, *args, **kwargs):
        """Parameters
        ----------
        evaler : cymetric.evaluator.Evaluator, optional
            The evaluator for the metrics, required if db not given.
        db : database, optional
            Required if evaler not given.
        args, kwargs : tuple, dict
            Other arguments to the mutable mapping.
        """
        self._ctx = ctx = {}
        self.evaler = evaler or evaluator.Evaluator(db)
        import cymetric as cym  # lazy import needed
        ctx['cym'] = cym
        ctx['np'] = np
        ctx['pd'] = pd
        ctx['uuid'] = uuid
        try:
            import matplotlib
            import matplotlib.pyplot as plt
            ctx['matplotlib'] = matplotlib
            ctx['plt'] = plt
        except ImportError:
            pass
        ctx.update(*args, **kwargs)

    def __getitem__(self, key):
        """Retrieves metric from registry or column from root metrics."""
        if key in __builtins__:
            raise KeyError
        elif key in METRIC_REGISTRY:
            self._ctx[key] = MetricProxy(key, self.evaler)
        elif key not in self._ctx:
            self._ctx[key] = ColumnProxy(key)
        return self._ctx[key]

    def __setitem__(self, key, value):
        """Sets metric with value indexed by key."""
        self._ctx[key] = value

    def __delitem__(self, key):
        """Deletes metric indexed by key."""
        del self._ctx[key]

    def __iter__(self):
        """Indexes metric by key."""
        for key in self._ctx:
            yield key

    def __len__(self):
        """Returns length of metric dict."""
        return len(self._ctx)

    def keys(self):
        """Returns copy of metric keys."""
        return self._ctx.keys()

    def values(self):
        """Returns copy of metric values."""
        return self._ctx.values()

    def items(self):
        """Returns copy of metrics in (key, value) form."""
        return self._ctx.items()

    def __del__(self):
        """Closes db, flushing remaining buffers."""
        self.evaler.db.flush()


def exec_code(code, db, write=True):
    """Runs a code snipper in the context of a database."""
    evaler = Evaluator(db, write=write)
    glb = {}
    loc = ExecutionContext(evaler=evaler)
    exec(code, glb, loc)

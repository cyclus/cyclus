"""Cymetric: The Cyclus Analysis Toolkit"""
from __future__ import unicode_literals, print_function

from cyclus.typesystem import *  # only grabs code generated defintiions
from cyclus.lib import Datum, FullBackend, SqliteBack, Hdf5Back, \
        Recorder

try:
    from cymetric.tools import dbopen
    from cymetric.schemas import schema, canon_dbtype, canon_shape, \
        canon_column, canon_name
    from cymetric.root_metrics import root_metric
    from cymetric.metrics import Metric, metric
    from cymetric.evaluator import METRIC_REGISTRY, register_metric, \
        raw_to_series, Evaluator, eval
    from cymetric.execution import ExecutionContext, exec_code
except ImportError:
    # again with the wacky CI issues
    from .tools import dbopen
    from .schemas import schema, canon_dbtype, canon_shape, \
        canon_column, canon_name
    from .root_metrics import root_metric
    from .metrics import Metric, metric
    from .evaluator import METRIC_REGISTRY, register_metric, \
        raw_to_series, Evaluator, eval
    from .execution import ExecutionContext, exec_code

__version__ = '1.5.3'


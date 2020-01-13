"""Tests for timseries method"""
from __future__ import print_function, unicode_literals
from uuid import UUID
import os
import subprocess
from functools import wraps

import nose
from nose.tools import assert_equal, assert_less
from nose.plugins.skip import SkipTest

import numpy as np
import pandas as pd
from pandas.util.testing import assert_frame_equal


from tools import setup, dbtest

import cymetric as cym
from cymetric import timeseries as ts
from cymetric.tools import raw_to_series, ensure_dt_bytes

try:
    from pyne import data
    import pyne.enrichment as enr
    HAVE_PYNE = True
except ImportError:
    HAVE_PYNE = False


@dbtest
def test_timeseries_transactions(db, fname, backend):
    evaler = cym.Evaluator(db)
    cal = ts.transactions(evaler)
    exp_head = ['Time', 'Mass']
    assert_equal(list(cal), exp_head)  # Check we have the correct headers

    if not HAVE_PYNE:
        raise SkipTest
    # test single nuclide selection
    cal = ts.transactions(evaler, nucs=['942390000'])
    refs = pd.DataFrame(np.array([
        (0, 0.000000000),
        (1, 0.0444814879803),
        (2, 0.0889629759607),
        (3, 0.0889629759607),
        (4, 0.0889629759607),
    ], dtype=ensure_dt_bytes([
        ('Time', '<i8'), ('Mass', '<f8')
    ]))
    )
    assert_frame_equal(cal, refs)

    # test multiple nuclide selection
    cal = ts.transactions(
        evaler, nucs=['942390000', '922380000'])
    refs = pd.DataFrame(np.array([
        (0, 0.000000000),
        (1, 0.831724864011),
        (2, 1.66344972802),
        (3, 2.62344972802),
        (4, 2.62344972802),
    ], dtype=ensure_dt_bytes([
        ('Time', '<i8'), ('Mass', '<f8')
    ]))
    )
    assert_frame_equal(cal, refs)


@dbtest
def test_timeseries_transactions_activity(db, fname, backend):
    if not HAVE_PYNE:
        raise SkipTest
    evaler = cym.Evaluator(db)
    cal = ts.transactions_activity(evaler)
    exp_head = ['Time', 'Activity']
    assert_equal(list(cal), exp_head)  # Check we have the correct headers

    # test single nuclide selection
    cal = ts.transactions_activity(
        evaler, nucs=['942390000'])
    refs = pd.DataFrame(np.array([
        (0, 0.000000000),
        (1, 102084984531.0),
        (2, 204169969062.0),
        (3, 204169969062.0),
        (4, 204169969062.0),
    ], dtype=ensure_dt_bytes([
        ('Time', '<i8'), ('Activity', '<f8')
    ]))
    )
    assert_frame_equal(cal, refs)

    # test multiple nuclide selection
    cal = ts.transactions_activity(
        evaler, nucs=['942390000', '922380000'])
    refs = pd.DataFrame(np.array([
        (0, 0.000000000),
        (1, 102094774891.0),
        (2, 204189549782.0),
        (3, 204201488588.0),
        (4, 204201488588.0),
    ], dtype=ensure_dt_bytes([
        ('Time', '<i8'), ('Activity', '<f8')
    ]))
    )
    assert_frame_equal(cal, refs)


@dbtest
def test_timeseries_transactions_decayheat(db, fname, backend):
    if not HAVE_PYNE:
        raise SkipTest
    evaler = cym.Evaluator(db)
    cal = ts.transactions_decayheat(evaler)
    exp_head = ['Time', 'DecayHeat']
    assert_equal(list(cal), exp_head)  # Check we have the correct headers

    # test single nuclide selection
    cal = ts.transactions_decayheat(
        evaler, nucs=['942390000'])
    refs = pd.DataFrame(np.array([
        (0, 0.000000000),
        (1, 3.34065303191e+30),
        (2, 6.68130606382e+30),
        (3, 6.68130606382e+30),
        (4, 6.68130606382e+30),
    ], dtype=ensure_dt_bytes([
        ('Time', '<i8'), ('DecayHeat', '<f8')
    ]))
    )
    assert_frame_equal(cal, refs)

    # test multiple nuclide selection
    cal = ts.transactions_decayheat(
        evaler, nucs=['942390000', '922380000'])
    refs = pd.DataFrame(np.array([
        (0, 0.000000000),
        (1, 3.34091395721e+30),
        (2, 6.68182791443e+30),
        (3, 6.68214609848e+30),
        (4, 6.68214609848e+30),
    ], dtype=ensure_dt_bytes([
        ('Time', '<i8'), ('DecayHeat', '<f8')
    ]))
    )
    assert_frame_equal(cal, refs)


@dbtest
def test_timeseries_inventories(db, fname, backend):
    evaler = cym.Evaluator(db)
    cal = ts.inventories(evaler)
    exp_head = ['Time', 'Quantity']
    assert_equal(list(cal), exp_head)  # Check we have the correct headers

    if not HAVE_PYNE:
        raise SkipTest
    cal = ts.inventories(evaler, facilities=['Reactor1'],
                                      nucs=['94239'])
    refs = pd.DataFrame(np.array([
        (0, 0.0),
        (1, 0.0444814879803),
        (2, 0.0621806030246),
        (3, 0.0798797180688),
        (4, 0.0530973451327)
    ], dtype=ensure_dt_bytes([
        ('Time', '<i8'), ('Quantity', '<f8')
    ]))
    )
    assert_frame_equal(cal, refs)

    cal = ts.inventories(evaler, facilities=['Reactor1'],
                                      nucs=['94239', '92235'])
    refs = pd.DataFrame(np.array([
        (0, 0.0),
        (1, 0.0460607124057),
        (2, 0.0726093849721),
        (3, 0.0991580575384),
        (4, 0.119646017699)
    ], dtype=ensure_dt_bytes([
        ('Time', '<i8'), ('Quantity', '<f8')
    ]))
    )
    assert_frame_equal(cal, refs)


@dbtest
def test_timeseries_inventories_activity(db, fname, backend):
    if not HAVE_PYNE:
        raise SkipTest
    evaler = cym.Evaluator(db)
    cal = ts.inventories_activity(evaler)
    exp_head = ['Time', 'Activity']
    assert_equal(list(cal), exp_head)  # Check we have the correct headers

    cal = ts.inventories_activity(evaler, facilities=['Reactor1'],
                                               nucs=['94239'])
    refs = pd.DataFrame(np.array([
        (0, 0.0),
        (1, 2.44036364223e+13),
        (2, 3.41138054869e+13),
        (3, 4.38239745515e+13),
        (4, 2.91305071939e+13)
    ], dtype=ensure_dt_bytes([
        ('Time', '<i8'), ('Activity', '<f8')
    ]))
    )
    assert_frame_equal(cal, refs)

    cal = ts.inventories_activity(evaler, facilities=['Reactor1'],
                                               nucs=['94239', '92235'])
    refs = pd.DataFrame(np.array([
        (0, 0.0),
        (1, 2.4403666094e+13),
        (2, 3.41140014315e+13),
        (3, 4.3824336769e+13),
        (4, 2.91317575657e+13),
    ], dtype=ensure_dt_bytes([
        ('Time', '<i8'), ('Activity', '<f8')
    ]))
    )
    assert_frame_equal(cal, refs)


@dbtest
def test_timeseries_inventories_decayheat(db, fname, backend):
    if not HAVE_PYNE:
        raise SkipTest
    evaler = cym.Evaluator(db)
    cal = ts.inventories_decayheat(evaler)
    exp_head = ['Time', 'DecayHeat']
    assert_equal(list(cal), exp_head)  # Check we have the correct headers

    cal = ts.inventories_decayheat(evaler, facilities=['Reactor1'],
                                                nucs=['94239'])
    refs = pd.DataFrame(np.array([
        (0, 0.0),
        (1, 7.98590335085e+32),
        (2, 1.11634819022e+33),
        (3, 1.43410604536e+33),
        (4, 9.53273565408e+32)
    ], dtype=ensure_dt_bytes([
        ('Time', '<i8'), ('DecayHeat', '<f8')
    ]))
    )
    assert_frame_equal(cal, refs)

    cal = ts.inventories_decayheat(evaler, facilities=['Reactor1'],
                                                nucs=['94239', '92235'])
    refs = pd.DataFrame(np.array([
        (0, 0.0),
        (1, 7.98591200694e+32),
        (2, 1.11635390648e+33),
        (3, 1.43411661226e+33),
        (4, 9.53310042276e+32)
    ], dtype=ensure_dt_bytes([
        ('Time', '<i8'), ('DecayHeat', '<f8')
    ]))
    )
    assert_frame_equal(cal, refs)


if __name__ == "__main__":
    nose.runmodule()

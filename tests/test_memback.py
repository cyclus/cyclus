"""Tests Python memory backend."""
from __future__ import print_function, unicode_literals

import nose
from nose.tools import assert_equal, assert_true

from cyclus import memback
from cyclus import lib
from cyclus import typesystem as ts

import numpy as np
import pandas as pd
from pandas.util.testing import assert_frame_equal


def make_rec_back():
    """Makes a new recorder and backend."""
    rec = lib.Recorder(inject_sim_id=False)
    back = memback.MemBack()
    rec.register_backend(back)
    return rec, back


def test_simple():
    rec, back = make_rec_back()
    d = rec.new_datum("test")
    d.add_val("col0", 1, dbtype=ts.INT)
    d.add_val("col1", 42.0, dbtype=ts.DOUBLE)
    d.add_val("col2", "wakka", dbtype=ts.VL_STRING)
    d.record()
    rec.flush()

    exp = pd.DataFrame({"col0": [1], "col1": [42.0], "col2": ["wakka"]},
                       columns=['col0', 'col1', 'col2'])
    obs = back.query("test")
    assert_frame_equal(exp, obs)
    rec.close()


def test_many_rows_one_table():
    n = 10
    rec, back = make_rec_back()
    for i in range(n):
        d = rec.new_datum("test")
        d.add_val("col0", i, dbtype=ts.INT)
        d.add_val("col1", 42.0*i, dbtype=ts.DOUBLE)
        d.add_val("col2", "wakka"*i, dbtype=ts.VL_STRING)
        d.record()
    rec.flush()

    exp = pd.DataFrame({
        "col0": list(range(n)),
        "col1": [42.0*i for i in range(n)],
        "col2": ["wakka"*i for i in range(n)]},
        columns=['col0', 'col1', 'col2'])
    obs = back.query("test")
    assert_frame_equal(exp, obs)
    rec.close()


def test_two_tables_interleaved():
    n = 10
    rec, back = make_rec_back()
    for i in range(n):
        d = rec.new_datum("test0" if i%2 == 0 else "test1")
        d.add_val("col0", i, dbtype=ts.INT)
        d.add_val("col1", 42.0*i, dbtype=ts.DOUBLE)
        d.add_val("col2", "wakka"*i, dbtype=ts.VL_STRING)
        d.record()
    rec.flush()

    exp0 = pd.DataFrame({
        "col0": list(range(0, n, 2)),
        "col1": [42.0*i for i in range(0, n, 2)],
        "col2": ["wakka"*i for i in range(0, n, 2)]},
        columns=['col0', 'col1', 'col2'])
    obs0 = back.query("test0")
    yield assert_frame_equal, exp0, obs0


    exp1 = pd.DataFrame({
        "col0": list(range(1, n, 2)),
        "col1": [42.0*i for i in range(1, n, 2)],
        "col2": ["wakka"*i for i in range(1, n, 2)]},
        columns=['col0', 'col1', 'col2'])
    obs1 = back.query("test1")
    yield assert_frame_equal, exp1, obs1
    rec.close()


if __name__ == "__main__":
    nose.runmodule()
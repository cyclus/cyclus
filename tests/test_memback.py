"""Tests Python memory backend."""
from __future__ import print_function, unicode_literals

import nose
from nose.tools import assert_equal, assert_true, assert_is_instance, \
    assert_in, assert_false, assert_not_in

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


def make_two_interleaved(rec, n):
    for i in range(n):
        d = rec.new_datum("test0" if i%2 == 0 else "test1")
        d.add_val("col0", i, dbtype=ts.INT)
        d.add_val("col1", 42.0*i, dbtype=ts.DOUBLE)
        d.add_val("col2", "wakka"*i, dbtype=ts.VL_STRING)
        d.record()
    rec.flush()


def test_two_tables_interleaved():
    n = 10
    rec, back = make_rec_back()
    make_two_interleaved(rec, n)

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



def test_three_tables_grouped():
    names = ["test0", "test1", "test2"]
    n = 10
    rec, back = make_rec_back()
    for j, name in enumerate(names):
        for i in range(n):
            d = rec.new_datum(name)
            if j%3 != 0:
                d.add_val("col0", i*j, dbtype=ts.INT)
            if j%3 != 1:
                d.add_val("col1", 42.0*i*j, dbtype=ts.DOUBLE)
            if j%3 != 2:
                d.add_val("col2", "wakka"*i, dbtype=ts.VL_STRING)
            d.record()
    rec.flush()

    j = 0
    exp0 = pd.DataFrame({
        "col1": [42.0*i*j for i in range(n)],
        "col2": ["wakka"*i for i in range(n)]},
        columns=['col1', 'col2'])
    obs0 = back.query("test0")
    yield assert_frame_equal, exp0, obs0

    j = 1
    exp1 = pd.DataFrame({
        "col0": [i*j for i in range(n)],
        "col2": ["wakka"*i for i in range(n)]},
        columns=['col0', 'col2'])
    obs1 = back.query("test1")
    yield assert_frame_equal, exp1, obs1

    j = 2
    exp2 = pd.DataFrame({
        "col0": [i*j for i in range(n)],
        "col1": [42.0*i*j for i in range(n)]},
        columns=['col0', 'col1'])
    obs2 = back.query("test2")
    yield assert_frame_equal, exp2, obs2
    rec.close()


def test_record_flush_twice():
    n = 10
    rec, back = make_rec_back()
    for i in range(n//2):
        d = rec.new_datum("test")
        d.add_val("col0", i, dbtype=ts.INT)
        d.add_val("col1", 42.0*i, dbtype=ts.DOUBLE)
        d.add_val("col2", "wakka"*i, dbtype=ts.VL_STRING)
        d.record()
    rec.flush()
    for i in range(n//2, n):
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


def test_record_flush_thrice():
    n = 9
    rec, back = make_rec_back()
    for i in range(n//3):
        d = rec.new_datum("test")
        d.add_val("col0", i, dbtype=ts.INT)
        d.add_val("col1", 42.0*i, dbtype=ts.DOUBLE)
        d.add_val("col2", "wakka"*i, dbtype=ts.VL_STRING)
        d.record()
    rec.flush()
    for i in range(n//3, 2*n//3):
        d = rec.new_datum("test")
        d.add_val("col0", i, dbtype=ts.INT)
        d.add_val("col1", 42.0*i, dbtype=ts.DOUBLE)
        d.add_val("col2", "wakka"*i, dbtype=ts.VL_STRING)
        d.record()
    rec.flush()
    for i in range(2*n//3, n):
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


def test_many_cols_one_table():
    n = 100
    rec, back = make_rec_back()
    d = rec.new_datum("test")
    for i in range(n):
        d.add_val("col" + str(i), i, dbtype=ts.INT)
    d.record()
    rec.flush()

    cols = ["col" + str(i) for i in range(n)]
    exp = pd.DataFrame({c: [i] for c, i in zip(cols, range(n))}, columns=cols)
    obs = back.query("test")
    assert_frame_equal(exp, obs)
    rec.close()


def test_registry_operations():
    n = 10
    rec, back = make_rec_back()
    yield assert_true, back.store_all_tables
    rec.flush()  # test empty datalist

    # test storing only one table
    back.registry = ["test0"]
    yield assert_false, back.store_all_tables
    yield assert_is_instance, back.registry, frozenset
    yield assert_equal, 1, len(back.registry)
    yield assert_equal, 0, len(back.cache)
    make_two_interleaved(rec, n)
    yield assert_equal, 1, len(back.cache)
    yield assert_in, "test0", back.cache

    # test removing registry with False
    back.registry = False
    yield assert_false, back.store_all_tables
    yield assert_is_instance, back.registry, frozenset
    yield assert_equal, 0, len(back.cache)
    rec.flush()

    # test partial registry
    back.registry = ["test0", "test1"]
    yield assert_false, back.store_all_tables
    yield assert_is_instance, back.registry, frozenset
    yield assert_equal, 2, len(back.registry)
    yield assert_equal, 0, len(back.cache)
    make_two_interleaved(rec, n)
    yield assert_equal, 2, len(back.cache)
    yield assert_in, "test0", back.cache
    yield assert_in, "test1", back.cache
    # stop following test1
    back.registry = ["test0", "test42", "test43"]
    yield assert_equal, 3, len(back.registry)
    yield assert_equal, 1, len(back.cache)
    yield assert_in, "test0", back.cache
    yield assert_not_in, "test1", back.cache

    # test removing registry with None
    back.registry = None
    yield assert_false, back.store_all_tables
    yield assert_is_instance, back.registry, frozenset
    yield assert_equal, 0, len(back.cache)
    rec.close()



if __name__ == "__main__":
    nose.runmodule()
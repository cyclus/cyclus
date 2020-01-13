"""Tests for root metrics"""
from __future__ import print_function, unicode_literals
import os
import subprocess
from functools import wraps

import nose
from nose.tools import assert_equal, assert_less

from cymetric import root_metrics

from tools import setup, dbtest


@dbtest
def test_resources(db, fname, backend):
    r = root_metrics.resources(db=db)
    obs = r()
    assert_less(0, len(obs))
    assert_equal('Resources', r.name)

@dbtest
def test_compositions(db, fname, backend):
    r = root_metrics.compositions(db=db)
    obs = r()
    assert_less(0, len(obs))
    assert_equal('Compositions', r.name)

@dbtest
def test_recipes(db, fname, backend):
    r = root_metrics.recipes(db=db)
    obs = r()
    assert_less(0, len(obs))
    assert_equal('Recipes', r.name)

@dbtest
def test_products(db, fname, backend):
    r = root_metrics.products(db=db)
    obs = r()
    if obs is None:
        return
    assert_less(0, len(obs))
    assert_equal('Products', r.name)

@dbtest
def test_res_creators(db, fname, backend):
    r = root_metrics.res_creators(db=db)
    obs = r()
    assert_less(0, len(obs))
    assert_equal('ResCreators', r.name)

@dbtest
def test_agent_entry(db, fname, backend):
    r = root_metrics.agent_entry(db=db)
    obs = r()
    assert_less(0, len(obs))
    assert_equal('AgentEntry', r.name)

@dbtest
def test_agent_exit(db, fname, backend):
    r = root_metrics.agent_exit(db=db)
    obs = r()
    if obs is None:
        return
    assert_less(0, len(obs))
    assert_equal('AgentExit', r.name)

@dbtest
def test_transactions(db, fname, backend):
    r = root_metrics.transactions(db=db)
    obs = r()
    assert_less(0, len(obs))
    assert_equal('Transactions', r.name)

@dbtest
def test_info(db, fname, backend):
    r = root_metrics.info(db=db)
    obs = r()
    assert_less(0, len(obs))
    assert_equal('Info', r.name)

@dbtest
def test_finish(db, fname, backend):
    r = root_metrics.finish(db=db)
    obs = r()
    assert_less(0, len(obs))
    assert_equal('Finish', r.name)

@dbtest
def test_input_files(db, fname, backend):
    r = root_metrics.input_files(db=db)
    obs = r()
    assert_less(0, len(obs))
    assert_equal('InputFiles', r.name)

@dbtest
def test_decom_schedule(db, fname, backend):
    r = root_metrics.decom_schedule(db=db)
    obs = r()
    if obs is None:
        return
    assert_less(0, len(obs))
    assert_equal('DecomSchedule', r.name)

@dbtest
def test_build_schedule(db, fname, backend):
    r = root_metrics.build_schedule(db=db)
    obs = r()
    assert_less(0, len(obs))
    assert_equal('BuildSchedule', r.name)

@dbtest
def test_snapshots(db, fname, backend):
    r = root_metrics.snapshots(db=db)
    obs = r()
    assert_less(0, len(obs))
    assert_equal('Snapshots', r.name)


@dbtest
def test_inventories(db, fname, backend):
    r = root_metrics.explicit_inventory(db=db)
    obs = r()
    assert_less(0, len(obs))
    assert_equal('ExplicitInventory', r.name)


@dbtest
def test_inventories_compact(db, fname, backend):
    r = root_metrics.explicit_inventory_compact(db=db)
    obs = r()
    assert_less(0, len(obs))
    assert_equal('ExplicitInventoryCompact', r.name)


@dbtest
def test_resources_non_existent_filter(db, fname, backend):
    r = root_metrics.resources(db=db)
    obs = r(conds=[('NotAColumn', '!=', 'not-a-value')])
    assert_less(0, len(obs))
    assert_equal('Resources', r.name)


if __name__ == "__main__":
    nose.runmodule()

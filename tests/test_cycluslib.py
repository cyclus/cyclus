"""Tests for cyclus wrappers"""
import os
import subprocess
from functools import wraps

import nose
from nose.tools import assert_equal, assert_less

from cyclus import lib

from tools import libcyclus_setup, dbtest


setup = libcyclus_setup

@dbtest
def test_name(db, fname, backend):
    obs = db.name
    assert_equal(fname, obs)


@dbtest
def test_simid(db, fname, backend):
    df = db.query("AgentEntry")
    simid = df['SimId']
    exp = simid[0]
    for obs in simid:
        assert_equal(exp, obs)

@dbtest
def test_conds_ae(db, fname, backend):
    obs = db.query("AgentEntry", [('Kind', '==', 'Region')])
    assert_equal(1, len(obs))
    assert_equal('Region', obs['Kind'][0])
    assert_equal(':agents:NullRegion', obs['Spec'][0])


@dbtest
def test_conds_comp(db, fname, backend):
    conds = [('NucId', '==', 922350000), ('MassFrac', '<=', 0.0072)]
    df = db.query("Compositions", conds)
    assert_less(0, len(df))
    for row in df['MassFrac']:
        assert_less(row, 0.00720000001)

@dbtest
def test_schema(db, fname, backend):
    schema = db.schema("AgentEntry")
    assert_equal(8, len(schema))
    cols = ["SimId", "AgentId", "Kind", "Spec", "Prototype", "ParentId", "Lifetime", "EnterTime"]
    dbs = [7, 1, 5, 5, 5, 1, 1, 1]
    for i, ci in enumerate(schema):
        assert_equal("AgentEntry", ci.table)
        assert_equal(cols[i], ci.col)
        assert_equal(dbs[i], ci.dbtype)
        assert_equal(i, ci.index)
        assert_equal(1, len(ci.shape))
        assert_equal(-1, ci.shape)

if __name__ == "__main__":
    nose.runmodule()

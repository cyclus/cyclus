"""Tests for fco metrics."""
from __future__ import print_function, unicode_literals
from uuid import UUID

import nose
from nose.tools import assert_equal, assert_less
from nose.plugins.skip import SkipTest

import numpy as np
import pandas as pd
from pandas.util.testing import assert_frame_equal

try:
    from pyne import data
    import pyne.enrichment as enr
    HAVE_PYNE = True
except ImportError:
    HAVE_PYNE = False

from cymetric import fco_metrics
from cymetric.tools import raw_to_series, ensure_dt_bytes

#################################
####### FCO METRICS TESTS #######
#################################

def test_fco_u_mined():
    if not HAVE_PYNE:
        raise SkipTest
    exp = pd.DataFrame(np.array([(0, 3.780034), (1, 2.185349)],
        dtype=ensure_dt_bytes([('Year', '<i8'), ('UMined', '<f8')]))
        )
    mats = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 7, 3, 3, 922350000, 8.328354),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 7, 3, 3, 922380000, 325.004979),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 6, 8, 4, 3, 922350000, 11.104472),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 6, 8, 4, 3, 922380000, 322.228861),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 7, 9, 5, 12, 922350000, 11.104472),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 7, 9, 5, 12, 922380000, 322.228861),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('QualId', '<i8'), ('ResourceId', '<i8'),
                ('ObjId', '<i8'), ('TimeCreated', '<i8'), ('NucId', '<i8'),
                ('Mass', '<f8')]))
        )
    trans = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 7, 'LWR Fuel'),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 8, 'LWR Fuel'),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 3, 9, 'LWR Fuel'),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('TransactionId', '<i8'), ('ResourceId', '<i8'),
                ('Commodity', 'O')]))
        )
    obs = fco_metrics.fco_u_mined.func(mats, trans)
    assert_frame_equal(exp, obs)


def test_fco_swu():
    if not HAVE_PYNE:
        raise SkipTest
    exp = pd.DataFrame(np.array([(0, 0.002407), (1, 0.001473)],
        dtype=ensure_dt_bytes([('Year', '<i8'), ('SWU', '<f8')]))
        )
    mats = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 7, 3, 3, 922350000, 8.328354),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 7, 3, 3, 922380000, 325.004979),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 6, 8, 4, 3, 922350000, 11.104472),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 6, 8, 4, 3, 922380000, 322.228861),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 7, 9, 5, 12, 922350000, 11.104472),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 7, 9, 5, 12, 922380000, 322.228861),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('QualId', '<i8'), ('ResourceId', '<i8'),
                ('ObjId', '<i8'), ('TimeCreated', '<i8'), ('NucId', '<i8'),
                ('Mass', '<f8')]))
        )
    trans = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 7, 'LWR Fuel'),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 8, 'LWR Fuel'),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 3, 9, 'LWR Fuel'),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('TransactionId', '<i8'), ('ResourceId', '<i8'),
                ('Commodity', 'O')]))
        )
    obs = fco_metrics.fco_swu.func(mats, trans)
    np.allclose(exp, obs)


def test_fco_fuel_loading():
    exp = pd.DataFrame(np.array([(0, 0.666666), (1, 0.333333)],
        dtype=ensure_dt_bytes([('Year', '<i8'), ('FuelLoading', '<f8')]))
        )
    mats = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 7, 3, 3, 922350000, 8.328354),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 7, 3, 3, 922380000, 325.004979),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 6, 8, 4, 3, 922350000, 11.104472),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 6, 8, 4, 3, 922380000, 322.228861),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 7, 9, 5, 12, 922350000, 11.104472),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 7, 9, 5, 12, 922380000, 322.228861),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('QualId', '<i8'), ('ResourceId', '<i8'),
                ('ObjId', '<i8'), ('TimeCreated', '<i8'), ('NucId', '<i8'),
                ('Mass', '<f8')]))
        )
    trans = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 7, 'LWR Fuel'),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 8, 'FR Fuel'),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 3, 9, 'FR Fuel'),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('TransactionId', '<i8'), ('ResourceId', '<i8'),
                ('Commodity', 'O')]))
        )
    obs = fco_metrics.fco_fuel_loading.func(mats, trans)
    assert_frame_equal(exp, obs)


def test_fco_electricity_generated():
    exp = pd.DataFrame(np.array([(0, 3),
				 (1, 3)
	], dtype=ensure_dt_bytes([
	        ('Year', '<i8'), ('Energy', '<f8')]))
        )
    eg = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 0, 1000),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 0, 2000),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 3, 1, 3000),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('AgentId', '<i8'), ('Year', '<i8'),
                ('Energy', '<f8')]))
        )
    obs = fco_metrics.fco_electricity_generated.func(eg)
    assert_frame_equal(exp, obs)


def test_fco_monthly_electricity_generated():
    exp = pd.DataFrame(np.array([(0, 3),
				 (1, 3)
	], dtype=ensure_dt_bytes([
	        ('Month', '<i8'), ('Energy', '<f8')]))
        )
    eg = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 0, 1000),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 0, 2000),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 3, 1, 3000),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('AgentId', '<i8'), ('Month', '<i8'),
                ('Energy', '<f8')]))
        )
    obs = fco_metrics.fco_monthly_electricity_generated.func(eg)
    assert_frame_equal(exp, obs)

if __name__ == "__main__":
    nose.runmodule()

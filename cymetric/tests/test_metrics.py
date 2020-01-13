"""Tests for metrics. These test metric calculation functions
unbound to any database. This makes writing the tests easier in a unit
test like fashion.
"""
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

from cymetric import metrics
from cymetric.tools import raw_to_series, ensure_dt_bytes


def test_build_series():
    exp = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), -1, 'FRx', 1),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), -1, 'LWR', 1),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 'FRx', 1),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 'LWR', 2),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 'FRx', 2),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('EnterTime', '<i8'), ('Prototype', 'O'),
        		('Count', '<i8')]))
        )
    agent_entry = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 'FRx', -1),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 'LWR', -1),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 'LWR', 1),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 'FRx', 1),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 'LWR', 1),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 'FRx', 5),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 'FRx', 5),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('Prototype', 'O'), ('EnterTime', '<i8')]))
        )
    obs = metrics.build_series.func(agent_entry)
    assert_frame_equal(exp, obs)


def test_decommission_series():
    exp = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 10, 'LWR', 1),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 20, 'LWR', 2),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('ExitTime', '<i8'), ('Prototype', 'O'),
		        ('Count', '<i8')]))
        )
    agent_entry = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 'FRx'),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 'LWR'),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 3, 'LWR'),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 4, 'FRx'),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 'LWR'),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 6, 'FRx'),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 7, 'FRx'),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('AgentId', '<i8'), ('Prototype', 'O')]))
        )
    agent_exit = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 10),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 3, 20),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 20),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('AgentId', '<i8'), ('ExitTime', '<i8')]))
        )
    obs = metrics.decommission_series.func(agent_entry, agent_exit)
    assert_frame_equal(exp, obs)


def test_agents():
    exp = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 22, 'Region', ':agents:NullRegion', 'USA', -1, -1, 0, 120.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 23, 'Inst', ':agents:NullInst', 'utility', 22, -1, 0, 120.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 24, 'Facility', ':agents:Source', 'MineU235', 23, -1, 0, 120.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 25, 'Facility', ':agents:Source', 'U238', 23, -1, 0, 120.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 26, 'Facility', ':agents:Source', 'DU', 23, -1, 0, 120.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 27, 'Facility', ':agents:Source', 'DU2', 23, -1, 0, 120.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 28, 'Facility', ':Brightlite:FuelfabFacility', 'LWR Fuel FAb', 23, -1, 0, 120.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 29, 'Facility', ':Brightlite:ReprocessFacility', 'LWR Seperation', 23, -1, 0, 120.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 30, 'Facility', ':Brightlite:ReprocessFacility', 'FR Reprocess', 23, -1, 0, 120.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 31, 'Facility', ':agents:Sink', 'SINK', 23, -1, 0, 120.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 32, 'Facility', ':Brightlite:FuelfabFacility', 'FR Fuel Fab', 23, -1, 0, 120.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 33, 'Inst', ':cycamore:DeployInst', 'utility2', 22, -1, 0, 120.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 34, 'Facility', ':Brightlite:ReactorFacility', 'LWR', 33, -1, 5, 120.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 35, 'Facility', ':Brightlite:ReactorFacility', 'LWR', 33, -1, 5, 120.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 36, 'Facility', ':Brightlite:ReactorFacility', 'FRx', 33, -1, 10, 120.0),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('AgentId', '<i8'),
                ('Kind', 'O'), ('Spec', 'O'), ('Prototype', 'O'),
                ('ParentId', '<i8'), ('Lifetime', '<i8'),
                ('EnterTime', '<i8'), ('ExitTime', '<f8')]))
        )
    agent_entry = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 22, 'Region', ':agents:NullRegion', 'USA', -1, -1, 0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 23, 'Inst', ':agents:NullInst', 'utility', 22, -1, 0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 24, 'Facility', ':agents:Source', 'MineU235', 23, -1, 0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 25, 'Facility', ':agents:Source', 'U238', 23, -1, 0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 26, 'Facility', ':agents:Source', 'DU', 23, -1, 0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 27, 'Facility', ':agents:Source', 'DU2', 23, -1, 0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 28, 'Facility', ':Brightlite:FuelfabFacility', 'LWR Fuel FAb', 23, -1, 0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 29, 'Facility', ':Brightlite:ReprocessFacility', 'LWR Seperation', 23, -1, 0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 30, 'Facility', ':Brightlite:ReprocessFacility', 'FR Reprocess', 23, -1, 0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 31, 'Facility', ':agents:Sink', 'SINK', 23, -1, 0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 32, 'Facility', ':Brightlite:FuelfabFacility', 'FR Fuel Fab', 23, -1, 0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 33, 'Inst', ':cycamore:DeployInst', 'utility2', 22, -1, 0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 34, 'Facility', ':Brightlite:ReactorFacility', 'LWR', 33, -1, 5),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 35, 'Facility', ':Brightlite:ReactorFacility', 'LWR', 33, -1, 5),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 36, 'Facility', ':Brightlite:ReactorFacility', 'FRx', 33, -1, 10),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('AgentId', '<i8'),
                ('Kind', 'O'), ('Spec', 'O'), ('Prototype', 'O'),
                ('ParentId', '<i8'), ('Lifetime', '<i8'),
                ('EnterTime', '<i8')]))
        )
    info = pd.DataFrame({'Duration': {0: 120},
        'SimId': {0: UUID('f22f2281-2464-420a-8325-37320fd418f8')},
        })
    obs = metrics.agents.func(agent_entry, None, None, info)
    assert_frame_equal(exp, obs)


def test_materials():
    exp = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 9, 7, 1, 922350000, 'kg', 0.04),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 9, 7, 1, 922380000, 'kg', 1.94),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 9, 7, 1, 942390000, 'kg', 0.01),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('QualId', '<i8'), ('ResourceId', '<i8'), ('ObjId', '<i8'),
                ('TimeCreated', '<i8'), ('NucId', '<i8'), ('Units', 'O'), ('Mass', '<f8')]))
        )
    res = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 9, 7, 'Material', 1, 2, 'kg', 5, 0, 0),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('ResourceId', '<i8'), ('ObjId', '<i8'),
                ('Type', 'O'), ('TimeCreated', '<i8'), ('Quantity', '<i8'),
                ('Units', 'O'), ('QualId', '<i8'), ('Parent1', '<i8'),
                ('Parent2', '<i8')]))
        )
    comps = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 922350000, 0.02),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 922380000, 0.97),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 942390000, 0.005),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('QualId', '<i8'), ('NucId', '<i8'),
                ('MassFrac', '<f8')]))
        )
    obs = metrics.materials.func(res, comps)
    assert_frame_equal(exp, obs)


def test_activity():
    if not HAVE_PYNE:
        raise SkipTest
    exp = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 9, 7, 1, 922350000, 3197501.3876324706),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 9, 7, 1, 922380000, 24126337.066086654),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 9, 7, 1, 942390000, 22949993169.28023),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('QualId', '<i8'), ('ResourceId', '<i8'), ('ObjId', '<i8'),
                ('TimeCreated', '<i8'), ('NucId', '<i8'), ('Activity', '<f8')]))
        )

    mass = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 9, 7, 1, 922350000, 0.04),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 9, 7, 1, 922380000, 1.94),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 9, 7, 1, 942390000, 0.01),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('QualId', '<i8'), ('ResourceId', '<i8'), ('ObjId', '<i8'),
                ('TimeCreated', '<i8'), ('NucId', '<i8'), ('Mass', '<f8')]))
        )
    obs = metrics.activity.func(mass)
    assert_frame_equal(exp, obs)


def test_decayheat():
    if not HAVE_PYNE:
        raise SkipTest
    exp = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 9, 7, 1, 922350000, 9.3280119931e+25),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 9, 7, 1, 922380000, 6.42996948889e+26),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 9, 7, 1, 942390000, 7.51020971553e+29),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('QualId', '<i8'), ('ResourceId', '<i8'), ('ObjId', '<i8'),
                ('TimeCreated', '<i8'), ('NucId', '<i8'), ('DecayHeat', '<f8')]))
        )

    act = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 9, 7, 1, 922350000, 3197501.3876324706),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 9, 7, 1, 922380000, 24126337.066086654),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 9, 7, 1, 942390000, 22949993169.28023),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('QualId', '<i8'), ('ResourceId', '<i8'), ('ObjId', '<i8'),
                ('TimeCreated', '<i8'), ('NucId', '<i8'), ('Activity', '<f8')]))
        )

    obs = metrics.decay_heat.func(act)
    assert_frame_equal(exp, obs)


def test_transaction_quantity():
    exp = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 7, 3, 3,  10, 20, 'LWR Fuel', 'kg', 410),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 8, 4, 3,  20, 30, 'FR Fuel', 'kg', 305),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 3, 9, 5, 12, 30, 40, 'Spent Fuel', 'kg', 9),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('TransactionId', '<i8'), ('ResourceId', '<i8'),
                ('ObjId', '<i8'), ('TimeCreated', '<i8'), ('SenderId', '<i8'),
                ('ReceiverId', '<i8'), ('Commodity', 'O'), ('Units', 'O'),
                ('Quantity', '<f8')]))
        )
    mats = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 7, 3, 3, 922350000, 'kg', 10),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 5, 7, 3, 3, 922380000, 'kg', 400),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 6, 8, 4, 3, 942390000, 'kg', 5),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 6, 8, 4, 3, 922380000, 'kg', 300),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 7, 9, 5, 12, 942390000, 'kg', 5),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 7, 9, 5, 12, 922360000, 'kg', 4),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('QualId', '<i8'), ('ResourceId', '<i8'),
                ('ObjId', '<i8'), ('TimeCreated', '<i8'), ('NucId', '<i8'),
                ('Units', 'O'), ('Mass', '<f8')]))
        )
    trans = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 10, 20, 7, 'LWR Fuel'),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 20, 30, 8, 'FR Fuel'),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 3, 30, 40, 9, 'Spent Fuel'),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('TransactionId', '<i8'), ('SenderId', '<i8'),
                ('ReceiverId', '<i8'), ('ResourceId', '<i8'), ('Commodity', 'O')]))
        )
    obs = metrics.transaction_quantity.func(mats, trans)
    assert_frame_equal(exp, obs)


def test_explicit_inventory_by_agent():
    exp = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 1, 'core', 922350000, 1.0),
    	(UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 2, 'core', 922350000, 4.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 1, 'inventory', 922350000, 1.0),
	    (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 1, 'inventory', 922380000, 2.0),
	    (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 2, 'core', 922350000, 2.0)
	], dtype=ensure_dt_bytes([
	        ('SimId', 'O'), ('AgentId', '<i8'), ('Time', '<i8'),
            ('InventoryName', 'O'), ('NucId', '<i8'), ('Quantity', '<f8')]))
        )
    inv = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 1, 'core', 922350000, 1.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 2, 'core', 922350000, 2.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 2, 'core', 922350000, 2.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 1, 'inventory', 922350000, 1.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 1, 'inventory', 922380000, 2.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 2, 'core', 922350000, 2.0),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('AgentId', '<i8'), ('Time', '<i8'),
                ('InventoryName', 'O'), ('NucId', '<i8'), ('Quantity', '<f8')]))
        )
    obs = metrics.explicit_inventory_by_agent.func(inv)
    assert_frame_equal(exp, obs)


def test_explicit_inventory_by_nuc():
    exp = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 'core', 922350000, 1.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 'inventory', 922350000, 1.0),
	    (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 'inventory', 922380000, 2.0),
    	(UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 'core', 922350000, 4.0),
	    (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 'core', 922380000, 2.0)
	], dtype=ensure_dt_bytes([
	        ('SimId', 'O'), ('Time', '<i8'), ('InventoryName', 'O'),
    		('NucId', '<i8'), ('Quantity', '<f8')]))
        )
    inv = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 1, 'core', 922350000, 1.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 2, 'core', 922350000, 2.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 2, 'core', 922380000, 2.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 1, 'inventory', 922350000, 1.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 1, 'inventory', 922380000, 2.0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 2, 'core', 922350000, 2.0),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('AgentId', '<i8'), ('Time', '<i8'),
                ('InventoryName', 'O'), ('NucId', '<i8'), ('Quantity', '<f8')]))
        )
    obs = metrics.explicit_inventory_by_nuc.func(inv)
    assert_frame_equal(exp, obs)


def test_annual_electricity_generated_by_agent():
    exp = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 0, 100),
	    (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 1, 100),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 0, 200),
	    (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 1, 200),
	    (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 3, 1, 400)
	], dtype=ensure_dt_bytes([
	        ('SimId', 'O'), ('AgentId', '<i8'), ('Year', '<i8'),
    		('Energy', '<f8')]))
        )
    tsp = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 3, 1200),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 3, 2400),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 12, 1200),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 12, 2400),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 3, 12, 4800),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('AgentId', '<i8'), ('Time', '<i8'),
                ('Value', '<f8')]))
        )
    obs = metrics.annual_electricity_generated_by_agent.func(tsp)
    assert_frame_equal(exp, obs)


def test_monthly_electricity_generated_by_agent():
    exp = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 0, 100),
	    (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 1, 100),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 0, 200),
	    (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 1, 200),
	    (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 3, 1, 400)
	], dtype=ensure_dt_bytes([
	        ('SimId', 'O'), ('AgentId', '<i8'), ('Month', '<i8'),
    		('Energy', '<f8')]))
        )
    tsp = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 0, 100),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 0, 200),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1, 1, 100),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2, 1, 200),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 3, 1, 400),
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('AgentId', '<i8'), ('Time', '<i8'),
                ('Value', '<f8')]))
        )
    obs = metrics.monthly_electricity_generated_by_agent.func(tsp)
    assert_frame_equal(exp, obs)


def test_timelist():
    exp = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 0),
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 1),
        (UUID('f2952c34-0a0e-47df-b495-6f9afc351d1b'), 0),
        (UUID('f2952c34-0a0e-47df-b495-6f9afc351d1b'), 1)
        ], dtype=ensure_dt_bytes([('SimId', 'O'), ('TimeStep', '<i8')]))
        )
    info = pd.DataFrame(np.array([
        (UUID('f22f2281-2464-420a-8325-37320fd418f8'), 2),
        (UUID('f2952c34-0a0e-47df-b495-6f9afc351d1b'), 2)
        ], dtype=ensure_dt_bytes([
                ('SimId', 'O'), ('Duration', '<i8')]))
        )
    obs = metrics.timelist.func(info)
    assert_frame_equal(exp, obs)


if __name__ == "__main__":
    nose.runmodule()

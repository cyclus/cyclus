"""A collection of metrics that come stock with cymetric.
"""
from __future__ import print_function, unicode_literals
import inspect

import numpy as np
import pandas as pd

try:
    from pyne import data
    import pyne.enrichment as enr
    HAVE_PYNE = True
except ImportError:
    HAVE_PYNE = False

from cyclus import lib
from cyclus import typesystem as ts

try:
    from cymetric import schemas
    from cymetric import tools
    from cymetric.evaluator import register_metric
except ImportError:
    # some wacky CI paths prevent absolute importing, try relative
    from . import schemas
    from . import tools
    from .evaluator import register_metric


class Metric(object):
    """Metric class"""
    dependencies = NotImplemented
    schema = NotImplemented

    def __init__(self, db):
        self.db = db

    @property
    def name(self):
        return self.__class__.__name__


def _genmetricclass(f, name, depends, scheme):
    """Creates a new metric class with a given name, dependencies, and schema.

    Parameters
    ----------
    name : str
        Metric name
    depends : list of lists (table name, tuple of indices, column name)
        Dependencies on other database tables (metrics or root metrics)
    scheme : list of tuples (column name, data type)
        Schema for metric
    """
    if not isinstance(scheme, schemas.schema):
        scheme = schemas.schema(scheme)

    class Cls(Metric):
        dependencies = depends
        schema = scheme
        func = staticmethod(f)

        __doc__ = inspect.getdoc(f)

        def __init__(self, db):
            """Constructor for metric object in database."""
            super(Cls, self).__init__(db)

        def __call__(self, frames, conds=None, known_tables=None, *args, **kwargs):
            """Computes metric for given input data and conditions."""
            # FIXME test if I already exist in the db, read in if I do
            if known_tables is None:
                known_tables = self.db.tables()
            if self.name in known_tables:
                return self.db.query(self.name, conds=conds)
            return f(*frames)

    Cls.__name__ = str(name)
    register_metric(Cls)
    return Cls


def metric(name=None, depends=NotImplemented, schema=NotImplemented):
    """Decorator that creates metric class from a function or class."""
    def dec(f):
        clsname = name or f.__name__
        return _genmetricclass(f=f, name=clsname, scheme=schema, depends=depends)
    return dec


#####################
## General Metrics ##
#####################

# Material Mass (quantity * massfrac)
_matdeps = ['Resources', 'Compositions']

_matschema = [
    ('SimId', ts.UUID),
    ('QualId', ts.INT),
    ('ResourceId', ts.INT),
    ('ObjId', ts.INT),
    ('TimeCreated', ts.INT),
    ('NucId', ts.INT),
    ('Units', ts.STRING),
    ('Mass', ts.DOUBLE)
    ]

@metric(name='Materials', depends=_matdeps, schema=_matschema)
def materials(rsrcs, comps):
    """Materials metric returns the material mass (quantity of material in
    Resources times the massfrac in Compositions) indexed by the SimId, QualId,
    ResourceId, ObjId, TimeCreated, and NucId.
    """
    x = pd.merge(rsrcs, comps, on=['SimId', 'QualId'], how='inner')
    x = x.set_index(['SimId', 'QualId', 'ResourceId', 'ObjId','TimeCreated',
                     'NucId', 'Units'])
    y = x['Quantity'] * x['MassFrac']
    y.name = 'Mass'
    z = y.reset_index()
    return z


del _matdeps, _matschema


# Activity (mass * decay_const / atomic_mass)
_actdeps = ['Materials']

_actschema = [
    ('SimId', ts.UUID),
    ('QualId', ts.INT),
    ('ResourceId', ts.INT),
    ('ObjId', ts.INT),
    ('TimeCreated', ts.INT),
    ('NucId', ts.INT),
    ('Activity', ts.DOUBLE)
    ]

@metric(name='Activity', depends=_actdeps, schema=_actschema)
def activity(mats):
    """Activity metric returns the instantaneous activity of a nuclide
    in a material (material mass * decay constant / atomic mass)
    indexed by the SimId, QualId, ResourceId, ObjId, TimeCreated, and NucId.
    """
    tools.raise_no_pyne('Activity could not be computed', HAVE_PYNE)
    mass = tools.raw_to_series(mats,
                               ('SimId', 'QualId', 'ResourceId', 'ObjId', 'TimeCreated', 'NucId'),
                               'Mass')
    act = []
    for (simid, qual, res, obj, time, nuc), m in mass.iteritems():
        val = (1000 * data.N_A * m * data.decay_const(nuc) \
              / data.atomic_mass(nuc))
        act.append(val)
    act = pd.Series(act, index=mass.index)
    act.name = 'Activity'
    rtn = act.reset_index()
    return rtn

del _actdeps, _actschema


# DecayHeat (activity * q_value)
_dhdeps = ['Activity']

_dhschema = [
    ('SimId', ts.UUID),
    ('QualId', ts.INT),
    ('ResourceId', ts.INT),
    ('ObjId', ts.INT),
    ('TimeCreated', ts.INT),
    ('NucId', ts.INT),
    ('DecayHeat', ts.DOUBLE)
    ]

@metric(name='DecayHeat', depends=_dhdeps, schema=_dhschema)
def decay_heat(acts):
    """Decay heat metric returns the instantaneous decay heat of a nuclide
    in a material (Q value * activity) indexed by the SimId, QualId,
    ResourceId, ObjId, TimeCreated, and NucId.
    """
    tools.raise_no_pyne('DecayHeat could not be computed', HAVE_PYNE)
    act = tools.raw_to_series(acts,
                              ('SimId', 'QualId', 'ResourceId', 'ObjId', 'TimeCreated', 'NucId'),
                              'Activity')
    dh = []
    for (simid, qual, res, obj, time, nuc), a in act.iteritems():
        val = (data.MeV_per_MJ * a * data.q_val(nuc))
        dh.append(val)
    dh = pd.Series(dh, index=act.index)
    dh.name = 'DecayHeat'
    rtn = dh.reset_index()
    return rtn

del _dhdeps, _dhschema


# Agent Building
_bsdeps = ['AgentEntry']

_bsschema = [
    ('SimId', ts.UUID), ('EnterTime', ts.INT), ('Prototype', ts.STRING),
    ('Count', ts.INT)
    ]

@metric(name='BuildSeries', depends=_bsdeps, schema=_bsschema)
def build_series(entry):
    """Provides a time series of the building of agents by prototype.
    """
    entry_index = ['SimId', 'EnterTime', 'Prototype']
    count = entry.groupby(entry_index).size()
    count.name = 'Count'
    rtn = count.reset_index()
    return rtn


del _bsdeps, _bsschema


# Agent Decommissioning
_dsdeps = ['AgentEntry', 'AgentExit']

_dsschema = [
    ('SimId', ts.UUID),
    ('ExitTime', ts.INT),
    ('Prototype', ts.STRING),
    ('Count', ts.INT)
    ]

@metric(name='DecommissionSeries', depends=_dsdeps, schema=_dsschema)
def decommission_series(entry, exit):
    """Provides a time series of the decommissioning of agents by prototype.
    """
    exit_index = ['SimId', 'ExitTime', 'Prototype']
    if exit is not None:
        exit = pd.merge(entry, exit, on=['SimId', 'AgentId'], how='inner')
        exit = exit.set_index(exit_index)
    else:
        return print('No agents were decommissioned during this simulation.')
    count = exit.groupby(level=exit_index).size()
    count.name = 'Count'
    rtn = count.reset_index()
    return rtn

del _dsdeps, _dsschema


# Agents
_agentsdeps = ['AgentEntry', 'AgentExit', 'DecomSchedule', 'Info']

_agentsschema = schemas.schema([
    ('SimId', ts.UUID),
    ('AgentId', ts.INT),
    ('Kind', ts.STRING),
    ('Spec', ts.STRING),
    ('Prototype', ts.STRING),
    ('ParentId', ts.INT),
    ('Lifetime', ts.INT),
    ('EnterTime', ts.INT),
    ('ExitTime', ts.INT),
    ])

@metric(name='Agents', depends=_agentsdeps, schema=_agentsschema)
def agents(entry, exit, decom, info):
    """Computes the Agents table. This is tricky because both the AgentExit
    table and the DecomSchedule table may not be present in the database.
    Furthermore, the Info table does not contain the AgentId column. This
    computation handles the calculation of the ExitTime in the face a
    significant amounts of missing data.
    """
    mergeon = ['SimId', 'AgentId']
    ent = tools.raw_to_series(entry, ['SimId', 'AgentId'], 'Kind')
    idx = ent.index
    df = entry[['SimId', 'AgentId', 'Kind', 'Spec', 'Prototype', 'ParentId',
                'Lifetime', 'EnterTime']]
    if exit is None:
        agent_exit = pd.Series(index=idx, data=[np.nan]*len(idx))
        agent_exit.name = 'ExitTime'
    else:
        agent_exit = agent_exit.reindex(index=idx)
    df = pd.merge(df, agent_exit.reset_index(), on=mergeon)
    if decom is not None:
        df = tools.merge_and_fillna_col(df, decom[['SimId', 'AgentId', 'DecomTime']],
                                        'ExitTime', 'DecomTime', on=mergeon)
    df = tools.merge_and_fillna_col(df, info[['SimId', 'Duration']],
                                    'ExitTime', 'Duration', on=['SimId'])
    return df

del _agentsdeps, _agentsschema


# Transaction Quantity
_transdeps = ['Materials', 'Transactions']

_transschema = [
    ('SimId', ts.UUID),
    ('TransactionId', ts.INT),
    ('ResourceId', ts.INT),
    ('ObjId', ts.INT),
    ('TimeCreated', ts.INT),
    ('SenderId', ts.INT),
    ('ReceiverId', ts.INT),
    ('Commodity', ts.STRING),
    ('Units', ts.STRING),
    ('Quantity', ts.DOUBLE)
    ]

@metric(name='TransactionQuantity', depends=_transdeps, schema=_transschema)
def transaction_quantity(mats, tranacts):
    """Transaction Quantity metric returns the quantity of each transaction throughout
    the simulation.
    """
    trans_index = ['SimId', 'TransactionId', 'ResourceId', 'ObjId',
            'TimeCreated', 'SenderId', 'ReceiverId', 'Commodity', 'Units']
    trans = pd.merge(mats, tranacts, on=['SimId', 'ResourceId'], how='inner')
    trans = trans.set_index(trans_index)
    trans = trans.groupby(level=trans_index)['Mass'].sum()
    trans.name = 'Quantity'
    rtn = trans.reset_index()
    return rtn

del _transdeps, _transschema


# Explicit Inventory By Agent
_invdeps = ['ExplicitInventory']

_invschema = [
    ('SimId', ts.UUID),
    ('AgentId', ts.INT),
    ('Time', ts.INT),
    ('InventoryName', ts.STRING),
    ('NucId', ts.INT),
    ('Quantity', ts.DOUBLE)
    ]

@metric(name='ExplicitInventoryByAgent', depends=_invdeps, schema=_invschema)
def explicit_inventory_by_agent(expinv):
    """The Inventory By Agent metric groups the inventories by Agent
    (keeping all nuc information)
    """
    inv_index = ['SimId', 'AgentId', 'Time', 'InventoryName', 'NucId']
    inv = tools.raw_to_series(expinv,
                              ['SimId', 'AgentId', 'Time', 'InventoryName', 'NucId'],
                              'Quantity')
    inv = inv.groupby(level=inv_index).sum()
    inv.name = 'Quantity'
    rtn = inv.reset_index()
    return rtn

del _invdeps, _invschema


# Explicit Inventory By Nuc
_invdeps = ['ExplicitInventory']

_invschema = [
    ('SimId', ts.UUID),
    ('Time', ts.INT),
    ('InventoryName', ts.STRING),
    ('NucId', ts.INT),
    ('Quantity', ts.DOUBLE)
    ]

@metric(name='ExplicitInventoryByNuc', depends=_invdeps, schema=_invschema)
def explicit_inventory_by_nuc(expinv):
    """The Inventory By Nuc metric groups the inventories by nuclide
    and discards the agent information it is attached to (providing fuel
    cycle-wide nuclide inventories)
    """
    inv_index = ['SimId', 'Time', 'InventoryName', 'NucId']
    inv = tools.raw_to_series(expinv,
                              ('SimId', 'Time', 'InventoryName', 'NucId'),
                              'Quantity')
    inv = inv.groupby(level=inv_index).sum()
    inv.name = 'Quantity'
    rtn = inv.reset_index()
    return rtn

del _invdeps, _invschema


# Electricity Generated [MWe-y]
_egdeps = ['TimeSeriesPower']

_egschema = [
    ('SimId', ts.UUID),
    ('AgentId', ts.INT),
    ('Year', ts.INT),
    ('Energy', ts.DOUBLE)
    ]

@metric(name='AnnualElectricityGeneratedByAgent', depends=_egdeps, schema=_egschema)
def annual_electricity_generated_by_agent(elec):
    """Annual Electricity Generated metric returns the total electricity
    generated in MWe-y for each agent, calculated from the average monthly
    power given in TimeSeriesPower.
    """
    elec = pd.DataFrame(data={'SimId': elec.SimId,
                              'AgentId': elec.AgentId,
                              'Year': elec.Time.apply(lambda x: x//12),
                              'Energy': elec.Value.apply(lambda x: x/12)},
			columns=['SimId', 'AgentId', 'Year', 'Energy'])
    el_index = ['SimId', 'AgentId', 'Year']
    elec = elec.groupby(el_index).sum()
    rtn = elec.reset_index()
    return rtn

del _egdeps, _egschema


# Electricity Generated [MWe-month]
_egdeps = ['TimeSeriesPower']

_egschema = [
    ('SimId', ts.UUID),
    ('AgentId', ts.INT),
    ('Month', ts.INT),
    ('Energy', ts.DOUBLE)
    ]
@metric(name='MonthlyElectricityGeneratedByAgent', depends=_egdeps, schema=_egschema)
def monthly_electricity_generated_by_agent(elec):
    """Monthly Electricity Generated metric returns the total electricity
    generated in MWe-month for each agent, calculated from the average monthly
    power given in TimeSeriesPower.
    """
    elec = pd.DataFrame(data={'SimId': elec.SimId,
                              'AgentId': elec.AgentId,
                              'Month': elec.Time,
                              'Energy': elec.Value},
			columns=['SimId', 'AgentId', 'Month', 'Energy'])
    el_index = ['SimId', 'AgentId', 'Month']
    elec = elec.groupby(el_index).sum()
    rtn = elec.reset_index()
    return rtn

#
# Not a metric, not a root metric metrics
#

# These are not metrics that have any end use in mind, but are required for the
# calculation of other metrics. Required tables like this should be stored
# elsewhere in the future if they become more common.

# TimeList

_tldeps = ['Info']

_tlschema = [
    ('SimId', ts.UUID),
    ('TimeStep', ts.INT)
    ]

@metric(name='TimeList', depends=_tldeps, schema=_tlschema)
def timelist(info):
    """In case the sim does not have entries for every timestep, this populates
    a list with all timesteps in the duration.
    """
    info = tools.raw_to_series(info, ('SimId',), 'Duration')
    tl = []
    for sim, dur in info.iteritems():
        for i in range(dur):
            tl.append((sim, i))
    tl = pd.DataFrame(tl, columns=['SimId', 'TimeStep'])
    return tl


del _tldeps, _tlschema


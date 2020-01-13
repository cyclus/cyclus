"""FCO metrics"""
import pandas as pd
try:
    from pyne import data
    import pyne.enrichment as enr
    HAVE_PYNE = True
except ImportError:
    HAVE_PYNE = False
from cyclus import typesystem as ts

from cymetric import metric
from cymetric import tools

#########################
## FCO-related metrics ##
#########################


# Electricity Generated [GWe-y]
_egdeps = ['AnnualElectricityGeneratedByAgent']

_egschema = [('Year', ts.INT), ('Energy', ts.DOUBLE)]

@metric(name='FcoElectricityGenerated', depends=_egdeps, schema=_egschema)
def fco_electricity_generated(elec):
    """FcoElectricityGenerated metric returns the electricity generated in GWe-y
    for all agents in simulation.
    """
    elec = pd.DataFrame(data={'Year': elec.Year,
                              'Energy': elec.Energy.apply(lambda x: x/1000)},
                        columns=['Year', 'Energy'])
    elec = elec.groupby('Year').sum()
    rtn = elec.reset_index()
    return rtn

del _egdeps, _egschema


# Electricity Generated [GWe-month]
_egdeps = ['MonthlyElectricityGeneratedByAgent']

_egschema = [('Month', ts.INT), ('Energy', ts.DOUBLE)]

@metric(name='FcoMonthlyElectricityGenerated', depends=_egdeps, schema=_egschema)
def fco_monthly_electricity_generated(elec):
    """FcoElectricityGenerated metric returns the electricity generated in GWe-month
    for all agents in simulation.
    """
    elec = pd.DataFrame(data={'Month': elec.Month,
                              'Energy': elec.Energy.apply(lambda x: x/1000)},
                        columns=['Month', 'Energy'])
    elec = elec.groupby('Month').sum()
    rtn = elec.reset_index()
    return rtn

del _egdeps, _egschema


# U Resources Mined [t]
_udeps= ['Materials', 'Transactions']

_uschema = [('Year', ts.INT), ('UMined', ts.DOUBLE)]

@metric(name='FcoUMined', depends=_udeps, schema=_uschema)
def fco_u_mined(mats, trans):
    """FcoUMined metric returns the uranium mined in tonnes for each year
    in a simulation. This is written for simulations that use the
    Bright-lite Fuel Fab (i.e., the U235 and U238 are given separately in the
    FCO simulations, and the filtering is archetype-specific).
    """
    tools.raise_no_pyne('U_Mined could not be computed', HAVE_PYNE)
    mass = pd.merge(mats, trans, on=['ResourceId'], how='inner')
    mass = mass.set_index(['ObjId', 'TimeCreated', 'NucId'])
    u = []
    prods = {}
    mass235 = {}
    m = mass[mass['Commodity'] == 'LWR Fuel']
    for (obj, _, nuc), value in m.iterrows():
        if 922320000 <= nuc <= 922390000:
            prods[obj] = prods.get(obj, 0.0) + value['Mass']
        if nuc==922350000:
            mass235[obj] = value['Mass']
    x_feed = 0.0072
    x_tails = 0.0025
    for obj, m235 in mass235.items():
        x_prod = m235 / prods[obj]
        feed = enr.feed(x_feed, x_prod, x_tails, product=prods[obj]) / 1000
        u.append(feed)
    m = m.groupby(level=['ObjId', 'TimeCreated'])['Mass'].sum()
    m = m.reset_index()
    # sum by years (12 time steps)
    u = pd.DataFrame(data={'Year': m.TimeCreated.apply(lambda x: x//12),
                           'UMined': u}, columns=['Year', 'UMined'])
    u = u.groupby('Year').sum()
    rtn = u.reset_index()
    return rtn

del _udeps, _uschema


# SWU Required [million SWU]
_swudeps = ['Materials', 'Transactions']

_swuschema = [('Year', ts.INT), ('SWU', ts.DOUBLE)]

@metric(name='FcoSwu', depends=_swudeps, schema=_swuschema)
def fco_swu(mats, trans):
    """FcoSwu metric returns the separative work units required for each
    year in a simulation. This is written for simulations that
    use the Bright-lite Fuel Fab (i.e., the U235 and U238 are given separately
    in the FCO simulations, and the filtering is archetype-specific).
    """
    tools.raise_no_pyne('SWU Required could not be computed', HAVE_PYNE)
    mass = pd.merge(mats, trans, on=['ResourceId'], how='inner')
    mass = mass.set_index(['ObjId', 'TimeCreated', 'NucId'])
    swu = []
    prods = {}
    mass235 = {}
    m = mass[mass['Commodity'] == 'LWR Fuel']
    for (obj, _, nuc), value in m.iterrows():
        if 922320000 <= nuc <= 922390000:
            prods[obj] = prods.get(obj, 0.0) + value['Mass']
        if nuc == 922350000:
            mass235[obj] = value['Mass']
    x_feed = 0.0072
    x_tails = 0.0025
    for obj, m235 in mass235.items():
        x_prod = m235 / prods[obj]
        swu0 = enr.swu(x_feed, x_prod, x_tails, product=prods[obj]) / 1e6
        swu.append(swu0)
    m = m.groupby(level=['ObjId', 'TimeCreated'])['Mass'].sum()
    m = m.reset_index()
    # sum by years (12 time steps)
    swu = pd.DataFrame(data={'Year': m.TimeCreated.apply(lambda x: x//12),
                             'SWU': swu}, columns=['Year', 'SWU'])
    swu = swu.groupby('Year').sum()
    rtn = swu.reset_index()
    return rtn

del _swudeps, _swuschema


# Annual Fuel Loading Rate [tHM/y]
_fldeps = ['Materials', 'Transactions']

_flschema = [('Year', ts.INT), ('FuelLoading', ts.DOUBLE)]

@metric(name='FcoFuelLoading', depends=_fldeps, schema=_flschema)
def fco_fuel_loading(mats, trans):
    """FcoFuelLoading metric returns the fuel loaded in tHM/y in a
    simulation. This is written for FCO databases that use Bright-lite
    archetypes (the commodity filtering is specific to these archetypes).
    """
    mass = pd.merge(mats, trans, on=['ResourceId'], how='inner')
    mass = mass.set_index(['TimeCreated'])
    mass = mass.query('Commodity == ["LWR Fuel", "FR Fuel"]')
    mass = mass.groupby(mass.index)['Mass'].sum()
    mass = mass.reset_index()
    # sum by years (12 time steps)
    mass = pd.DataFrame(data={'Year': mass.TimeCreated.apply(lambda x: x//12),
                              'FuelLoading': mass.Mass.apply(lambda x: x/1000)},
                        columns=['Year', 'FuelLoading'])
    mass = mass.groupby('Year').sum()
    rtn = mass.reset_index()
    return rtn

del _fldeps, _flschema



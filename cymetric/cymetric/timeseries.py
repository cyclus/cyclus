import warnings

import pandas as pd
import numpy as np

try:
    from pyne import data
    import pyne.enrichment as enr
    from pyne import nucname
    HAVE_PYNE = True
except ImportError:
    HAVE_PYNE = False


from cymetric import tools
from cymetric import filters


def transactions(evaler, senders=(), receivers=(), commodities=(), nucs=()):
    """
    Shape the reduced transation Data Frame into a simple time serie. Applying nuclides selection when required.

    Parameters
    ----------
    evaler : evaler
    senders :  of the sending facility
    receivers :  of the receiving facility
    commodities :  of the commodity exchanged
    nucs :  of nuclide to select.
    """

    if len(nucs) != 0:
        nucs = tools.format_nucs(nucs)

    df = filters.transactions_nuc(
        evaler, senders, receivers, commodities, nucs)

    df = df[['Time', 'Mass']].groupby(['Time']).sum()
    df.reset_index(inplace=True)

    time = evaler.eval('TimeList')
    df = tools.add_missing_time_step(df, time)
    return df


def transactions_activity(evaler, senders=(), receivers=(), commodities=(), nucs=()):
    """
    Shape the reduced transation Data Frame into a simple time serie. Applying nuclides selection when required.

    Parameters
    ----------
    evaler : evaler
    senders :  of the sending facility
    receivers :  of the receiving facility
    commodities :  of the commodity exchanged
    nucs :  of nuclide to select.
    """

    if len(nucs) != 0:
        nucs = tools.format_nucs(nucs)

    df = filters.transactions_activity(evaler, senders, receivers, commodities,
                                     nucs)

    df = df[['Time', 'Activity']].groupby(['Time']).sum()
    df.reset_index(inplace=True)

    time = evaler.eval('TimeList')
    df = tools.add_missing_time_step(df, time)
    return df


def transactions_decayheat(evaler, senders=(), receivers=(), commodities=(), nucs=()):
    """
    Shape the reduced transation Data Frame into a simple time serie. Applying nuclides selection when required.

    Parameters
    ----------
    evaler : evaler
    senders :  of the sending facility
    receivers :  of the receiving facility
    commodities :  of the commodity exchanged
    nucs :  of nuclide to select.
    """

    if len(nucs) != 0:
        nucs = tools.format_nucs(nucs)

    df = filters.transactions_decayheat(evaler, senders, receivers, commodities,
                                      nucs)

    df = df[['Time', 'DecayHeat']].groupby(['Time']).sum()
    df.reset_index(inplace=True)

    time = evaler.eval('TimeList')
    df = tools.add_missing_time_step(df, time)
    return df




def inventories(evaler, facilities=(), nucs=()):
    """
    Shape the reduced inventory Data Frame into a simple time serie. Applying
    nuclides/facilities selection when required.

    Parameters
    ----------
    evaler : evaler
    facilities :  of the facility
    nucs :  of nuclide to select.
    """

    if len(nucs) != 0:
        nucs = tools.format_nucs(nucs)
    else:
        wng_msg = "no nuclide provided"
        warnings.warn(wng_msg, UserWarning)

    df = filters.inventories(evaler, facilities, nucs)

    df = df[['Time', 'Quantity']].groupby(['Time']).sum()
    df.reset_index(inplace=True)

    time = evaler.eval('TimeList')
    df = tools.add_missing_time_step(df, time)
    return df


def inventories_activity(evaler, facilities=(), nucs=()):
    """
    Get a simple time series of the decay heat of the inventory in the selcted
    facilities. Applying nuclides selection when required.

    Parameters
    ----------
    evaler : evaler
    facilities :  of the facility
    nucs :  of nuclide to select.
    """

    if len(nucs) != 0:
        nucs = tools.format_nucs(nucs)
    else:
        wng_msg = "no nuclide provided"
        warnings.warn(wng_msg, UserWarning)

    df = filters.inventories_activity(evaler, facilities, nucs)
    df = df[['Time', 'Activity']].groupby(['Time']).sum()
    df.reset_index(inplace=True)

    time = evaler.eval('TimeList')
    df = tools.add_missing_time_step(df, time)
    return df


def inventories_decayheat(evaler, facilities=(), nucs=()):
    """
    Get a simple time series of the decay heat of the inventory in the selcted
    facilities. Applying nuclides selection when required.

    Parameters
    ----------
    evaler : evaler
    facilities :  of the facility
    nucs :  of nuclide to select.
    """

    if len(nucs) != 0:
        nucs = tools.format_nucs(nucs)
    else:
        wng_msg = "no nuclide provided"
        warnings.warn(wng_msg, UserWarning)

    df = filters.inventories_decayheat(evaler, facilities, nucs)
    df = df[['Time', 'DecayHeat']].groupby(['Time']).sum()
    df.reset_index(inplace=True)

    time = evaler.eval('TimeList')
    df = tools.add_missing_time_step(df, time)
    return df


def get_power(evaler, facilities=()):
    """
    Shape the reduced Power Data Frame into a simple time serie. Applying
    facilities selection when required.

    Parameters
    ----------
    evaler : evaler
    facilities :  of the facility
    """

    # Get inventory table
    df = evaler.eval('TimeSeriesPower')
    agents = evaler.eval('AgentEntry')

    rdc_ = []  # because we want to get rid of the facility asap
    if len(facilities) != 0:
        agents = agents[agents['Prototype'].isin(facilities)]
        rdc_.append(['AgentId', agents['AgentId'].tolist()])
    else:
        wng_msg = "no faciity provided"
        warnings.warn(wng_msg, UserWarning)
    df = tools.reduce(df, rdc_)

    base_col = ['SimId', 'AgentId']
    added_col = base_col + ['Prototype']
    df = tools.merge(df, base_col, agents, added_col)

    df = df[['Time', 'Value']].groupby(['Time']).sum()
    df.reset_index(inplace=True)

    time = evaler.eval('TimeList')
    df = tools.add_missing_time_step(df, time)
    return df


def get_deployment(evaler, facilities=()):
    """
    Get a simple time series with deployment schedule of the selected facilities.

    Parameters
    ----------
    evaler : evaler
    facilities :  of the facility
    """

    # Get inventory table
    df = evaler.eval('AgentEntry')

    rdc_ = []  # because we want to get rid of the facility asap
    if len(facilities) != 0:
        df = df[df['Prototype'].isin(facilities)]
        rdc_.append(['AgentId', df['AgentId'].tolist()])
    else:
        wng_msg = "no faciity provided"
        warnings.warn(wng_msg, UserWarning)

    # Adding a constant column to easily sum the amount of facilities build per
    # time step
    df = df.assign(Value=lambda x: 1)
    df = df[['EnterTime', 'Value']].groupby(['Time']).sum()
    df.reset_index(inplace=True)
    df.rename(index=str, columns={'EnterTime': 'Time'}, inplace=True)

    time = evaler.eval('TimeList')
    df = tools.add_missing_time_step(df, time)
    return df


def get_retirement(evaler, facilities=()):
    """
    Get a simple time series with retirement schedule of the selected facilities.

    Parameters
    ----------
    evaler : evaler
    facilities :  of the facility
    """

    # Get inventory table
    df = evaler.eval('AgentEntry')
    df = df[df['Lifetime'] > 0]

    rdc_ = []  # because we want to get rid of the facility asap
    if len(facilities) != 0:
        df = df[df['Prototype'].isin(facilities)]
        rdc_.append(['AgentId', df['AgentId'].tolist()])
    else:
        wng_msg = "no faciity provided"
        warnings.warn(wng_msg, UserWarning)

    # Adding a constant column to easily sum the amount of facilities build per
    # time step i

    df = df.assign(Value=lambda x: 1)
    df = df[['DecomTime','Value']].groupby(['Time']).sum()
    df.reset_index(inplace=True)

    df.rename(index=str, columns={'DecomTime': 'Time'}, inplace=True)
    time = evaler.eval('TimeList')
    df = tools.add_missing_time_step(df, time)
    return df

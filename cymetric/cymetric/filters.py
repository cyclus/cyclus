import warnings
import pandas as pd
import numpy as np

try:
    from graphviz import Digraph
    HAVE_GRAPHVIZ = True
except ImportError:
    HAVE_GRAPHVIZ = False

try:
    from pyne import data
    import pyne.enrichment as enr
    from pyne import nucname
    HAVE_PYNE = True
except ImportError:
    HAVE_PYNE = False


from cymetric.tools import format_nucs, reduce, merge, add_missing_time_step


def transactions(evaler, senders=(), receivers=(), commodities=()):
    """
    Filter the Transaction Data Frame on specific sending facility and
    receving facility.

    Parameters
    ----------
    evaler : evaler
    senders :  of the sending facility
    receivers :  of the receiving facility
    commodities :  of the commodity exchanged
    """

    # initiate evaluation
    trans = evaler.eval('Transactions')
    agents = evaler.eval('AgentEntry')

    rec_agent = agents.rename(index=str, columns={'AgentId': 'ReceiverId'})
    if len(receivers) != 0:
        rec_agent = rec_agent[rec_agent['Prototype'].isin(receivers)]

    send_agent = agents.rename(index=str, columns={'AgentId': 'SenderId'})
    if len(senders) != 0:
        send_agent = send_agent[send_agent['Prototype'].isin(senders)]

    # Clean Transation PDF
    rdc_table = []
    rdc_table.append(['ReceiverId', rec_agent['ReceiverId'].tolist()])
    rdc_table.append(['SenderId', send_agent['SenderId'].tolist()])
    if len(commodities) != 0:
        rdc_table.append(['Commodity', commodities])

    trans = reduce(trans, rdc_table)

    # Merge Sender to Transaction PDF
    base_col = ['SimId', 'SenderId']
    added_col = base_col + ['Prototype']
    trans = merge(trans, base_col, send_agent, added_col)
    trans = trans.rename(index=str, columns={
                         'Prototype': 'SenderPrototype'})

    # Merge Receiver to Transaction PDF
    base_col = ['SimId', 'ReceiverId']
    added_col = base_col + ['Prototype']
    trans = merge(trans, base_col, rec_agent, added_col)
    trans = trans.rename(index=str, columns={
                         'Prototype': 'ReceiverPrototype'})

    return trans


def transactions_nuc(evaler, senders=(), receivers=(), commodities=(), nucs=()):
    """
    Filter the Transaction Data Frame, which include nuclide composition, on
    specific sending facility and receving facility. Applying nuclides
    selection when required.

    Parameters
    ----------
    evaler : evaler
    senders :  of the sending facility
    receivers :  of the receiving facility
    commodities :  of the commodity exchanged
    nucs :  of nuclide to select.
    """

    compo = evaler.eval('Materials')

    df = transactions(evaler, senders, receivers, commodities)

    if len(nucs) != 0:
        nucs = format_nucs(nucs)
        compo = reduce(compo, [['NucId', nucs]])

    base_col = ['SimId', 'ResourceId']
    added_col = base_col + ['NucId', 'Mass']
    df = merge(df, base_col, compo, added_col)

    return df


def transactions_activity(evaler, senders=(), receivers=(), commodities=(), nucs=()):
    """
    Return the transation df, with the activities. Applying nuclides selection
    when required.

    Parameters
    ----------
    evaler : evaler
    senders :  of the sending facility
    receivers :  of the receiving facility
    commodities :  of the commodity exchanged
    nucs :  of nuclide to select.
    """

    df = transactions(evaler, senders, receivers, commodities)

    if len(nucs) != 0:
        nucs = format_nucs(nucs)

    compo = evaler.eval('Activity')
    compo = reduce(compo, [['NucId', nucs]])

    base_col = ['SimId', 'ResourceId']
    added_col = base_col + ['NucId', 'Activity']
    df = merge(df, base_col, compo, added_col)

    return df


def transactions_decayheat(evaler, senders=(), receivers=(), commodities=(), nucs=()):
    """
    Return the transation df, with the decayheat. Applying nuclides selection
    when required.

    Parameters
    ----------
    evaler : evaler
    senders :  of the sending facility
    receivers :  of the receiving facility
    commodities :  of the commodity exchanged
    nucs :  of nuclide to select.
    """

    df = transactions(evaler, senders, receivers, commodities)

    if len(nucs) != 0:
        nucs = format_nucs(nucs)

    compo = evaler.eval('DecayHeat')
    compo = reduce(compo, [['NucId', nucs]])

    base_col = ['SimId', 'ResourceId']
    added_col = base_col + ['NucId', 'DecayHeat']
    df = merge(df, base_col, compo, added_col)

    return df


def inventories(evaler, facilities=(), nucs=()):
    """
    Shape the reduced inventory Data Frame. Applying nuclides/facilities selection when required.

    Parameters
    ----------
    evaler : evaler
    facilities :  of the facility
    nucs :  of nuclide to select.
    """

    # Get inventory table
    df = evaler.eval('ExplicitInventory')
    agents = evaler.eval('AgentEntry')

    rdc_table = []  # because we want to get rid of the nuclide asap
    if len(nucs) != 0:
        nucs = format_nucs(nucs)
        rdc_table.append(['NucId', nucs])

    if len(facilities) != 0:
        agents = agents[agents['Prototype'].isin(facilities)]
        rdc_table.append(['AgentId', agents['AgentId'].tolist()])
    else:
        wng_msg = "no faciity provided"
        warnings.warn(wng_msg, UserWarning)
    df = reduce(df, rdc_table)

    base_col = ['SimId', 'AgentId']
    added_col = base_col + ['Prototype']
    df = merge(df, base_col, agents, added_col)

    return df


def inventories_activity(evaler, facilities=(), nucs=()):
    """
    Get a simple time series of the activity of the inventory in the selcted
    facilities. Applying nuclides selection when required.

    Parameters
    ----------
    evaler : evaler
    facilities :  of the facility
    nucs :  of nuclide to select.
    """

    if len(nucs) != 0:
        nucs = format_nucs(nucs)

    df = inventories(evaler, facilities, nucs)
    for i, row in df.iterrows():
        val = 1000 * data.N_A * row['Quantity'] * \
            data.decay_const(int(row['NucId']))
        df.set_value(i, 'Activity', val)

    return df


def inventories_decayheat(evaler, facilities=(), nucs=()):
    """
    Get a Inventory PDF including the decay heat of the inventory in the selected
    facilities. Applying nuclides selection when required.

    Parameters
    ----------
    evaler : evaler
    facilities :  of the facility
    nucs :  of nuclide to select.
    """

    if len(nucs) != 0:
        nucs = format_nucs(nucs)

    df = inventories_activity(evaler, facilities, nucs)
    for i, row in df.iterrows():
        val = data.MeV_per_MJ * \
            row['Activity'] * data.q_val(int(row['NucId']))
        df.set_value(i, 'DecayHeat', val)

    return df

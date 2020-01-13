"""A plot generator for Cymetric.
"""
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

from cymetric import tools
from cymetric.filters import transactions_nuc


def flow_graph(evaler, senders=(), receivers=(), commodities=(), nucs=(),
        label='', start=None, stop=None):
    """
    Generate the dot graph of the transation between facilitiese. Applying times
    nuclides selection when required.

    Parameters
    ----------
    evaler : evaler
    senders : list of the sending facility to consider
    receivers : list of the receiving facility to consider
    commodities : list of the commodity exchanged to consider
    nucs : list of nuclide to consider
    label : label key, used to add label on the arrow connecting facilities (for
    commodity use 'com', for mass use 'mass', for both use com,mass)
    start : first timestep to consider, start included
    stop : last timestep to consider, stop included
    """
    tools.raise_no_graphviz('Unable to generate flow graph!', HAVE_GRAPHVIZ)

    df = transactions_nuc(
        evaler, senders, receivers, commodities, nucs)

    if start != None:
        df = df.loc[(df['Time'] >= start)]
    if stop != None:
        df = df.loc[(df['Time'] <= stop)]

    group_end = ['ReceiverPrototype', 'SenderPrototype', 'Commodity']
    group_start = group_end + ['Mass']
    df = df[group_start].groupby(group_end).sum()
    df.reset_index(inplace=True)

    agents_ = evaler.eval('AgentEntry')['Prototype'].tolist()

    dot = Digraph('G')

    for agent in agents_:
        dot.node(agent)

    for index, row in df.iterrows():
        lbl = ''
        if 'com' in label:
            lbl += str(row['Commodity']) + ' '
        if 'mass' in label:
            lbl += str('{:.2e}'.format(row['Mass'])) + ' '
        dot.edge(row['SenderPrototype'], row['ReceiverPrototype'],
                 label= lbl)

    return dot

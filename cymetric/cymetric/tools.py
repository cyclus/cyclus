"""General cymetric tools.
"""
from __future__ import unicode_literals, print_function
import os
import sys
import warnings

import numpy as np
import pandas as pd

try:
    from pyne import data
    import pyne.enrichment as enr
    from pyne import nucname
    HAVE_PYNE = True
except ImportError:
    HAVE_PYNE = False

from cyclus import lib


EXT_BACKENDS = {'.h5': lib.Hdf5Back, '.sqlite': lib.SqliteBack}


def dbopen(fname):
    """Opens a Cyclus database."""
    _, ext = os.path.splitext(fname)
    if ext not in EXT_BACKENDS:
        msg = ('The backend database type of {0!r} could not be determined from '
               'extension {1!r}.')
        raise ValueError(msg.format(fname, ext))
    db = EXT_BACKENDS[ext](fname)
    return db


def raw_to_series(df, idx, val):
    """Convert data frame to series with multi-index."""
    d = df.set_index(list(map(str, idx)))
    s = df[val]
    s.index = d.index
    return s


def merge_and_fillna_col(left, right, lcol, rcol, how='inner', on=None):
    """Merges two dataframes and fills the values of the left column
    with the values from the right column. A copy of left is returned.

    Parameters
    ----------
    left : pd.DataFrame
        The left data frame
    right : pd.DataFrame
        The right data frame
    lcol : str
        The left column name
    rcol : str
        The right column name
    how : str, optional
        How to perform merge, same as in pd.merge()
    on : list of str, optional
        Which columns to merge on, same as in pd.merge()
    """
    m = pd.merge(left, right, how=how, on=on)
    f = m[lcol].fillna(m[rcol])
    left[lcol] = f
    return left


def ensure_dt_bytes(dt):
    """Ensures that a structured numpy dtype is given in a Python 2 & 3
    compatible way.
    """
    if sys.version_info[0] > 2:
        return dt
    dety = []
    for t in dt:
        t0 = t[0].encode() if isinstance(t[0], unicode) else t[0]
        t1 = t[1].encode() if isinstance(t[1], unicode) else t[1]
        ty = (t0, t1)
        if len(t) == 3:
            ty = ty + t[2:]
        dety.append(ty)
    return dety


def format_nucs(nucs):
    """
    format the nuclide  provided by the users into a standard format:
    ZZAASSSS.

    Parameters
    ----------
    nucs :  of nuclides
    """
    raise_no_pyne('Unable to format nuclide !', HAVE_PYNE)

    return [nucname.id(nuc) for nuc in nucs]


def add_missing_time_step(df, ref_time):
    """
    Add the missing time step to a Panda Data Frame.

    Parameters
    ----------
    df : Pandas Data Frame
    ref_time :  of the time step references (Coming from TimeStep metrics)
    """
    ref_time.rename(index=str, columns={'TimeStep': 'Time'}, inplace=True)

    if 'SimId' in ref_time.columns.values:
        ref_time.drop('SimId', 1, inplace=True)
    df = pd.merge(ref_time, df, how="outer")
    df.fillna(0, inplace=True)
    return df


def merge(df, base_col, add_df, add_col):
    """
    Merge some additionnal columns fram an additionnal Pandas Data Frame
    onother one and then remove the second base column (keeping SimID
    information).

    Parameters
    ----------
    df: Pandas Data Frame
    base_col:  of the base columns names
    add_df: Pandas Data Frame to add in the df one
    add_col: columns to be added
    """
    df = pd.merge(add_df[add_col], df, on=base_col)
    df.drop(base_col[1], 1)
    return df


def reduce(df, rdc_):
    """
    Filter the df Pandas Data Frame according to the rdc_ (list of item
    in the corresponding columns).

    Parameters
    ----------
    df: Pandas Data Frame
    rdc_: list of pair of string and string list.
    """
    for rdc in rdc_:
        if len(rdc[1]) != 0:
            df = df[df[rdc[0]].isin(rdc[1])]
        else:
            wng_msg = "Empty  provided for " + rdc[0] + " key."
            warnings.warn(wng_msg, UserWarning)
    return df


def raise_no_pyne(msg, have_pyne=False):
    """Raise an error when PyNE cannot be found."""
    if not have_pyne:
        raise ImportError('pyne could not be imported: ' + msg)


def raise_no_graphviz(msg, have_graphviz=False):
    """Raise an error when Graphviz cannot be found."""
    if not have_graphviz:
        raise ImportError('Graphviz python lib could not be imported: ' + msg)

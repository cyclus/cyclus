"""Tests for filters method"""
from __future__ import print_function, unicode_literals
from uuid import UUID
import os
import subprocess
from functools import wraps

import nose
from nose.tools import assert_equal, assert_less

import numpy as np
import pandas as pd
from pandas.util.testing import assert_frame_equal
from nose.plugins.skip import SkipTest

from tools import setup, dbtest

import cymetric as cym
from cymetric import filters
from cymetric.tools import raw_to_series, ensure_dt_bytes

try:
    from pyne import data
    import pyne.enrichment as enr
    HAVE_PYNE = True
except ImportError:
    HAVE_PYNE = False


@dbtest
def test_transactions(db, fname, backend):
    evaler = cym.Evaluator(db)
    cal = filters.transactions(evaler)
    exp_head = ['SimId', 'ReceiverId', 'ReceiverPrototype', 'SenderId',
                'SenderPrototype', 'TransactionId', 'ResourceId', 'Commodity', 'Time']
    assert_equal(list(cal), exp_head)  # Check we have the correct headers

    # SimId et al. change at each test need to drop it
    drop_cols = ['SimId', 'TransactionId', 'ResourceId', 'ReceiverId', 'SenderId']
    cal = cal.drop(drop_cols, axis=1)
    refs = pd.DataFrame(np.array([
        ('Reactor1', 'UOX_Source', 'uox', 4),
        ('Reactor1', 'MOX_Source', 'mox', 1),
        ('Reactor1', 'MOX_Source', 'mox', 2),
        ('Reactor1', 'MOX_Source', 'mox', 3),
        ('Reactor2', 'MOX_Source', 'mox', 2),
        ('Reactor2', 'MOX_Source', 'mox', 3),
        ('Reactor2', 'MOX_Source', 'mox', 4),
        ('Reactor3', 'UOX_Source', 'uox', 3),
        ('Reactor3', 'MOX_Source', 'mox', 4),
    ], dtype=ensure_dt_bytes([
        ('ReceiverPrototype', 'O'), ('SenderPrototype', 'O'),
        ('Commodity', 'O'), ('Time', '<i8')
    ]))
    )
    refs.index = refs.index.astype('str')
    assert_frame_equal(cal, refs)

    # test single sender
    cal = filters.transactions(evaler, senders=['UOX_Source'])
    cal = cal.drop(drop_cols, axis=1)
    refs = pd.DataFrame(np.array([
        ('Reactor1', 'UOX_Source', 'uox', 4),
        ('Reactor3', 'UOX_Source', 'uox', 3),
    ], dtype=ensure_dt_bytes([
        ('ReceiverPrototype', 'O'), ('SenderPrototype', 'O'),
        ('Commodity', 'O'), ('Time', '<i8')
    ]))
    )
    refs.index = refs.index.astype('str')
    assert_frame_equal(cal, refs)

    # test multiple sender
    cal = filters.transactions(
        evaler, senders=['UOX_Source', 'MOX_Source'])
    cal = cal.drop(drop_cols, axis=1)
    refs = pd.DataFrame(np.array([
        ('Reactor1', 'UOX_Source', 'uox', 4),
        ('Reactor1', 'MOX_Source', 'mox', 1),
        ('Reactor1', 'MOX_Source', 'mox', 2),
        ('Reactor1', 'MOX_Source', 'mox', 3),
        ('Reactor2', 'MOX_Source', 'mox', 2),
        ('Reactor2', 'MOX_Source', 'mox', 3),
        ('Reactor2', 'MOX_Source', 'mox', 4),
        ('Reactor3', 'UOX_Source', 'uox', 3),
        ('Reactor3', 'MOX_Source', 'mox', 4),
    ], dtype=ensure_dt_bytes([
        ('ReceiverPrototype', 'O'), ('SenderPrototype', 'O'),
        ('Commodity', 'O'), ('Time', '<i8')
    ]))
    )
    refs.index = refs.index.astype('str')
    assert_frame_equal(cal, refs)

    # test single receiver
    cal = filters.transactions(evaler, receivers=['Reactor1'])
    cal = cal.drop(drop_cols, 1)  # SimId change at each test need to drop it
    refs = pd.DataFrame(np.array([
        ('Reactor1', 'UOX_Source', 'uox', 4),
        ('Reactor1', 'MOX_Source', 'mox', 1),
        ('Reactor1', 'MOX_Source', 'mox', 2),
        ('Reactor1', 'MOX_Source', 'mox', 3),
    ], dtype=ensure_dt_bytes([
        ('ReceiverPrototype', 'O'), ('SenderPrototype', 'O'),
        ('Commodity', 'O'), ('Time', '<i8')
    ]))
    )
    refs.index = refs.index.astype('str')
    assert_frame_equal(cal, refs)

    # test multiple sender
    cal = filters.transactions(
        evaler, receivers=['Reactor1', 'Reactor3'])
    cal = cal.drop(drop_cols, 1)  # SimId change at each test need to drop it
    refs = pd.DataFrame(np.array([
        ('Reactor1', 'UOX_Source', 'uox', 4),
        ('Reactor1', 'MOX_Source', 'mox', 1),
        ('Reactor1', 'MOX_Source', 'mox', 2),
        ('Reactor1', 'MOX_Source', 'mox', 3),
        ('Reactor3', 'UOX_Source', 'uox', 3),
        ('Reactor3', 'MOX_Source', 'mox', 4),
    ], dtype=ensure_dt_bytes([
        ('ReceiverPrototype', 'O'), ('SenderPrototype', 'O'),
        ('Commodity', 'O'), ('Time', '<i8')
    ]))
    )
    refs.index = refs.index.astype('str')
    assert_frame_equal(cal, refs)

    # test multiple sender and multiple receiver
    cal = filters.transactions(evaler, senders=['UOX_Source', 'MOX_Source'],
                                     receivers=['Reactor1', 'Reactor2'])
    cal = cal.drop(drop_cols, 1)  # SimId change at each test need to drop it
    refs = pd.DataFrame(np.array([
        ('Reactor1', 'UOX_Source', 'uox', 4),
        ('Reactor1', 'MOX_Source', 'mox', 1),
        ('Reactor1', 'MOX_Source', 'mox', 2),
        ('Reactor1', 'MOX_Source', 'mox', 3),
        ('Reactor2', 'MOX_Source', 'mox', 2),
        ('Reactor2', 'MOX_Source', 'mox', 3),
        ('Reactor2', 'MOX_Source', 'mox', 4),
    ], dtype=ensure_dt_bytes([
        ('ReceiverPrototype', 'O'), ('SenderPrototype', 'O'),
        ('Commodity', 'O'), ('Time', '<i8')
    ]))
    )
    refs.index = refs.index.astype('str')
    assert_frame_equal(cal, refs)

    # test single filters.odity
    cal = filters.transactions(evaler, commodities=['uox'])
    cal = cal.drop(drop_cols, 1)  # SimId change at each test need to drop it
    refs = pd.DataFrame(np.array([
        ('Reactor1', 'UOX_Source', 'uox', 4),
        ('Reactor3', 'UOX_Source', 'uox', 3),
    ], dtype=ensure_dt_bytes([
        ('ReceiverPrototype', 'O'), ('SenderPrototype', 'O'),
        ('Commodity', 'O'), ('Time', '<i8')
    ]))
    )
    refs.index = refs.index.astype('str')
    assert_frame_equal(cal, refs)

    # test multiple sender
    cal = filters.transactions(evaler, commodities=['uox', 'mox'])
    cal = cal.drop(drop_cols, 1)  # SimId change at each test need to drop it
    refs = pd.DataFrame(np.array([
        ('Reactor1', 'UOX_Source', 'uox', 4),
        ('Reactor1', 'MOX_Source', 'mox', 1),
        ('Reactor1', 'MOX_Source', 'mox', 2),
        ('Reactor1', 'MOX_Source', 'mox', 3),
        ('Reactor2', 'MOX_Source', 'mox', 2),
        ('Reactor2', 'MOX_Source', 'mox', 3),
        ('Reactor2', 'MOX_Source', 'mox', 4),
        ('Reactor3', 'UOX_Source', 'uox', 3),
        ('Reactor3', 'MOX_Source', 'mox', 4),
    ], dtype=ensure_dt_bytes([
        ('ReceiverPrototype', 'O'), ('SenderPrototype', 'O'),
        ('Commodity', 'O'), ('Time', '<i8')
    ]))
    )
    refs.index = refs.index.astype('str')
    assert_frame_equal(cal, refs)


@dbtest
def test_transactions_nuc(db, fname, backend):
    evaler = cym.Evaluator(db)
    cal = filters.transactions_nuc(evaler)
    exp_head = ['SimId', 'ResourceId', 'NucId', 'Mass', 'ReceiverId', 'ReceiverPrototype',
                'SenderId', 'SenderPrototype', 'TransactionId', 'Commodity', 'Time']
    assert_equal(list(cal), exp_head)  # Check we have the correct headers

    if not HAVE_PYNE:
        raise SkipTest
    # test single nuclide selection
    cal = filters.transactions_nuc(evaler, nucs=['942390000'])
    cal = cal.drop('SimId', 1)  # SimId change at each test need to drop it
    # SimId change at each test need to drop it
    cal = cal.drop('TransactionId', 1)
    # SimId change at each test need to drop it
    cal = cal.drop('ResourceId', 1)
    refs = pd.DataFrame(np.array([
        (942390000, 0.0444814879803, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 1),
        (942390000, 0.0444814879803, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 2),
        (942390000, 0.0444814879803, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 2),
        (942390000, 0.0444814879803, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 3),
        (942390000, 0.0444814879803, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 3),
        (942390000, 0.0444814879803, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 4),
        (942390000, 0.0444814879803, 17, 'Reactor3', 14, 'MOX_Source', 'mox', 4),
    ], dtype=ensure_dt_bytes([
        ('NucId', '<i8'), ('Mass', '<f8'), ('ReceiverId', '<i8'),
        ('ReceiverPrototype', 'O'), ('SenderId', '<i8'), ('SenderPrototype', 'O'),
        ('Commodity', 'O'), ('Time', '<i8')
    ]))
    )
    # refs.index = refs.index.astype('str')
    assert_frame_equal(cal, refs)

    # test multiple nuclide selection
    cal = filters.transactions_nuc(
        evaler, nucs=['942390000', '922380000'])
    cal = cal.drop('SimId', 1)  # SimId change at each test need to drop it
    # SimId change at each test need to drop it
    cal = cal.drop('TransactionId', 1)
    # SimId change at each test need to drop it
    cal = cal.drop('ResourceId', 1)
    refs = pd.DataFrame(np.array([
        (922380000, 0.7872433760310, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 1),
        (942390000, 0.0444814879803, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 1),
        (922380000, 0.7872433760310, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 2),
        (942390000, 0.0444814879803, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 2),
        (922380000, 0.7872433760310, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 2),
        (942390000, 0.0444814879803, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 2),
        (922380000, 0.7872433760310, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 3),
        (942390000, 0.0444814879803, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 3),
        (922380000, 0.7872433760310, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 3),
        (942390000, 0.0444814879803, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 3),
        (922380000, 0.7872433760310, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 4),
        (942390000, 0.0444814879803, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 4),
        (922380000, 0.7872433760310, 17, 'Reactor3', 14, 'MOX_Source', 'mox', 4),
        (942390000, 0.0444814879803, 17, 'Reactor3', 14, 'MOX_Source', 'mox', 4),
        (922380000, 0.9600000000000, 17, 'Reactor3', 13, 'UOX_Source', 'uox', 3),
        (922380000, 0.9600000000000, 15, 'Reactor1', 13, 'UOX_Source', 'uox', 4),
    ], dtype=ensure_dt_bytes([
        ('NucId', '<i8'), ('Mass', '<f8'), ('ReceiverId', '<i8'),
        ('ReceiverPrototype', 'O'), ('SenderId', '<i8'), ('SenderPrototype', 'O'),
        ('Commodity', 'O'), ('Time', '<i8')
    ]))
    )
    # refs.index = refs.index.astype('str')
    assert_frame_equal(cal, refs)


@dbtest
def test_transactions_activity(db, fname, backend):
    if not HAVE_PYNE:
        raise SkipTest
    evaler = cym.Evaluator(db)
    cal = filters.transactions_activity(evaler)
    exp_head = ['SimId', 'ResourceId', 'NucId', 'Activity', 'ReceiverId', 'ReceiverPrototype',
                'SenderId', 'SenderPrototype', 'TransactionId', 'Commodity', 'Time']
    assert_equal(list(cal), exp_head)  # Check we have the correct headers

    # test single nuclide selection
    cal = filters.transactions_activity(evaler, nucs=['942390000'])
    cal = cal.drop('SimId', 1)  # SimId change at each test need to drop it
    # SimId change at each test need to drop it
    cal = cal.drop('TransactionId', 1)
    # SimId change at each test need to drop it
    cal = cal.drop('ResourceId', 1)
    refs = pd.DataFrame(np.array([
        (942390000, 102084984531.0, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 1),
        (942390000, 102084984531.0, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 2),
        (942390000, 102084984531.0, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 2),
        (942390000, 102084984531.0, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 3),
        (942390000, 102084984531.0, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 3),
        (942390000, 102084984531.0, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 4),
        (942390000, 102084984531.0, 17, 'Reactor3', 14, 'MOX_Source', 'mox', 4),
    ], dtype=ensure_dt_bytes([
        ('NucId', '<i8'), ('Activity', '<f8'), ('ReceiverId', '<i8'),
        ('ReceiverPrototype', 'O'), ('SenderId', '<i8'), ('SenderPrototype', 'O'),
        ('Commodity', 'O'), ('Time', '<i8')
    ]))
    )
    assert_frame_equal(cal, refs)

    # test multiple nuclide selection
    cal = filters.transactions_activity(
        evaler, nucs=['942390000', '922380000'])
    cal = cal.drop('SimId', 1)  # SimId change at each test need to drop it
    # SimId change at each test need to drop it
    cal = cal.drop('TransactionId', 1)
    # SimId change at each test need to drop it
    cal = cal.drop('ResourceId', 1)

    refs = pd.DataFrame(np.array([
        (922380000, 9790360.331530, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 1),
        (942390000, 102084984531.0, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 1),
        (922380000, 9790360.331530, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 2),
        (942390000, 102084984531.0, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 2),
        (922380000, 9790360.331530, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 2),
        (942390000, 102084984531.0, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 2),
        (922380000, 9790360.331530, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 3),
        (942390000, 102084984531.0, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 3),
        (922380000, 9790360.331530, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 3),
        (942390000, 102084984531.0, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 3),
        (922380000, 9790360.331530, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 4),
        (942390000, 102084984531.0, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 4),
        (922380000, 9790360.331530, 17, 'Reactor3', 14, 'MOX_Source', 'mox', 4),
        (942390000, 102084984531.0, 17, 'Reactor3', 14, 'MOX_Source', 'mox', 4),
        (922380000, 11938805.97080, 17, 'Reactor3', 13, 'UOX_Source', 'uox', 3),
        (922380000, 11938805.97080, 15, 'Reactor1', 13, 'UOX_Source', 'uox', 4),
    ], dtype=ensure_dt_bytes([
        ('NucId', '<i8'), ('Activity', '<f8'), ('ReceiverId', '<i8'),
        ('ReceiverPrototype', 'O'), ('SenderId', '<i8'), ('SenderPrototype', 'O'),
        ('Commodity', 'O'), ('Time', '<i8')
    ]))
    )
    assert_frame_equal(cal, refs)


@dbtest
def test_transactions_decayheat(db, fname, backend):
    if not HAVE_PYNE:
        raise SkipTest
    evaler = cym.Evaluator(db)
    cal = filters.transactions_decayheat(evaler)
    exp_head = ['SimId', 'ResourceId', 'NucId', 'DecayHeat', 'ReceiverId', 'ReceiverPrototype',
                'SenderId', 'SenderPrototype', 'TransactionId', 'Commodity', 'Time']
    assert_equal(list(cal), exp_head)  # Check we have the correct headers

    # test single nuclide selection
    cal = filters.transactions_decayheat(evaler, nucs=['942390000'])
    cal = cal.drop('SimId', 1)  # SimId change at each test need to drop it
    # SimId change at each test need to drop it
    cal = cal.drop('TransactionId', 1)
    # SimId change at each test need to drop it
    cal = cal.drop('ResourceId', 1)
    refs = pd.DataFrame(np.array([
        (942390000, 3.34065303191e+30, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 1),
        (942390000, 3.34065303191e+30, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 2),
        (942390000, 3.34065303191e+30, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 2),
        (942390000, 3.34065303191e+30, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 3),
        (942390000, 3.34065303191e+30, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 3),
        (942390000, 3.34065303191e+30, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 4),
        (942390000, 3.34065303191e+30, 17, 'Reactor3', 14, 'MOX_Source', 'mox', 4),
    ], dtype=ensure_dt_bytes([
        ('NucId', '<i8'), ('DecayHeat', '<f8'), ('ReceiverId', '<i8'),
        ('ReceiverPrototype', 'O'), ('SenderId', '<i8'), ('SenderPrototype', 'O'),
        ('Commodity', 'O'), ('Time', '<i8')
    ]))
    )
    assert_frame_equal(cal, refs)

    # test multiple nuclide selection
    cal = filters.transactions_decayheat(
        evaler, nucs=['942390000', '922380000'])
    cal = cal.drop('SimId', 1)  # SimId change at each test need to drop it
    # SimId change at each test need to drop it
    cal = cal.drop('TransactionId', 1)
    # SimId change at each test need to drop it
    cal = cal.drop('ResourceId', 1)
    refs = pd.DataFrame(np.array([
        (922380000, 2.609253035160e26, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 1),
        (942390000, 3.34065303191e+30, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 1),
        (922380000, 2.609253035160e26, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 2),
        (942390000, 3.34065303191e+30, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 2),
        (922380000, 2.609253035160e26, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 2),
        (942390000, 3.34065303191e+30, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 2),
        (922380000, 2.609253035160e26, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 3),
        (942390000, 3.34065303191e+30, 15, 'Reactor1', 14, 'MOX_Source', 'mox', 3),
        (922380000, 2.609253035160e26, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 3),
        (942390000, 3.34065303191e+30, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 3),
        (922380000, 2.609253035160e26, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 4),
        (942390000, 3.34065303191e+30, 16, 'Reactor2', 14, 'MOX_Source', 'mox', 4),
        (922380000, 2.609253035160e26, 17, 'Reactor3', 14, 'MOX_Source', 'mox', 4),
        (942390000, 3.34065303191e+30, 17, 'Reactor3', 14, 'MOX_Source', 'mox', 4),
        (922380000, 3.18184057182e+26, 17, 'Reactor3', 13, 'UOX_Source', 'uox', 3),
        (922380000, 3.18184057182e+26, 15, 'Reactor1', 13, 'UOX_Source', 'uox', 4),
    ], dtype=ensure_dt_bytes([
        ('NucId', '<i8'), ('DecayHeat', '<f8'), ('ReceiverId', '<i8'),
        ('ReceiverPrototype', 'O'), ('SenderId', '<i8'), ('SenderPrototype', 'O'),
        ('Commodity', 'O'), ('Time', '<i8')
    ]))
    )
    assert_frame_equal(cal, refs)


@dbtest
def test_inventories(db, fname, backend):
    evaler = cym.Evaluator(db)
    cal = filters.inventories(evaler)
    exp_head = ['SimId', 'AgentId', 'Prototype',
                'Time', 'InventoryName', 'NucId', 'Quantity', 'Units']
    assert_equal(list(cal), exp_head)  # Check we have the correct headers

    if not HAVE_PYNE:
        raise SkipTest
    cal = filters.inventories(evaler, facilities=['Reactor1'],
                                   nucs=['94239'])
    cal = cal.drop('SimId', 1)  # SimId change at each test need to drop it
    refs = pd.DataFrame(np.array([
        (15, 'Reactor1', 1, 'core',  942390000, 0.0444814879803),
        (15, 'Reactor1', 2, 'core',  942390000, 0.0444814879803),
        (15, 'Reactor1', 2, 'spent', 942390000, 0.0176991150442),
        (15, 'Reactor1', 3, 'core',  942390000, 0.0444814879803),
        (15, 'Reactor1', 3, 'spent', 942390000, 0.0353982300885),
        (15, 'Reactor1', 4, 'spent', 942390000, 0.0530973451327)
    ], dtype=ensure_dt_bytes([
        ('AgentId', '<i8'), ('Prototype', 'O'), ('Time', '<i8'),
        ('InventoryName', 'O'), ('NucId', '<i8'), ('Quantity', '<f8'),
        ('Units', '0')
    ]))
    )
    assert_frame_equal(cal, refs)

    cal = filters.inventories(evaler, facilities=['Reactor1'],
                                   nucs=['94239', '92235'])
    cal = cal.drop('SimId', 1)  # SimId change at each test need to drop it
    refs = pd.DataFrame(np.array([
        (15, 'Reactor1', 1, 'core',  922350000, 0.00157922442534),
        (15, 'Reactor1', 1, 'core',  942390000, 0.0444814879803),
        (15, 'Reactor1', 2, 'core',  922350000, 0.00157922442534),
        (15, 'Reactor1', 2, 'core',  942390000, 0.0444814879803),
        (15, 'Reactor1', 2, 'spent', 922350000, 0.00884955752212),
        (15, 'Reactor1', 2, 'spent', 942390000, 0.0176991150442),
        (15, 'Reactor1', 3, 'core',  922350000, 0.00157922442534),
        (15, 'Reactor1', 3, 'core',  942390000, 0.0444814879803),
        (15, 'Reactor1', 3, 'spent', 922350000, 0.0176991150442),
        (15, 'Reactor1', 3, 'spent', 942390000, 0.0353982300885),
        (15, 'Reactor1', 4, 'core',  922350000, 0.04),
        (15, 'Reactor1', 4, 'spent', 922350000, 0.0265486725664),
        (15, 'Reactor1', 4, 'spent', 942390000, 0.0530973451327)
    ], dtype=ensure_dt_bytes([
        ('AgentId', '<i8'), ('Prototype', 'O'), ('Time', '<i8'),
        ('InventoryName', 'O'), ('NucId', '<i8'), ('Quantity', '<f8'),
        ('Units', '0')

    ]))
    )
    assert_frame_equal(cal, refs)


@dbtest
def test_inventories_activity(db, fname, backend):
    if not HAVE_PYNE:
        raise SkipTest
    evaler = cym.Evaluator(db)
    cal = filters.inventories_activity(evaler)
    exp_head = ['SimId', 'AgentId', 'Prototype', 'Time', 'InventoryName',
                'NucId', 'Quantity', 'Activity']
    assert_equal(list(cal), exp_head)  # Check we have the correct headers

    cal = filters.inventories_activity(evaler, facilities=['Reactor1'],
                                            nucs=['94239'])
    cal = cal.drop('SimId', 1)  # SimId change at each test need to drop it
    refs = pd.DataFrame(np.array([
        (15, 'Reactor1', 1, 'core',  942390000,
         0.0444814879803, 2.44036364223e+13),
        (15, 'Reactor1', 2, 'core',  942390000,
         0.0444814879803, 2.44036364223e+13),
        (15, 'Reactor1', 2, 'spent', 942390000,
         0.0176991150442, 9.71016906463e+12),
        (15, 'Reactor1', 3, 'core',  942390000,
         0.0444814879803, 2.44036364223e+13),
        (15, 'Reactor1', 3, 'spent', 942390000,
         0.0353982300885, 1.94203381293e+13),
        (15, 'Reactor1', 4, 'spent', 942390000, 0.0530973451327, 2.91305071939e+13)
    ], dtype=ensure_dt_bytes([
        ('AgentId', '<i8'), ('Prototype', 'O'), ('Time', '<i8'),
        ('InventoryName', 'O'), ('NucId', '<i8'), ('Quantity', '<f8'),
        ('Activity', '<f8')
    ]))
    )
    assert_frame_equal(cal, refs)

    cal = filters.inventories_activity(evaler, facilities=['Reactor1'],
                                            nucs=['94239', '92235'])
    cal = cal.drop('SimId', 1)  # SimId change at each test need to drop it
    refs = pd.DataFrame(np.array([
        (15, 'Reactor1', 1, 'core',  922350000, 0.00157922442534, 29671782.9213),
        (15, 'Reactor1', 1, 'core',  942390000,
         0.0444814879803, 2.44036364223e+13),
        (15, 'Reactor1', 2, 'core',  922350000, 0.00157922442534, 29671782.9213),
        (15, 'Reactor1', 2, 'core',  942390000,
         0.0444814879803, 2.44036364223e+13),
        (15, 'Reactor1', 2, 'spent', 922350000, 0.00884955752212, 166272852.378),
        (15, 'Reactor1', 2, 'spent', 942390000,
         0.0176991150442, 9.71016906463e+12),
        (15, 'Reactor1', 3, 'core',  922350000, 0.00157922442534, 29671782.9213),
        (15, 'Reactor1', 3, 'core',  942390000,
         0.0444814879803, 2.44036364223e+13),
        (15, 'Reactor1', 3, 'spent', 922350000, 0.0176991150442, 332545704.756),
        (15, 'Reactor1', 3, 'spent', 942390000,
         0.0353982300885, 1.94203381293e+13),
        (15, 'Reactor1', 4, 'core',  922350000, 0.04, 751553292.748),
        (15, 'Reactor1', 4, 'spent', 922350000, 0.0265486725664, 498818557.134),
        (15, 'Reactor1', 4, 'spent', 942390000, 0.0530973451327, 2.91305071939e+13)
    ], dtype=ensure_dt_bytes([
        ('AgentId', '<i8'), ('Prototype', 'O'), ('Time', '<i8'),
        ('InventoryName', 'O'), ('NucId', '<i8'), ('Quantity', '<f8'),
        ('Activity', '<f8')
    ]))
    )
    assert_frame_equal(cal, refs)


@dbtest
def test_inventories_decayheat(db, fname, backend):
    if not HAVE_PYNE:
        raise SkipTest
    evaler = cym.Evaluator(db)
    cal = filters.inventories_decayheat(evaler)
    exp_head = ['SimId', 'AgentId', 'Prototype', 'Time', 'InventoryName',
                'NucId', 'Quantity', 'Activity', 'DecayHeat']
    assert_equal(list(cal), exp_head)  # Check we have the correct headers

    cal = filters.inventories_decayheat(evaler, facilities=['Reactor1'],
                                             nucs=['94239'])
    cal = cal.drop('SimId', 1)  # SimId change at each test need to drop it
    refs = pd.DataFrame(np.array([
        (15, 'Reactor1', 1, 'core',  942390000,
         0.0444814879803, 2.44036364223e+13, 7.98590335085e+32),
        (15, 'Reactor1', 2, 'core',  942390000,
         0.0444814879803, 2.44036364223e+13, 7.98590335085e+32),
        (15, 'Reactor1', 2, 'spent', 942390000,
         0.0176991150442, 9.71016906463e+12, 3.17757855136e+32),
        (15, 'Reactor1', 3, 'core',  942390000,
         0.0444814879803, 2.44036364223e+13, 7.98590335085e+32),
        (15, 'Reactor1', 3, 'spent', 942390000,
         0.0353982300885, 1.94203381293e+13, 6.35515710272e+32),
        (15, 'Reactor1', 4, 'spent', 942390000,
         0.0530973451327, 2.91305071939e+13, 9.53273565408e+32)
    ], dtype=ensure_dt_bytes([
        ('AgentId', '<i8'), ('Prototype', 'O'), ('Time', '<i8'),
        ('InventoryName', 'O'), ('NucId', '<i8'), ('Quantity', '<f8'),
        ('Activity', '<f8'), ('DecayHeat', '<f8')
    ]))
    )
    assert_frame_equal(cal, refs)

    cal = filters.inventories_decayheat(evaler, facilities=['Reactor1'],
                                             nucs=['94239', '92235'])
    cal = cal.drop('SimId', 1)  # SimId change at each test need to drop it
    refs = pd.DataFrame(np.array([
        (15, 'Reactor1', 1, 'core',  922350000,
         0.00157922442534, 29671782.9213, 8.65609466244e+26),
        (15, 'Reactor1', 1, 'core',  942390000,
         0.0444814879803, 2.44036364223e+13, 7.98590335085e+32),
        (15, 'Reactor1', 2, 'core',  922350000,
         0.00157922442534, 29671782.9213, 8.65609466244e+26),
        (15, 'Reactor1', 2, 'core',  942390000,
         0.0444814879803, 2.44036364223e+13, 7.98590335085e+32),
        (15, 'Reactor1', 2, 'spent', 922350000,
         0.00884955752212, 166272852.378, 4.85064734329e+27),
        (15, 'Reactor1', 2, 'spent', 942390000,
         0.0176991150442, 9.71016906463e+12, 3.17757855136e+32),
        (15, 'Reactor1', 3, 'core',  922350000,
         0.00157922442534, 29671782.9213, 8.65609466244e+26),
        (15, 'Reactor1', 3, 'core',  942390000,
         0.0444814879803, 2.44036364223e+13, 7.98590335085e+32),
        (15, 'Reactor1', 3, 'spent', 922350000,
         0.0176991150442, 332545704.756, 9.70129468658e+27),
        (15, 'Reactor1', 3, 'spent', 942390000,
         0.0353982300885, 1.94203381293e+13, 6.35515710272e+32),
        (15, 'Reactor1', 4, 'core',  922350000,
         0.04, 751553292.748, 2.19249259917e+28),
        (15, 'Reactor1', 4, 'spent', 922350000,
         0.0265486725664, 498818557.134, 1.45519420299e+28),
        (15, 'Reactor1', 4, 'spent', 942390000,
         0.0530973451327, 2.91305071939e+13, 9.53273565408e+32)
    ], dtype=ensure_dt_bytes([
        ('AgentId', '<i8'), ('Prototype', 'O'), ('Time', '<i8'),
        ('InventoryName', 'O'), ('NucId', '<i8'), ('Quantity', '<f8'),
        ('Activity', '<f8'), ('DecayHeat', '<f8')
    ]))
    )
    assert_frame_equal(cal, refs)


if __name__ == "__main__":
    nose.runmodule()

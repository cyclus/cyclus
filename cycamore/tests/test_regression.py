#! /usr/bin/env python
import os
import uuid
import sqlite3
import platform

import tables
import numpy as np
from numpy.testing import assert_array_almost_equal
from numpy.testing import assert_almost_equal
from cyclus.lib import Env

from nose.plugins.skip import SkipTest
from nose.tools import assert_equal, assert_true


import helper
from helper import check_cmd, run_cyclus, table_exist, cyclus_has_coin


ALLOW_MILPS = Env().allow_milps


def skip_if_dont_allow_milps():
    """A don't run certain tests if MILPs are disabled."""
    if not ALLOW_MILPS:
        raise SkipTest("Cyclus was compiled without MILPS support or the "
                       "ALLOW_MILPS env var was not set to true.")


class TestRegression(object):
    """A base class for all regression tests. A derived class is required for
    each new input file to be tested. Each derived class *must* declare an `inf`
    member in their `__init__` function that points to the input file to be
    tested, e.g., `self.inf_ = ./path/to/my/input_file.xml. See below for
    examples.
    """
    def __init__(self, *args, **kwargs):
        self.ext = '.sqlite'
        self.outf = str(uuid.uuid4()) + self.ext
        self.inf = None

    def setUp(self):
        if not self.inf:
            raise TypeError(("self.inf must be set in derived classes "
                             "to run regression tests."))
        run_cyclus("cyclus", os.getcwd(), self.inf, self.outf)

        # Get specific tables and columns
        if self.ext == '.h5':
            with tables.open_file(self.outf, mode="r") as f:
                # Get specific tables and columns
                self.agent_entry = f.get_node("/AgentEntry")[:]
                self.agent_exit = f.get_node("/AgentExit")[:] \
                    if "/AgentExit" in f \
                    else None
                self.enrichments = f.get_node("/Enrichments")[:] \
                    if "/Enrichments" in f \
                    else None
                self.resources = f.get_node("/Resources")[:]
                self.transactions = f.get_node("/Transactions")[:]
                self.compositions = f.get_node("/Compositions")[:]
                self.info = f.get_node("/Info")[:]
                self.rsrc_qtys = {
                    x["ResourceId"]: x["Quantity"] for x in self.resources}
        else:
            self.conn = sqlite3.connect(self.outf)
            self.conn.row_factory = sqlite3.Row
            self.cur = self.conn.cursor()
            exc = self.cur.execute
            self.agent_entry = exc('SELECT * FROM AgentEntry').fetchall()
            self.agent_exit = exc('SELECT * FROM AgentExit').fetchall() \
                if len(exc(
                    ("SELECT * FROM sqlite_master WHERE "
                     "type='table' AND name='AgentExit'")).fetchall()) > 0 \
                     else None
            self.enrichments = exc('SELECT * FROM Enrichments').fetchall() \
                if len(exc(
                    ("SELECT * FROM sqlite_master WHERE "
                     "type='table' AND name='Enrichments'")).fetchall()) > 0 \
                     else None
            self.resources = exc('SELECT * FROM Resources').fetchall()
            self.transactions = exc('SELECT * FROM Transactions').fetchall()
            self.compositions = exc('SELECT * FROM Compositions').fetchall()
            self.info = exc('SELECT * FROM Info').fetchall()
            self.rsrc_qtys = {
                x["ResourceId"]: x["Quantity"] for x in self.resources}

    def find_ids(self, spec, a, spec_col="Spec", id_col="AgentId"):
        if self.ext == '.h5':
            return helper.find_ids(spec, a[spec_col], a[id_col])
        else:
            return [x[id_col] for x in a if x[spec_col] == spec]

    def to_ary(self, a, k):
        if self.ext == '.sqlite':
            return np.array([x[k] for x in a])
        else:
            return a[k]

    def tearDown(self):
        if self.ext == '.sqlite':
            self.conn.close()
        if os.path.isfile(self.outf):
            print("removing {0}".format(self.outf))
            os.remove(self.outf)

class _PhysorEnrichment(TestRegression):
    """This class tests the 1_Enrichment_2_Reactor.xml file related to the
    Cyclus Physor 2014 publication. The number of key facilities, the enrichment
    values, and the transactions to each reactor are tested.
    """
    def __init__(self, *args, **kwargs):
        super(_PhysorEnrichment, self).__init__(*args, **kwargs)

    def setUp(self):
        super(_PhysorEnrichment, self).setUp()
        tbl = self.agent_entry
        self.rx_id = self.find_ids(":cycamore:Reactor", tbl)
        self.enr_id = self.find_ids(":cycamore:Enrichment", tbl)

    def test_deploy(self):
        assert_equal(len(self.rx_id), 2)
        assert_equal(len(self.enr_id), 1)

    def test_swu(self):
        enr = self.enrichments
        # this can be updated if/when we can call into the cyclus::toolkit's
        # enrichment module from python
        # with old BatchReactor: exp = [6.9, 10, 4.14, 6.9]
        exp = [6.85, 9.94, 4.11, 6.85]
        obs = [np.sum(self.to_ary(enr, "SWU")[
                    self.to_ary(enr, "Time") == t]) for t in range(4)]
        assert_array_almost_equal(exp, obs, decimal=2)

    def test_nu(self):
        enr = self.enrichments
        # this can be updated if/when we can call into the cyclus::toolkit's
        # enrichment module from python

        # with old BatchReactor: exp = [13.03, 16.54, 7.83, 13.03]
        exp = [13.14, 16.69, 7.88, 13.14]
        obs = [np.sum(self.to_ary(enr, "Natural_Uranium")[
                    self.to_ary(enr, "Time") == t]) for t in range(4)]
        assert_array_almost_equal(exp, obs, decimal=2)

    def test_xactions1(self):
        # reactor 1 transactions
        exp = [1, 1, 1, 1]
        txs = [0, 0, 0, 0]
        for tx in self.transactions:
            if tx['ReceiverId'] == self.rx_id[0]:
                txs[tx['Time']] += self.rsrc_qtys[tx['ResourceId']]

        msg = "Testing that first reactor gets less than it wants."
        assert_array_almost_equal(exp, txs, decimal=2, err_msg=msg)

    def test_xactions2(self):
        # reactor 2 transactions
        exp = [1, 0.8, 0.2, 1]
        txs = [0, 0, 0, 0]
        for tx in self.transactions:
            if tx['ReceiverId'] == self.rx_id[1]:
                txs[tx['Time']] += self.rsrc_qtys[tx['ResourceId']]

        msg = "Testing that second reactor gets what it wants."
        assert_array_almost_equal(exp, txs, decimal=2, err_msg=msg)

class TestCBCPhysorEnrichment(_PhysorEnrichment):
    def __init__(self, *args, **kwargs):
        super(TestCBCPhysorEnrichment, self).__init__(*args, **kwargs)
        self.inf = "../input/physor/1_Enrichment_2_Reactor.xml"
        skip_if_dont_allow_milps()

class TestGreedyPhysorEnrichment(_PhysorEnrichment):
    def __init__(self, *args, **kwargs):
        super(TestGreedyPhysorEnrichment, self).__init__(*args, **kwargs)
        self.inf = "../input/physor/greedy_1_Enrichment_2_Reactor.xml"

class _PhysorSources(TestRegression):
    """This class tests the 2_Sources_3_Reactor.xml file related to the Cyclus
    Physor 2014 publication. Reactor deployment and transactions between
    suppliers and reactors are tested.
    """
    def __init__(self, *args, **kwargs):
        super(_PhysorSources, self).__init__(*args, **kwargs)

    def setUp(self):
        super(_PhysorSources, self).setUp()

        # identify each reactor and supplier by id
        tbl = self.agent_entry
        rx_id = self.find_ids(":cycamore:Reactor", tbl)
        self.r1, self.r2, self.r3 = tuple(rx_id)
        s_id = self.find_ids(":cycamore:Source", tbl)
        self.smox = self.transactions[0]["SenderId"]
        s_id.remove(self.smox)
        self.suox = s_id[0]

    def test_rxtr_deployment(self):
        depl_time = {x["AgentId"]: x["EnterTime"] for x in self.agent_entry}

        assert_equal(depl_time[self.r1], 1)
        assert_equal(depl_time[self.r2], 2)
        assert_equal(depl_time[self.r3], 3)

    def test_rxtr1_xactions(self):
        mox_exp = [0, 1, 1, 1, 0]
        txs = [0, 0, 0, 0, 0]
        for tx in self.transactions:
            if tx['ReceiverId'] == self.r1 and tx['SenderId'] == self.smox:
                txs[tx['Time']] += self.rsrc_qtys[tx['ResourceId']]
        assert_array_almost_equal(mox_exp, txs)

        uox_exp = [0, 0, 0, 0, 1]
        txs = [0, 0, 0, 0, 0]
        for tx in self.transactions:
            if tx['ReceiverId'] == self.r1 and tx['SenderId'] == self.suox:
                txs[tx['Time']] += self.rsrc_qtys[tx['ResourceId']]
        assert_array_almost_equal(uox_exp, txs)

    def test_rxtr2_xactions(self):
        mox_exp = [0, 0, 1, 1, 1]
        txs = [0, 0, 0, 0, 0]
        for tx in self.transactions:
            if tx['ReceiverId'] == self.r2 and tx['SenderId'] == self.smox:
                txs[tx['Time']] += self.rsrc_qtys[tx['ResourceId']]
        assert_array_almost_equal(mox_exp, txs)

        uox_exp = [0, 0, 0, 0, 0]
        txs = [0, 0, 0, 0, 0]
        for tx in self.transactions:
            if tx['ReceiverId'] == self.r2 and tx['SenderId'] == self.suox:
                txs[tx['Time']] += self.rsrc_qtys[tx['ResourceId']]
        assert_array_almost_equal(uox_exp, txs)

    def test_rxtr3_xactions(self):
        mox_exp = [0, 0, 0, 0.5, 1]
        txs = [0, 0, 0, 0, 0]
        for tx in self.transactions:
            if tx['ReceiverId'] == self.r3 and tx['SenderId'] == self.smox:
                txs[tx['Time']] += self.rsrc_qtys[tx['ResourceId']]
        assert_array_almost_equal(mox_exp, txs)

        uox_exp = [0, 0, 0, 0.5, 0]
        txs = [0, 0, 0, 0, 0]
        for tx in self.transactions:
            if tx['ReceiverId'] == self.r3 and tx['SenderId'] == self.suox:
                txs[tx['Time']] += self.rsrc_qtys[tx['ResourceId']]
        assert_array_almost_equal(uox_exp, txs)

class TestCBCPhysorSources(_PhysorSources):
    def __init__(self, *args, **kwargs):
        super(TestCBCPhysorSources, self).__init__(*args, **kwargs)
        self.inf = "../input/physor/2_Sources_3_Reactors.xml"
        skip_if_dont_allow_milps()

class TestGreedyPhysorSources(_PhysorSources):
    def __init__(self, *args, **kwargs):
        super(TestGreedyPhysorSources, self).__init__(*args, **kwargs)
        self.inf = "../input/physor/greedy_2_Sources_3_Reactors.xml"

class TestDynamicCapacitated(TestRegression):
    """Tests dynamic capacity restraints involving changes in the number of
    source and sink facilities.

    A source facility is expected to offer a commodity of amount 1,
    and a sink facility is expected to request for a commodity of amount 1.
    Therefore, number of facilities correspond to the amounts of offers
    and requests.

    At time step 1, 3 source facilities and 2 sink facilities are deployed, and
    at time step 2, additional 2 sink facilities are deployed. After time
    step 2, the older 2 sink facilities are decommissioned.
    According to this deployment schedule, at time step 1, only 2 transactions
    are expected, the number of sink facilities being the constraint; whereas,
    at time step 2, only 3 transactions are expected, the number of source
    facilities being the constraint. At time step 3, after decommissioning 2
    older sink facilities, the remaining number of sink facilities becomes
    the constraint, resulting in the same transaction amount as in time step 1.
    """
    def __init__(self, *args, **kwargs):
        super(TestDynamicCapacitated, self).__init__(*args, **kwargs)
        self.inf = "./input/dynamic_capacitated.xml"
        if not cyclus_has_coin():
            raise SkipTest('Cyclus not compiled with COIN')

    def setUp(self):
        super(TestDynamicCapacitated, self).setUp()

        # Find agent ids of source and sink facilities
        self.agent_ids = self.to_ary(self.agent_entry, "AgentId")
        self.agent_impl = self.to_ary(self.agent_entry, "Spec")
        self.depl_time = self.to_ary(self.agent_entry, "EnterTime")
        self.exit_time = self.to_ary(self.agent_exit, "ExitTime")
        self.exit_ids = self.to_ary(self.agent_exit, "AgentId")
        self.source_id = self.find_ids(":cycamore:Source", self.agent_entry)
        self.sink_id = self.find_ids(":cycamore:Sink", self.agent_entry)

        # Check transactions
        self.sender_ids = self.to_ary(self.transactions, "SenderId")
        self.receiver_ids = self.to_ary(self.transactions, "ReceiverId")
        self.trans_time = self.to_ary(self.transactions, "Time")
        self.trans_resource = self.to_ary(self.transactions, "ResourceId")

        # Track transacted resources
        self.resource_ids = self.to_ary(self.resources, "ResourceId")
        self.quantities = self.to_ary(self.resources, "Quantity")

    def tearDown(self):
        super(TestDynamicCapacitated, self).tearDown()

    def test_source_deployment(self):
        # test number of sources
        assert_equal(len(self.source_id), 3)
        # Test that source facilities are all deployed at time step 1
        for s in self.source_id:
            assert_equal(self.depl_time[np.where(self.agent_ids == s)], 1)

    def test_sink_deployment(self):
        # test number of sinks
        assert_equal(len(self.sink_id), 4)
        # Test that first 2 sink facilities are deployed at time step 1
        # and decommissioned at time step 2
        for i in [0, 1]:
            assert_equal(
                self.depl_time[np.where(self.agent_ids == self.sink_id[i])][0],
                1)
            assert_equal(
                self.exit_time[np.where(self.exit_ids == self.sink_id[i])][0],
                2)
        # Test that second 2 sink facilities are deployed at time step 2
        # and decommissioned at time step 3
        for i in [2, 3]:
            assert_equal(
                self.depl_time[np.where(self.agent_ids == self.sink_id[i])][0],
                2)
            assert_equal(
                self.exit_time[np.where(self.exit_ids == self.sink_id[i])][0],
                3)

    def test_xaction_general(self):
        # Check that transactions are between sources and sinks only
        for s in self.sender_ids:
            assert_equal(len(np.where(self.source_id == s)[0]), 1)
        for r in self.receiver_ids:
            assert_equal(len(np.where(self.sink_id == r)[0]), 1)
        # Total expected number of transactions
        assert_equal(len(self.trans_time), 7)
        # Check that at time step 1, there are 2 transactions
        assert_equal(len(np.where(self.trans_time == 1)[0]), 2)
        # Check that at time step 2, there are 3 transactions
        assert_equal(len(np.where(self.trans_time == 2)[0]), 3)
        # Check that at time step 3, there are 2 transactions
        assert_equal(len(np.where(self.trans_time == 3)[0]), 2)

    def test_xaction_specific(self):
        # Check that at time step 1, there are 2 transactions with total
        # amount of 2
        quantity = 0
        for t in np.where(self.trans_time == 1)[0]:
            quantity += self.quantities[
                np.where(self.resource_ids == self.trans_resource[t])]
        assert_equal(quantity, 2)

        # Check that at time step 2, there are 3 transactions with total
        # amount of 3
        quantity = 0
        for t in np.where(self.trans_time == 2)[0]:
            quantity += self.quantities[
                np.where(self.resource_ids == self.trans_resource[t])]
        assert_equal(quantity, 3)

        # Check that at time step 3, there are 2 transactions with total
        # amount of 2
        quantity = 0
        for t in np.where(self.trans_time == 3)[0]:
            quantity += self.quantities[
                np.where(self.resource_ids == self.trans_resource[t])]
        assert_equal(quantity, 2)

class TestGrowth(TestRegression):
    """Tests GrowthRegion, ManagerInst, and Source over a 4-time step
    simulation.

    A linear growth demand (y = x + 2) is provided to the growth region. Two
    Sources are allowed in the ManagerInst, with capacities of 2 and 1.1,
    respectively. At t=1, a 2-capacity Source is expected to be built, and at
    t=2 and t=3, 1-capacity Sources are expected to be built.

    A linear growth demand (y = 0x + 3) for a second commodity is provided at t=2
    to test the demand for multiple commodities.
    """
    def __init__(self, *args, **kwargs):
        super(TestGrowth, self).__init__(*args, **kwargs)
        self.inf = "./input/growth.xml"
        if not cyclus_has_coin():
            raise SkipTest('Cyclus not compiled with COIN')

    def setUp(self):
        super(TestGrowth, self).setUp()

    def tearDown(self):
        super(TestGrowth, self).tearDown()

    def test_deployment(self):
        pass
        agent_ids = self.to_ary(self.agent_entry, "AgentId")
        proto = self.to_ary(self.agent_entry, "Prototype")
        enter_time = self.to_ary(self.agent_entry, "EnterTime")

        source1_id = self.find_ids("Source1", self.agent_entry,
                                   spec_col="Prototype")
        source2_id = self.find_ids("Source2", self.agent_entry,
                                   spec_col="Prototype")
        source3_id = self.find_ids("Source3", self.agent_entry,
                                   spec_col="Prototype")

        assert_equal(len(source2_id), 1)
        assert_equal(len(source1_id), 2)
        assert_equal(len(source3_id), 3)

        assert_equal(enter_time[np.where(agent_ids == source2_id[0])], 1)
        assert_equal(enter_time[np.where(agent_ids == source1_id[0])], 2)
        assert_equal(enter_time[np.where(agent_ids == source1_id[1])], 3)
        for x in source3_id:
            assert_equal(enter_time[np.where(agent_ids == x)], 2)

class _Recycle(TestRegression):
    """This class tests the input/recycle.xml file.
    """
    def __init__(self, *args, **kwargs):
        super(_Recycle, self).__init__(*args, **kwargs)

        # this test requires separations which isn't supported by hdf5
        # so we force sqlite:
        base, _ = os.path.splitext(self.outf)
        self.ext = '.sqlite'
        self.outf = base + self.ext
        self.sql = """
            SELECT t.time as time,SUM(c.massfrac*r.quantity) as qty FROM transactions as t
            JOIN resources as r ON t.resourceid=r.resourceid AND r.simid=t.simid
            JOIN agententry as send ON t.senderid=send.agentid AND send.simid=t.simid
            JOIN agententry as recv ON t.receiverid=recv.agentid AND recv.simid=t.simid
            JOIN compositions as c ON c.qualid=r.qualid AND c.simid=r.simid
            WHERE send.prototype=? AND recv.prototype=? AND c.nucid=?
            GROUP BY t.time;"""

    def do_compare(self, fromfac, tofac, nuclide, exp_invs):
        conn = sqlite3.connect(self.outf)
        c = conn.cursor()
        eps = 1e-10
        simdur = len(exp_invs)

        invs = [0.0] * simdur
        for i, row in enumerate(c.execute(self.sql, (fromfac, tofac, nuclide))):
            t = row[0]
            invs[t] = row[1]

        expfname = 'exp_recycle_{0}-{1}-{2}.dat'.format(fromfac, tofac, nuclide)
        with open(expfname, 'w') as f:
            for t, val in enumerate(exp_invs):
                f.write('{0} {1}\n'.format(t, val))
        obsfname = 'obs_recycle_{0}-{1}-{2}.dat'.format(fromfac, tofac, nuclide)
        with open(obsfname, 'w') as f:
            for t, val in enumerate(invs):
                f.write('{0} {1}\n'.format(t, val))

        i = 0
        for exp, obs in zip(invs, exp_invs):
            assert_almost_equal(
                exp, obs, err_msg='mismatch at t={}, {} != {}'.format(i, exp, obs))
            i += 1

        os.remove(expfname)
        os.remove(obsfname)

    def test_pu239_sep_repo(self):
        simdur = 600
        exp = [0.0] * simdur
        exp[18] = 1.70022267
        exp[37] = 1.70022267
        exp[56] = 1.70022267
        exp[75] = 1.70022267
        exp[94] = 1.70022267
        exp[113] = 1.70022267
        exp[132] = 1.70022267
        exp[151] = 1.70022267
        exp[170] = 1.70022267
        exp[189] = 1.70022267
        exp[208] = 1.70022267
        exp[246] = 1.70022267
        exp[265] = 1.70022267
        exp[284] = 1.70022267
        exp[303] = 1.70022267
        exp[322] = 1.70022267
        exp[341] = 1.70022267
        exp[360] = 1.70022267
        exp[379] = 1.70022267
        exp[417] = 1.70022267
        exp[436] = 1.70022267
        exp[455] = 1.70022267
        exp[474] = 1.70022267
        exp[493] = 1.70022267
        exp[512] = 1.70022267
        exp[531] = 1.70022267
        exp[569] = 1.70022267
        exp[588] = 1.70022267

        self.do_compare('separations', 'repo', 942390000, exp)

    def test_pu239_reactor_repo(self):
        simdur = 600
        exp = [0.0] * simdur
        exp[226] = 420.42772559790944
        exp[397] = 420.42772559790944
        exp[549] = 420.42772559790944
        self.do_compare('reactor', 'repo', 942390000, exp)

class TestGreedyRecycle(_Recycle):
    """This class tests the input/recycle.xml file.
    """
    def __init__(self, *args, **kwargs):
        super(TestGreedyRecycle, self).__init__(*args, **kwargs)
        self.inf = "../input/greedy_recycle.xml"

class TestCbcRecycle(_Recycle):
    """This class tests the input/recycle.xml file.
    """
    def __init__(self, *args, **kwargs):
        super(TestCbcRecycle, self).__init__(*args, **kwargs)
        self.inf = "../input/recycle.xml"
        skip_if_dont_allow_milps()

#! /usr/bin/env python

from numpy.testing import assert_array_equal
import os
import sqlite3
import tables
import numpy as np
import pytest

from tools import check_cmd, cyclus_has_coin, thread_count
from helper import tables_exist, find_ids, create_sim_input, \
    h5_suffix, clean_outs, to_ary, which_outfile

INPUT = os.path.join(os.path.dirname(__file__), "input")

def test_source_to_sink(thread_count):
    """Tests simulations with one facility that has a conversion factor.

    The trivial cycle simulation involves only one KFacility which provides
    what it requests itself. The conversion factors for requests and bids
    are kept the same so that the facility provides exactly what it requests.
    The amount of the transactions follow a power law.

    Amount = InitialAmount * ConversionFactor ^ Time

    This equation is used to test each transaction amount.
    """
    if not cyclus_has_coin():
        pytest.skip("Cyclus does not have COIN")

    # A reference simulation input for the trivial cycle simulation.
    ref_input = os.path.join(INPUT, "trivial_cycle.xml")
    # Conversion factors for the three simulations
    k_factors = [0.95, 1, 2]

    for k_factor in k_factors:
        clean_outs()

        sim_input = create_sim_input(ref_input, k_factor, k_factor)

        holdsrtn = [1]  # needed because nose does not send() to test generator
        outfile = which_outfile(thread_count)
        cmd = ["cyclus", "-j", thread_count, "-o", outfile, "--input-file", sim_input]
        check_cmd(cmd, '.', holdsrtn)
        rtn = holdsrtn[0]
        if rtn != 0:
            return  # don't execute further commands

        # tables of interest
        paths = ["/AgentEntry", "/Resources", "/Transactions",
                "/Info"]
        # Check if these tables exist
        assert  tables_exist(outfile, paths)
        if not tables_exist(outfile, paths):
            outfile.close()
            clean_outs()
            return  # don't execute further commands

        # Get specific tables and columns
        if outfile.endswith(h5_suffix):
            output = tables.open_file(outfile, mode = "r")
            agent_entry = output.get_node("/AgentEntry")[:]
            info = output.get_node("/Info")[:]
            resources = output.get_node("/Resources")[:]
            transactions = output.get_node("/Transactions")[:]
            output.close()
        else:
            conn = sqlite3.connect(outfile)
            conn.row_factory = sqlite3.Row
            cur = conn.cursor()
            exc = cur.execute
            agent_entry = exc('SELECT * FROM AgentEntry').fetchall()
            info = exc('SELECT * FROM Info').fetchall()
            resources = exc('SELECT * FROM Resources').fetchall()
            transactions = exc('SELECT * FROM Transactions').fetchall()
            conn.close()

        # Find agent ids
        agent_ids = to_ary(agent_entry, "AgentId")
        spec = to_ary(agent_entry, "Spec")

        facility_id = find_ids(":agents:KFacility", spec, agent_ids)
        # Test for only one KFacility
        assert len(facility_id) == 1

        sender_ids = to_ary(transactions, "SenderId")
        receiver_ids = to_ary(transactions, "ReceiverId")
        expected_sender_array = np.empty(sender_ids.size)
        expected_sender_array.fill(facility_id[0])
        expected_receiver_array = np.empty(receiver_ids.size)
        expected_receiver_array.fill(facility_id[0])
        assert_array_equal, sender_ids, expected_sender_array
        assert_array_equal, receiver_ids, expected_receiver_array

        # Transaction ids must be equal range from 1 to the number of rows
        expected_trans_ids = np.arange(0, sender_ids.size, 1)
        assert_array_equal, \
            to_ary(transactions, "TransactionId"), \
                   expected_trans_ids

        # Track transacted resources
        resource_ids = to_ary(resources, "ResourceId")
        quantities = to_ary(resources, "Quantity")

        # Almost equal cases due to floating point k_factors
        i = 0
        initial_capacity = quantities[0]
        for q in quantities:
            assert pytest.approx(q, abs=1e-7) == initial_capacity * k_factor ** i
            i += 1

        clean_outs()
        os.remove(sim_input)

#! /usr/bin/env python

from nose.tools import assert_equal, assert_almost_equal, assert_true
from numpy.testing import assert_array_equal
import os
import tables
import numpy as np
from tools import check_cmd
from helper import table_exist, find_ids, exit_times, create_sim_input, \
    h5out, sqliteout, clean_outs

"""Tests"""
def test_source_to_sink():
    """Tests simulations with one facility that has a conversion factor.

    The trivial cycle simulation involves only one KFacility which provides
    what it requests itself. The conversion factors for requests and bids
    are kept the same so that the facility provides exactly what it requests.
    The amount of the transactions follow a power law.

    Amount = InitialAmount * ConversionFactor ^ Time

    This equation is used to test each transaction amount.
    """
    # A reference simulation input for the trivial cycle simulation.
    ref_input = "./input/trivial_cycle.xml"
    # Conversion factors for the three simulations
    k_factors = [0.95, 1, 2]

    for k_factor in k_factors:
        clean_outs()

        sim_input = create_sim_input(ref_input, k_factor, k_factor)

        holdsrtn = [1]  # needed because nose does not send() to test generator
        cmd = ["cyclus", "-o", h5out, "--input-file", sim_input]
        yield check_cmd, cmd, '.', holdsrtn
        rtn = holdsrtn[0]
        if rtn != 0:
            return  # don't execute further commands

        output = tables.open_file(h5out, mode = "r")
        # tables of interest
        paths = ["/AgentEntry", "/Resources", "/Transactions",
                "/Info"]
        # Check if these tables exist
        yield assert_true, table_exist(output, paths)
        if not table_exist(output, paths):
            output.close()
            clean_outs()
            return  # don't execute further commands

        # Get specific tables and columns
        agent_entry = output.get_node("/AgentEntry")[:]
        info = output.get_node("/Info")[:]
        resources = output.get_node("/Resources")[:]
        transactions = output.get_node("/Transactions")[:]

        # Find agent ids
        agent_ids = agent_entry["AgentId"]
        agent_impl = agent_entry["Implementation"]

        facility_id = find_ids("KFacility", agent_impl, agent_ids)
        # Test for only one KFacility
        yield assert_equal, len(facility_id), 1

        sender_ids = transactions["SenderId"]
        receiver_ids = transactions["ReceiverId"]
        expected_sender_array = np.empty(sender_ids.size)
        expected_sender_array.fill(facility_id[0])
        expected_receiver_array = np.empty(receiver_ids.size)
        expected_receiver_array.fill(facility_id[0])
        yield assert_array_equal, sender_ids, expected_sender_array
        yield assert_array_equal, receiver_ids, expected_receiver_array

        # Transaction ids must be equal range from 1 to the number of rows
        expected_trans_ids = np.arange(0, sender_ids.size, 1)
        yield assert_array_equal, transactions["TransactionId"], expected_trans_ids

        # Track transacted resources
        resource_ids = resources["ResourceId"]
        quantities = resources["Quantity"]

        # Almost equal cases due to floating point k_factors
        i = 0
        initial_capacity = quantities[0]
        for q in quantities:
            yield assert_almost_equal, q, initial_capacity * k_factor ** i
            i += 1

        output.close()
        clean_outs()
        os.remove(sim_input)

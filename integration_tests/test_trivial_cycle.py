#! /usr/bin/python

from nose.tools import assert_equal, assert_true
from numpy.testing import assert_array_equal
import os
import tables
import numpy as np
from tools import check_cmd

""" Tests """
def test_source_to_sink():
    """ Tests simulations with a facilty that has a conversion factor.
    In future, may eliminate checks if needed tables exist, and rely on errors.
    In future, may eliminate tests for the existance and uniqueness of the
    facilities. In addition, may eliminate other non-integration testing relevant
    code and tests.
    """
    # Cyclus simulation input for source_to_sink
    sim_inputs = ["./Inputs/trivial_cycle_deplete.xml",
                  "./Inputs/trivial_cycle_steady.xml",
                  "./Inputs/trivial_cycle_growth.xml"]
    # Conversion factors for the three simulations
    k_factors = [0.5, 1, 2]

    sim_info = zip(sim_inputs, k_factors)

    for sim_input, k_factor in sim_info:
        holdsrtn = [1]  # needed because nose does not send() to test generator
        cmd = ["cyclus", "-o", "./output_temp.h5", "--input-file", sim_input]
        check_cmd(cmd, '.', holdsrtn)
        rtn = holdsrtn[0]
        if rtn != 0:
            return  # don't execute further commands

        output = tables.open_file("./output_temp.h5", mode = "r")
        # tables of interest
        paths = ["/AgentEntry", "/AgentExit", "/Resources", "/Transactions",
                "/Info"]
        # Check if these tables exist
        tables_there = True
        for path in paths:
            yield assert_true, output.__contains__(path)
            # Have to stop further operations after these tests
            if tables_there and not output.__contains__(path):
                tables_there = False

        if not tables_there:
            output.close()
            os.remove("./output_temp.h5")
            return

        # Get specific tables and columns
        agent_entry = output.get_node("/AgentEntry")[:]
        agent_exit = output.get_node("/AgentExit")[:]
        info = output.get_node("/Info")[:]
        resources = output.get_node("/Resources")[:]
        transactions = output.get_node("/Transactions")[:]

        # Find agent ids
        agent_ids = agent_entry["AgentId"]
        agent_impl = agent_entry["Implementation"]
        duration = info["Duration"][0]
        facility_id = []
        i = 0
        for impl in agent_impl:
            if impl == "KFacility":
                facility_id.append(i)
            i += 1
        yield assert_equal, len(facility_id), 1
        facility_id = agent_ids[facility_id[0]]

        sender_ids = transactions["SenderId"]
        receiver_ids = transactions["ReceiverId"]
        expected_sender_array = np.empty(sender_ids.size)
        expected_sender_array.fill(facility_id)
        expected_receiver_array = np.empty(receiver_ids.size)
        expected_receiver_array.fill(sink_id)
        yield assert_array_equal, sender_ids, expected_sender_array
        yield assert_array_equal, receiver_ids, expected_receiver_array

        # Transaction ids must be equal range from 1 to the number of rows
        expected_trans_ids = np.arange(0, sender_ids.size, 1)
        yield assert_array_equal, transactions["TransactionId"], expected_trans_ids

        # Track transacted resources
        resource_ids = resources["ResourceId"]
        quantities = resources["Quantity"]
        expected_quantities = np.empty(resource_ids.size)
        # Expect that every transaction quantity is the same amount
        initial_inv = quantities[0]
        for i in range(expected_quantities.size):
            expected_quantities[i] = initial_inv * k_factor ** i

        # Should find a logic for almost equal cases
        yield assert_array_equal, quantities, expected_quantities

        output.close()
        os.remove("./output_temp.h5")

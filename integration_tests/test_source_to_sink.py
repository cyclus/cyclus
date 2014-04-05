#! /usr/bin/python

from nose.tools import assert_equal, assert_true
from numpy.testing import assert_array_equal
import os
import tables
import numpy as np
from tools import check_cmd
from helper import table_exist, find_ids, exit_times

""" Tests """
def test_source_to_sink():
    """ Tests linear growth of sink inventory by checking if the transactions
    were of equal quantities and only between these two facilities.
    In future, may eliminate checks if needed tables exist, and rely on errors.
    In future, may eliminate tests for the existance and uniqueness of the
    facilities. In addition, may eliminate other non-integration testing relevant
    code and tests.
    """
    # Cyclus simulation input for source_to_sink
    sim_inputs = ["./Inputs/source_to_sink.xml"]

    for sim_input in sim_inputs:
        holdsrtn = [1]  # needed because nose does not send() to test generator
        cmd = ["cyclus", "-o", "./output_temp.h5", "--input-file", sim_input]
        yield check_cmd, cmd, '.', holdsrtn
        rtn = holdsrtn[0]
        if rtn != 0:
            return  # don't execute further commands

        output = tables.open_file("./output_temp.h5", mode = "r")
        # tables of interest
        paths = ["/AgentEntry", "/Resources", "/Transactions", "/Info"]
        # Check if these tables exist
        yield assert_true, table_exist(output, paths)
        if not table_exist(output, paths):
            output.close()
            os.remove("./output_temp.h5")
            # This is a starter sqlite db created implicitly
            os.remove("./output_temp.sqlite")
            return  # don't execute further commands

        # Get specific tables and columns
        agent_entry = output.get_node("/AgentEntry")[:]
        info = output.get_node("/Info")[:]
        resources = output.get_node("/Resources")[:]
        transactions = output.get_node("/Transactions")[:]

        # Find agent ids of source and sink facilities
        agent_ids = agent_entry["AgentId"]
        agent_impl = agent_entry["Implementation"]
        duration = info["Duration"][0]

        source_id = find_ids("Source", agent_impl, agent_ids)
        sink_id = find_ids("Sink", agent_impl, agent_ids)

        # Test for only one source and one sink are deployed in the simulation
        yield assert_equal, len(source_id), 1
        yield assert_equal, len(sink_id), 1

        # Check if transactions are only between source and sink
        sender_ids = transactions["SenderId"]
        receiver_ids = transactions["ReceiverId"]
        expected_sender_array = np.empty(sender_ids.size)
        expected_sender_array.fill(source_id[0])
        expected_receiver_array = np.empty(receiver_ids.size)
        expected_receiver_array.fill(sink_id[0])
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
        expected_quantities.fill(quantities[0])

        yield assert_array_equal, quantities, expected_quantities

        output.close()
        os.remove("./output_temp.h5")
        # This is a starter sqlite db created implicitly
        os.remove("./output_temp.sqlite")

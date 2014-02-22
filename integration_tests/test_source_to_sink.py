#! /usr/bin/python

from nose.tools import assert_equal, assert_true
from numpy.testing import assert_array_equal
import os
import tables
import numpy as np
from tools import check_cmd

""" Tests """
def test_source_to_sink():
    """ Tests linear growth of sink inventory """
    #Cyclus simulation input for source_to_sink
    sim_inputs = ["./Inputs/source_to_sink.xml"]

    for sim_input in sim_inputs:
        holdsrtn = [1] # needed because nose does not send() to test generator
        cmd = ["cyclus", "-o", "./output_temp.h5", "--input-file", sim_input]
        check_cmd(cmd, '.', holdsrtn)
        rtn = holdsrtn[0]
        if rtn != 0:
            return # don't execute further commands

        output = tables.open_file("./output_temp.h5", mode = "r")
        # tables of interest
        paths = ["/Agents", "/Resources", "/Transactions","/TransactedResources"]
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
        agents = output.get_node("/Agents")[:]
        resources = output.get_node("/Resources")[:]
        transactions = output.get_node("/Transactions")[:]
        transacted_res = output.get_node("/TransactedResources")[:]

        # Find agent ids of source and sink facilities
        agent_ids = agents["ID"]
        agent_prototypes = agents["Prototype"]
        source_index = []
        sink_index = []
        i = 0
        for prototype in agent_prototypes:
            if prototype == "Source":
                source_index.append(i)
            elif prototype == "Sink":
                sink_index.append(i)
            i += 1
        # Test for only one source and one sink
        yield assert_equal, len(source_index), 1
        yield assert_equal, len(sink_index), 1
        # Get ids of the source and sink
        source_id = agent_ids[source_index[0]]
        sink_id = agent_ids[sink_index[0]]

        # Check if transactions are only between source and sink
        sender_ids = transactions["SenderID"]
        receiver_ids = transactions["ReceiverID"]
        expected_sender_array = np.empty(sender_ids.size)
        expected_sender_array.fill(source_id)
        expected_receiver_array = np.empty(receiver_ids.size)
        expected_receiver_array.fill(sink_id)
        yield assert_array_equal, sender_ids, expected_sender_array
        yield assert_array_equal, receiver_ids, expected_receiver_array

        # Transaction ids must be equal range from 1 to the number of rows
        # from both Transactions and TransactedResources tables
        expected_trans_ids = np.arange(1, sender_ids.size + 1, 1)
        yield assert_array_equal, transactions["ID"], expected_trans_ids
        yield assert_array_equal, transacted_res["TransactionID"], expected_trans_ids

        # Track transacted resources
        trans_resource_ids = transacted_res["ResourceID"]
        quantities = resources["Quantity"]
        transacted_quantities = np.zeros(trans_resource_ids.size)
        expected_quantities = np.empty(trans_resource_ids.size)
        # Expect that every transaction quantity is the same amount
        expected_quantities.fill(quantities[trans_resource_ids[0] - 1])
        j = 0
        for id in trans_resource_ids:
            transacted_quantities[j] = quantities[id-1]
            j += 1

        yield assert_array_equal, transacted_quantities, expected_quantities

        output.close()
        os.remove("./output_temp.h5")

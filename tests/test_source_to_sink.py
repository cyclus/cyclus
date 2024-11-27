#! /usr/bin/env python


from numpy.testing import assert_array_equal
import os
import sqlite3
import tables
import numpy as np
import pytest

from tools import check_cmd, cyclus_has_coin, thread_count
from helper import tables_exist, find_ids, \
    h5_suffix, clean_outs, to_ary, which_outfile

INPUT = os.path.join(os.path.dirname(__file__), "input")

@pytest.fixture(params=[("source_to_sink.xml", ":agents:Source", ":agents:Sink"),
                        ("source_to_sink.py", ":cyclus.pyagents:Source", ":cyclus.pyagents:Sink"),
                        ])
def source_to_sink_case(request):
    yield request.param

def test_source_to_sink(source_to_sink_case, thread_count):
    """Tests linear growth of sink inventory by checking if the transactions
    were of equal quantities and only between sink and source facilities.
    """
    clean_outs()

    fname, source_spec, sink_spec = source_to_sink_case

    if not cyclus_has_coin():
        pytest.skip("Cyclus does not have COIN")

    # Cyclus simulation input for Source and Sink
    sim_inputs = [os.path.join(INPUT, fname)]

    for sim_input in sim_inputs:
        holdsrtn = [1]  # needed because nose does not send() to test generator
        outfile = which_outfile(thread_count)
        cmd = ["cyclus", "-j", thread_count, "-o", outfile, "--input-file", sim_input]
        check_cmd(cmd, '.', holdsrtn)
        rtn = holdsrtn[0]
        if rtn != 0:
            return  # don't execute further commands

        # Tables of interest
        paths = ["/AgentEntry", "/Resources", "/Transactions", "/Info"]
        # Check if these tables exist
        assert  tables_exist(outfile, paths)
        if not tables_exist(outfile, paths):
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

        # Find agent ids of source and sink facilities
        agent_ids = to_ary(agent_entry, "AgentId")
        spec = to_ary(agent_entry, "Spec")

        source_id = find_ids(source_spec, spec, agent_ids)
        sink_id = find_ids(sink_spec, spec, agent_ids)

        # Test for only one source and one sink are deployed in the simulation
        assert len(source_id) == 1
        assert len(sink_id) == 1

        # Check if transactions are only between source and sink
        sender_ids = to_ary(transactions, "SenderId")
        receiver_ids = to_ary(transactions, "ReceiverId")
        expected_sender_array = np.empty(sender_ids.size)
        expected_sender_array.fill(source_id[0])
        expected_receiver_array = np.empty(receiver_ids.size)
        expected_receiver_array.fill(sink_id[0])
        assert_array_equal, sender_ids, expected_sender_array
        assert_array_equal, receiver_ids, expected_receiver_array

        # Transaction ids must be equal range from 1 to the number of rows
        expected_trans_ids = np.arange(0, sender_ids.size, 1)
        assert_array_equal, \
            to_ary(transactions, "TransactionId"),\
            expected_trans_ids

        # Track transacted resources
        resource_ids = to_ary(resources, "ResourceId")
        quantities = to_ary(resources, "Quantity")
        expected_quantities = np.empty(resource_ids.size)
        # Expect that every transaction quantity is the same amount
        expected_quantities.fill(quantities[0])

        assert_array_equal, quantities, expected_quantities

        clean_outs()




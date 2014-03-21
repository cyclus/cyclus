#! /usr/bin/python

from nose.tools import assert_false, assert_true, assert_equal, assert_not_equal
import os
import tables
import numpy as np
from tools import check_cmd
from helper import table_exist, find_ids, exit_times

""" Tests """
def test_null_sink():
    """ Testing for null sink case without a source facility.
    No transactions are expected in this test; therefore, a table with
    transaction records must not exist in order to pass this test.
    In addition, tests check if SimpleSink is deployed and decommissioned for
    this simulation. This may be removed in future.
    May also consider breaking this test into multiple tests.
    """
    # Cyclus simulation input for null sink testing
    sim_input = "./Inputs/null_sink.xml"
    holdsrtn = [1]  # needed because nose does not send() to test generator
    cmd = ["cyclus", "-o", "./output_temp.h5", "--input-file", sim_input]
    yield check_cmd, cmd, '.', holdsrtn
    rtn = holdsrtn[0]
    if rtn != 0:
        return  # don't execute further commands

    output = tables.open_file("./output_temp.h5", mode = "r")
    legal_paths = ["/AgentEntry", "/AgentExit", "/Info"]
    illegal_paths = ["/Transactions"]  # this must contain tables to test
    # Check if these tables exist
    yield assert_true, table_exist(output, legal_paths)
    if not table_exist(output, legal_paths):
        output.close()
        os.remove("./output_temp.h5")
        return  # don't execute further commands

    # Get specific data
    agent_entry = output.get_node("/AgentEntry")[:]
    agent_exit = output.get_node("/AgentExit")[:]
    info = output.get_node("/Info")[:]

    # SimpleSink's deployment and decommissioning
    agent_ids = agent_entry["AgentId"]
    agent_impl = agent_entry["Implementation"]
    duration = info["Duration"][0]

    sink_id = find_ids("SimpleSink", agent_impl, agent_ids)
    # Test if one SimpleSink is deployed
    yield assert_equal, len(sink_id), 1

    # Test if SimpleSink is decommissioned at the end of the simulation
    yield assert_equal, duration, exit_times(sink_id[0], agent_exit)


    # No resource exchange is expected
    yield assert_false, table_exist(output, illegal_paths)

    output.close()
    os.remove("./output_temp.h5")

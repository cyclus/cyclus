#! /usr/bin/python

from nose.tools import assert_false, assert_true, assert_equal, assert_not_equal
import os
import tables
import numpy as np
from tools import check_cmd

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
    check_cmd(cmd, '.', holdsrtn)
    rtn = holdsrtn[0]
    if rtn != 0:
        return  # don't execute further commands

    output = tables.open_file("./output_temp.h5", mode = "r")
    legal_paths = ["/AgentEntry", "/AgentExit", "/Info"]
    illegal_paths = ["/Transactions"]  # this must contain tables to test
    # Check if these tables exist
    tables_there = True
    for path in legal_paths:
        yield assert_true, output.__contains__(path)
        if tables_there and not output.__contains__(path):
            tables_there = False

    if not tables_there:
        output.close()
        os.remove("./output_temp.h5")
        return

    # Get specific data
    agent_entry = output.get_node("/AgentEntry")[:]
    agent_exit = output.get_node("/AgentExit")[:]
    info = output.get_node("/Info")[:]

    # SimpleSink's deployment and decommissioning
    agent_ids = agent_entry["AgentId"]
    agent_impl = agent_entry["Implementation"]
    duration = info["Duration"][0]

    sink_id = 0
    i = 0
    for agent in agent_impl:
        if agent == "SimpleSink":
            sink_id = agent_ids[i]
            break
        else:
            i += 1
    # Test if SimpleSink is actually deployed
    yield assert_not_equal, sink_id, 0

    # Test if SimpleSink is decommissioned at the end of the simulation
    i = 0
    for index in agent_exit["AgentId"]:
        if index == sink_id:
            yield assert_equal, duration, agent_exit["ExitTime"][i]
            break
        else:
            i += 1

    # No resource exchange is expected
    for path in illegal_paths:
        yield assert_false, output.__contains__(path)

    output.close()
    os.remove("./output_temp.h5")

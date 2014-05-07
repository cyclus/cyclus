#! /usr/bin/env python

from nose.tools import assert_false, assert_true, assert_equal
import os
import tables
import numpy as np
from tools import check_cmd
from helper import table_exist, find_ids, exit_times, \
    h5out, sqliteout, clean_outs

"""Tests"""
def test_null_sink():
    """Testing for null sink case without a source facility.

    No transactions are expected in this test; therefore, a table with
    transaction records must not exist in order to pass this test.
    """
    clean_outs()

    # Cyclus simulation input for null sink testing
    sim_input = "./input/null_sink.xml"
    holdsrtn = [1]  # needed because nose does not send() to test generator
    cmd = ["cyclus", "-o", h5out, "--input-file", sim_input]
    yield check_cmd, cmd, '.', holdsrtn
    rtn = holdsrtn[0]
    if rtn != 0:
        return  # don't execute further commands

    output = tables.open_file(h5out, mode = "r")
    legal_paths = ["/AgentEntry", "/Info"]
    illegal_paths = ["/Transactions"]  # this must contain tables to test
    # Check if these tables exist
    yield assert_true, table_exist(output, legal_paths)
    if not table_exist(output, legal_paths):
        output.close()
        clean_outs()
        return  # don't execute further commands

    # Get specific data
    agent_entry = output.get_node("/AgentEntry")[:]
    info = output.get_node("/Info")[:]

    # Sink's deployment
    agent_ids = agent_entry["AgentId"]
    agent_impl = agent_entry["Implementation"]

    sink_id = find_ids("Sink", agent_impl, agent_ids)
    # Test if one SimpleSink is deployed
    yield assert_equal, len(sink_id), 1

    # No resource exchange is expected
    yield assert_false, table_exist(output, illegal_paths)

    output.close()
    clean_outs()

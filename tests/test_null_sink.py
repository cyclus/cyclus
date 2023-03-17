#! /usr/bin/env python

import os
import sqlite3
import pytest



import numpy as np
import tables
from helper import tables_exist, find_ids, exit_times, \
    h5out, sqliteout, clean_outs, to_ary, which_outfile

from tools import check_cmd, cyclus_has_coin


INPUT = os.path.join(os.path.dirname(__file__), "input")

def check_null_sink(fname, given_spec):
    """Testing for null sink case without a source facility.

    No transactions are expected in this test; therefore, a table with
    transaction records must not exist in order to pass this test.
    """
    clean_outs()
    if not cyclus_has_coin():
        pytest.skip("Cyclus does not have COIN")

    # Cyclus simulation input for null sink testing
    sim_input = os.path.join(INPUT, fname)
    holdsrtn = [1]  # needed because nose does not send() to test generator
    outfile = which_outfile()
    cmd = ["cyclus", "-o", outfile, "--input-file", sim_input]
    check_cmd(cmd, '.', holdsrtn)
    rtn = holdsrtn[0]
    if rtn != 0:
        return  # don't execute further commands

    legal_paths = ["/AgentEntry", "/Info"]
    illegal_paths = ["/Transactions"]  # this must contain tables to test
    # Check if these tables exist
    assert  tables_exist(outfile, legal_paths)
    if not tables_exist(outfile, legal_paths):
        outfile.close()
        clean_outs()
        return  # don't execute further commands

    # Get specific data
    if outfile == h5out:
        output = tables.open_file(h5out, mode = "r")
        agent_entry = output.get_node("/AgentEntry")[:]
        info = output.get_node("/Info")[:]
        output.close()

    else:
        conn = sqlite3.connect(outfile)
        conn.row_factory = sqlite3.Row
        cur = conn.cursor()
        exc = cur.execute
        agent_entry = exc('SELECT * FROM AgentEntry').fetchall()
        info = exc('SELECT * FROM Info').fetchall()
        conn.close()

    # Sink's deployment
    agent_ids = to_ary(agent_entry, "AgentId")
    spec = to_ary(agent_entry, "Spec")

    sink_id = find_ids(given_spec, spec, agent_ids)
    # Test if one SimpleSink is deployed
    assert len(sink_id) == 1

    # No resource exchange is expected
    assert not tables_exist(outfile, illegal_paths)
    assert 1 == 2 # FIX ME
    clean_outs()


def test_null_sink():
    cases = [("null_sink.xml", ":agents:Sink"),
             ("null_sink.py", ":cyclus.pyagents:Sink")]
    for case in cases:
        for x in check_null_sink(*case):
            pass


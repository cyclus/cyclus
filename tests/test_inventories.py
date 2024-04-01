#! /usr/bin/env python

from numpy.testing import assert_array_equal
import os
import tables
import numpy as np
import sqlite3
from tools import check_cmd, thread_count
from helper import tables_exist, find_ids, exit_times, \
    h5out, sqliteout, clean_outs, to_ary, which_outfile

INPUT = os.path.join(os.path.dirname(__file__), "input")

def test_inventories_false(thread_count):
    """Testing for inventory and compact inventory table non-creation.
    """
    clean_outs()
    # Cyclus simulation input with inventory table
    sim_inputs = [os.path.join(INPUT, "inventory_false.xml"),
                  os.path.join(INPUT, "inventory_compact_false.xml")]
    paths = [["/ExplicitInventory"], ["/ExplicitInventoryCompact"]]
    for sim, path in zip(sim_inputs, paths):
        holdsrtn = [1]  # needed because nose does not send() to test generator
        outfile = sqliteout
        cmd = ["cyclus", "-j", thread_count, "-o", outfile, "--input-file", sim]
        check_cmd(cmd, '.', holdsrtn)
        rtn = holdsrtn[0]
        if rtn != 0:
            return  # don't execute further commands

        # Ensure tables do not exist
        assert not tables_exist(outfile, path)
        if tables_exist(outfile, path):
            print('Inventory table exists despite false entry in control section of input file.')
            outfile.close()
            clean_outs()
            return  # don't execute further commands

def test_inventories(thread_count):
    """Testing for inventory and compact inventory table creation.
    """
    clean_outs()
    # Cyclus simulation input with inventory table
    sim_inputs = [os.path.join(INPUT, "inventory.xml"),
                  os.path.join(INPUT, "inventory_compact.xml")]
    paths = [["/ExplicitInventory"], ["/ExplicitInventoryCompact"]]
    for sim, path in zip(sim_inputs, paths):
        holdsrtn = [1]  # needed because nose does not send() to test generator
        outfile = sqliteout
        cmd = ["cyclus", "-j", thread_count, "-o", outfile, "--input-file", sim]
        check_cmd(cmd, '.', holdsrtn)
        rtn = holdsrtn[0]
        if rtn != 0:
            return  # don't execute further commands

        # Check if inventory tables exist
        assert tables_exist(outfile, path)
        if not tables_exist(outfile, path):
            print('Inventory table does not exist despite true entry in control section of input file.')
            outfile.close()
            clean_outs()
            return  # don't execute further commands

        # Get specific table
        table = path[0]
        if outfile == h5out:
            output = tables.open_file(h5out, mode = "r")
            inventory = output.get_node(table)[:]
            compositions = output.get_node("/Compositions")[:]
            output.close()
        else:
            conn = sqlite3.connect(outfile)
            conn.row_factory = sqlite3.Row
            cur = conn.cursor()
            exc = cur.execute
            sqltable = table.replace('/', '')
            sql = "SELECT * FROM %s" % sqltable
            inventory = exc(sql).fetchall()
            compositions = exc('SELECT * FROM Compositions').fetchall()
            conn.close()

    # Test that quantity increases as expected with k=1
    if "Compact" not in table:
        time = to_ary(inventory, "Time")
        quantity = to_ary(inventory, "Quantity")
        nucid = to_ary(inventory, "NucId")
        massfrac = to_ary(compositions, "MassFrac")
        nucid_comp = to_ary(compositions, "NucId")
        expected_quantity = []
        expected_nucid = []
        repeat = 100
        for t in time:
            if t != repeat:
                expected_quantity.append((t+1)*massfrac[1])
                expected_nucid.append(nucid_comp[1])
            else:
                expected_quantity.append((t+1)*massfrac[0])
                expected_nucid.append(nucid_comp[0])
                t = repeat
                assert_array_equal, quantity, expected_quantity
                assert_array_equal, nucid, expected_nucid
                return  # don't execute further commands
    else:
        # composition will be the same in compact inventory
        return  # don't execute further commands

        clean_outs()


#! /usr/bin/env python

import nose

from nose.tools import assert_equal, assert_almost_equal, assert_true
from numpy.testing import assert_array_equal
import os
import tables
import numpy as np
from tools import check_cmd
from helper import table_exist, find_ids, exit_times, create_sim_input, \
    h5out, sqliteout, clean_outs, sha1array

def change_k_factors(fs_read, fs_write, k_factor_in, k_factor_out, n = 1):
    """Changes k_factor_in and k_factor_out for one facility.

    It is assumed that k_factor_in will precede k_factor_out in
    xml input file.

    Args:
        fs_read: Stream from a xml input file.
        fs_write: Output stream
        k_factor_in: A new conversion factor for requests.
        k_factor_out: A new conversion factor for offers.
        n: A number of sets of k_factors to be changed.

    Returns:
        A text with changed k_factors.

    Raises:
        AssertionError: if k_factor_in does not precede k_factor_out
    """
    k_in_changed = False
    i = 0  # to tract a number of sets

    for f in fs_read:
        if f.count("k_factor_in_"):
            assert(not k_in_changed)
            f = f.split("<")[0] + "<k_factor_in_>" + str(k_factor_in) + \
                "</k_factor_in_>\n"
            k_in_changed = True

        elif f.count("k_factor_out_"):
            assert(k_in_changed)
            f = f.split("<")[0] + "<k_factor_out_>" + str(k_factor_out) + \
                "</k_factor_out_>\n"
            k_in_changed = False
            i += 1

        fs_write.write(f)
        if i == n: break
    return

def change_minimal_input(ref_input, k_factor_a, k_factor_b):
    """Changes k_factors of facilities in the minimal cycle simulation.

    Args:
        k_factor_a: A new conversion factor for the KFacility A.
        k_factor_b: A new conversion factor for the KFacility B.

    Returns:
        A path to a file with updated conversion factors.
    """
    # A file to be created
    fw_path = ref_input.split(".xml")[0] + "_" + str(k_factor_a) + \
              "_" + str(k_factor_b) + ".xml"
    fw = open(fw_path, "w")
    fr = open(ref_input, "r")
    # change k_factors for the first facility
    change_k_factors(fr, fw, k_factor_b, k_factor_a)
    # reverse change k_factors for the second facility
    change_k_factors(fr, fw, k_factor_a, k_factor_b)
    # write the rest of the file
    for f in fr:
        fw.write(f)

    # Closing open files
    fr.close()
    fw.close()

    return fw_path

def test_minimal_cycle():
    """Tests simulations with two facilities with several conversion factors.

    The commodities of the facilities are different. Facility A offers a
    commodity A and requests commodity B; whereas, Facility B offers a
    commodity B and requests commodity A. In addition, it is also assumed that
    the first requests and offers are the same quantities for respective
    receivers and senders.
    The amount of the transactions follow a power law.

    Amount = InitialAmount * ConversionFactor ^ Time

    This equation is used to test each transaction amount.
    """
    # A reference simulation input for minimal cycle with different commodities
    ref_input = "./input/minimal_cycle.xml"
    # Conversion factors for the simulations
    k_factors = [0.95, 1, 2]

    for k_factor_a in k_factors:
        for k_factor_b in k_factors:
            clean_outs()
            sim_input = change_minimal_input(ref_input, k_factor_a, k_factor_b)

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
            #agent_entry = output.get_node("/AgentEntry")[:]
            print(output.root.AgentEntry)
            agent_entry = output.root.AgentEntry[:]
            info = output.get_node("/Info")[:]
            resources = output.get_node("/Resources")[:]
            transactions = output.get_node("/Transactions")[:]

            # Find agent ids
            agent_ids = agent_entry["AgentId"]
            agent_impl = agent_entry["Implementation"]
            agent_protos = agent_entry["Prototype"]
            duration = info["Duration"][0]

            facility_id = find_ids("KFacility:KFacility:KFacility", agent_impl, agent_ids)
            # Test for two KFacility
            yield assert_equal, len(facility_id), 2

            # Test for one Facility A and Facility B
            facility_a = find_ids("FacilityA", agent_protos, agent_ids)
            facility_b = find_ids("FacilityB", agent_protos, agent_ids)
            yield assert_equal, len(facility_a), 1
            yield assert_equal, len(facility_b), 1

            # Test if both facilities are KFracilities
            # Assume FacilityA is deployed first according to the schema
            yield assert_equal, facility_a[0], facility_id[0]
            yield assert_equal, facility_b[0], facility_id[1]

            # Test if the transactions are strictly between Facility A and
            # Facility B. There are no Facility A to Facility A or vice versa.
            sender_ids = transactions["SenderId"]
            receiver_ids = transactions["ReceiverId"]
            pattern_one = np.arange(0, sender_ids.size, 2)
            pattern_two = np.arange(1, sender_ids.size, 2)
            pattern_a = pattern_one  # expected pattern for Facility A as a sender
            pattern_b = pattern_two  # expected pattern for Facility B as a sender

            # Re-assign in case the expected patterns are incorrect
            if sender_ids[0] == facility_b[0]:
                pattern_a = pattern_two
                pattern_b = pattern_one

            yield assert_array_equal, np.where(sender_ids == facility_a[0])[0], \
                pattern_a
            yield assert_array_equal, np.where(receiver_ids == facility_a[0])[0], \
                pattern_b  # reverse pattern when acted as a receiver

            yield assert_array_equal, np.where(sender_ids == facility_b[0])[0], \
                pattern_b
            yield assert_array_equal, np.where(receiver_ids == facility_b[0])[0], \
                pattern_a  # reverse pattern when acted as a receiver

            # Transaction ids must be equal range from 1 to the number of rows
            expected_trans_ids = np.arange(sender_ids.size)
            yield assert_array_equal, transactions["TransactionId"], expected_trans_ids

            # When k_factors are very low and the simulation time is big
            # the number of transactions maybe shortened due to the lower
            # limit cyclus::eps() for transaction amounts.
            # Expect not to have shortened transactions, so for two facilities,
            # there must be (2 * duration) number of transactions.
            yield assert_equal, sender_ids.size, 2 * duration

            # Track transacted resources
            quantities = resources["Quantity"]

            # Almost equal cases due to floating point k_factors
            init_capacity_a = quantities[0]
            init_capacity_b = quantities[1]
            j = 0
            for p in pattern_a:
                yield assert_almost_equal, quantities[p], \
                    init_capacity_a * k_factor_a ** j
                j += 1

            j = 0
            for p in pattern_b:
                yield assert_almost_equal, quantities[p], \
                    init_capacity_b * k_factor_b ** j
                j += 1

            output.close()
            clean_outs()
            os.remove(sim_input)

if __name__ == "__main__":
    nose.runmodule()

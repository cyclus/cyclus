"""A set of tools for use in integration tests."""
import tables
import os

h5out = "output_temp.h5"
sqliteout = "output_temp.sqlite"

def clean_outs():
    """Removes output files if they exist."""
    if os.path.exists(h5out):
        os.remove(h5out)
    if os.path.exists(sqliteout):
        os.remove(sqliteout)
        

def table_exist(db, tables):
    """Checks if hdf5 database contains the specified tables.
    """
    return all([t in db.root for t in tables])

def find_ids(data, data_table, id_table):
    """Finds ids of the specified data located in the specified data_table,
    and extracts the corresponding id from the specified id_table.
    """
    ids = []
    i = 0
    for d in data_table:
        if d == data:
            ids.append(id_table[i])
        i += 1

    return ids

def exit_times(agent_id, exit_table):
    """Finds exit times of the specified agent from the exit table.
    """
    i = 0
    exit_times = []
    for index in exit_table["AgentId"]:
        if index == agent_id:
            exit_times.append(exit_table["ExitTime"][i])
        i += 1

    return exit_times

def create_sim_input(ref_input, k_factor_in, k_factor_out):
    """Creates xml input file from a reference xml input file.

    Changes k_factor_in_ and k_factor_out_ in a simulation input
    files for KFacility.

    Args:
        ref_input: A reference xml input file with k_factors.
        k_factor_in: A new k_factor for requests.
        k_factor_out: A new conversion factor for offers.

    Returns:
        A path to the created file. It is created in the same
        directory as the reference input file.
    """
    # File to be created
    fw_path = ref_input.split(".xml")[0] + "_" + str(k_factor_in) + \
              "_" + str(k_factor_out) + ".xml"
    fw = open(fw_path, "w")
    fr = open(ref_input, "r")
    for f in fr:
        if f.count("k_factor_in_"):
            f = f.split("<")[0] + "<k_factor_in_>" + str(k_factor_in) + \
                "</k_factor_in_>\n"
        elif f.count("k_factor_out_"):
            f = f.split("<")[0] + "<k_factor_out_>" + str(k_factor_out) + \
                "</k_factor_out_>\n"

        fw.write(f)

    # Closing open files
    fr.close()
    fw.close()

    return fw_path


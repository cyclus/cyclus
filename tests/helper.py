"""A set of tools for use in integration tests."""
import os
from hashlib import sha1

import numpy as np
import tables

def hasher(x):
    return int(sha1(x.encode()).hexdigest(), 16)

def idx(h):
    ind = [None] * 5
    for i in range(4, -1, -1):
        h, ind[i] = divmod(h, 2**32)
    return tuple(ind)

sha1array = lambda x: np.array(idx(hasher(x)), np.uint32)

h5out = "output_temp.h5"
sqliteout = "output_temp.sqlite"

def clean_outs():
    """Removes output files if they exist."""
    if os.path.exists(h5out):
        os.remove(h5out)
    if os.path.exists(sqliteout):
        os.remove(sqliteout)

def tables_exist(db, tables):
    """Checks if hdf5 database contains the specified tables.
    """
    return all([t in db.root for t in tables])

def find_ids(data, data_table, id_table):
    """Finds ids of the specified data located in the specified data_table,
    and extracts the corresponding id from the specified id_table.
    """
    ids = []
    for i, d in enumerate(data_table):
        if isinstance(d, np.ndarray) and isinstance(data, np.ndarray):
            if (d == data).all():
                ids.append(id_table[i])
        elif isinstance(d, np.ndarray) and not isinstance(data, np.ndarray):
            if (d == sha1array(data)).all():
                ids.append(id_table[i])
        elif d == data:
            ids.append(id_table[i])
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

def agent_time_series(f, names):
    """Return a list of timeseries corresponding to the number of agents in a
    Cyclus simulation

    Parameters
    ----------
    f : PyTables file
        the output file
    names : list
        the list of agent names
    """
    nsteps = f.root.Info.cols.Duration[:][0]
    entries = {name: [0] * nsteps for name in names}
    exits = {name: [0] * nsteps for name in names}

    # Get specific tables and columns
    agent_entry = f.get_node("/AgentEntry")[:]
    agent_exit = f.get_node("/AgentExit")[:] if hasattr(f.root, 'AgentExit') \
        else None

    # Find agent ids
    agent_ids = agent_entry["AgentId"]
    agent_type = agent_entry["Prototype"]
    agent_ids = {name: find_ids(name, agent_type, agent_ids) for name in names}

    # entries per timestep
    for name, ids in agent_ids.items():
        for id in ids:
            idx = np.where(agent_entry['AgentId'] == id)[0][0]
            entries[name][agent_entry[idx]['EnterTime']] += 1

    # cumulative entries
    entries = {k: [sum(v[:i+1]) for i in range(len(v))] \
                   for k, v in entries.items()}

    if agent_exit is None:
        return entries

    # entries per timestep
    for name, ids in agent_ids.items():
        for id in ids:
            idxs = np.where(agent_exit['AgentId'] == id)[0]
            if len(idxs) > 0:
                exits[name][agent_exit[idxs[0]]['ExitTime']] += 1

    # cumulative exits
    exits = {k: [sum(v[:i+1]) for i in range(len(v))] \
                 for k, v in exits.items()}

    # return difference
    ret = {}
    for name in names:
        i = entries[name]
        # shift by one to account for agents that enter/exit in the same
        # timestep
        o = [0] + exits[name][:-1]
        ret[name] = [i - o for i, o in zip(i, o)]

    return ret

def create_sim_input(ref_input, k_factor_in, k_factor_out):
    """Creates xml input file from a reference xml input file.

    Changes k_factor_in and k_factor_out in a simulation input
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
        if f.count("k_factor_in"):
            f = f.split("<")[0] + "<k_factor_in>" + str(k_factor_in) + \
                "</k_factor_in>\n"
        elif f.count("k_factor_out"):
            f = f.split("<")[0] + "<k_factor_out>" + str(k_factor_out) + \
                "</k_factor_out>\n"

        fw.write(f)

    # Closing open files
    fr.close()
    fw.close()

    return fw_path

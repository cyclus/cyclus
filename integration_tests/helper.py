""" A set of tools for use in integration tests. """
import tables

def table_exist(db, tables):
    """ Checks if hdf5 database contains the specified tables.
    """
    for t in tables:
        if not db.__contains__(t):
            return False

    return True

def find_ids(data, data_table, id_table):
    """ Finds ids of the specified data located in the specified data_table,
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
    """ Finds exit times of the specified agent from the exit table.
    """
    i = 0
    exit_times = []
    for index in exit_table["AgentId"]:
        if index == agent_id:
            exit_times.append(exit_table["ExitTime"][i])
        i += 1

    return exit_times


